#include <lcom/lcf.h>

#include "i8042.h"
#include "i8254.h"
#include "kbc_utils.h"
#include "utils.h"
#include "mouse.h"


static uint8_t g_out_buf = 0;
static bool byte_valid = false;
void
(mouse_ih)()
{
	/* get current byte on the output buffer */
	if (!check_status_read(true)) {  // check for errors (is mouse data)
		byte_valid = true;
		util_sys_inb(KBC_IO_BUF, &g_out_buf);
	}
	else
		byte_valid = false;
}


static unsigned int g_timer_cnt = 0;
void
(timer_int_handler)()
{
	g_timer_cnt++;
}


transitions_enum
transition_calc(struct packet *pp, struct packet *pp_old, uint8_t x_len, int *curr_x, uint8_t tolerance)
{
	transitions_enum curr_transition;

	/* button action handling (press and release) */
	if (pp->mb || (pp->lb && pp->rb)) {  // middle click is always wrong
		curr_transition = FAIL;
		/**curr_state = ACCEPT;  // easy way to kill program while testing*/
		/*return;*/
	}
	else if (!pp_old->lb && pp->lb) {  // left button pressed
		curr_transition = LCLK;
		*curr_x = 0;  // reset x counting
	}
	else if (pp_old->lb && !pp->lb) {  // left button released
		if (*curr_x < x_len)
			curr_transition = FAIL;
		else
			curr_transition = LRE;
	}
	else if (!pp_old->rb && pp->rb) {  // right button pressed
		curr_transition = RCLK;
		*curr_x = 0;  // reset x counting
	}
	else if (pp_old->rb && !pp->rb) {  // right button released
		if (*curr_x < x_len)
			curr_transition = FAIL;
		else
			curr_transition = RRE;
	}
	else {
		/* update traveled distance */
		*curr_x += pp->delta_x;

		/* check if the movements were inside the tolerance and/or overflows */
		bool pass_tol_check = false;

		if (abs((int) pp->delta_x) <= tolerance &&
		    abs((int) pp->delta_y) <= tolerance &&
		    !pp->x_ov &&
		    !pp->y_ov)
			pass_tol_check = true;

		/* check movement slope and tolerance */
		float curr_slope = slope_calc((int) pp->delta_x, (int) pp->delta_y);

		if (curr_slope >= 0) {
			if (fabs(curr_slope) <= 1 && pp->lb && pp->delta_y >= 0)  // direcao correta, declive <= 1
				curr_transition = FAIL;
			else if (pass_tol_check)  // tolerance check is OK
				curr_transition = ASC_TOL;
			else if (pp->delta_y < 0)  // tolerance check is not OK, and we moved backwards (slope > 0 and y < 0 --> x < 0)
				curr_transition = FAIL;
			else
				curr_transition = ASC_NTOL;
		}
		else {
			if (fabs(curr_slope) <= 1 && pp->rb && pp->delta_y <= 0)  // direcao correta, declive <= 1
				curr_transition = FAIL;
			else if (pass_tol_check)  // tolerance check is OK
				curr_transition = DES_TOL;
			else if (pp->delta_y > 0)  // tolerance check is not OK, and we moved backwards (slope < 0 and y > 0 --> x < 0)
				curr_transition = FAIL;
			else
				curr_transition = DES_NTOL;
		}

	}

	return curr_transition;
}


void
(event_ih)(struct packet *pp, struct packet *pp_old, state_enum *curr_state, uint8_t x_len, int *curr_x, uint8_t tolerance)
{
	/* check pointers cause yes */
	if (!pp || !pp_old || !curr_state || !curr_x)
		return;

	state_enum event_matrix[][8] = {
		{INIT,	      INIT,	   INIT,	INIT,	    SLOPE_LEFT, INIT,	     INIT,    INIT},
		{SLOPE_LEFT,  INIT,	   SLOPE_LEFT,	SLOPE_LEFT, INIT,	INIT,	     VERTICE, INIT},
		{VERTICE,     INIT,	   VERTICE,	INIT,	    INIT,	SLOPE_RIGHT, INIT,    INIT},
		{SLOPE_RIGHT, SLOPE_RIGHT, SLOPE_RIGHT, INIT,	    INIT,	INIT,	     INIT,    ACCEPT},
		// no need to define matrix for the acceptance state
	};

	/* get current transition (pointers already checked) */
	transitions_enum curr_transition = transition_calc(pp, pp_old, x_len, curr_x, tolerance);

	/* transition to new state */
	if (curr_transition == FAIL) {
		*curr_x = 0;
		*curr_state = INIT;
	}
	else
		*curr_state = event_matrix[*curr_state][curr_transition];
}


int
mouse_sync_packet(uint8_t *curr_byte, struct packet *pp, uint32_t *cnt)
{
	if (!curr_byte || !pp)
		return 1;

	if (*curr_byte > 0) {
		pp->bytes[*curr_byte] = g_out_buf;

		if (*curr_byte == 2) {
			*curr_byte = 0;

			if (cnt != NULL)
				--(*cnt);

			/* parse packet info to 'struct packet' data type and print its info */
			mouse_parse_packet(pp);
			mouse_print_packet(pp);
		}
		else
			++(*curr_byte);
	}
	else if (g_out_buf & MOU_1ST_PACKET_BIT) {  // check if byte is a valid first byte (curr_cyte == 0)
		pp->bytes[*curr_byte] = g_out_buf;
		++(*curr_byte);
	}

	return 0;
}


int
mouse_iloop(const int *irq_sel, uint32_t cnt)
{
	int ipc_status, r;
	message msg;
	uint8_t curr_byte = 0;
	struct packet pp;

	while (cnt > 0) {
		/* get a request message. */
		r = driver_receive(ANY, &msg, &ipc_status);
		if (r != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {  // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:	// hardware interrupt notification
					if (msg.m_notify.interrupts & *irq_sel) {  // subscribed interrupt
						/* calls mouse interupt handler */
						mouse_ih();

						/* sync mouse packet's bytes for parsing */
						if (byte_valid) {
							if (mouse_sync_packet(&curr_byte, &pp, &cnt))
								return 1;
						}
						else {
							/* if reading fails, we start reading the packet from the beggining */
							curr_byte = 0;
						}
					}

					break;

				default:
					break;	// no other notifications expected: do nothing
			}
		}
	}

	return 0;
}


int
mouse_packet(uint32_t *cnt)
{
	/* check if cnt is valid */
	if (!cnt)
		return 1;

	/* set stream mode and enable data reporting */
	if (mouse_en_data_report())
		return 1;

	/* subscribe mouse interrupts */
	int hook_id = MOU_IRQ;
	const int irq_sel = BIT(MOU_IRQ);
	if (subscribe_int(&hook_id, MOU_IRQ, true))  // subscribe the interupts in EXCLUSIVE mode
		return 1;

	/* handle the incoming mouse packets */
	if (mouse_iloop(&irq_sel, *cnt))
		return 1;

	/* unsubscribe mouse interrupts */
	if (unsubscribe_int(&hook_id))
		return 1;

	/* disable data reporting */
	if (mouse_write_cmd(MOU_DISABLE_DATA_REPORT_CMD))
		return 1;

	return 0;
}


int
mouse_remote_poll(uint16_t *period, uint8_t cnt_8)
{
	/* period pointer was already verified */

	uint32_t delay = *period * 1000;  // convert ms to microseconds
	delay = micros_to_ticks(delay);  // convert microseconds to ticks

	uint32_t cnt = (uint32_t) cnt_8;  // casting cause LCOM is consistent
	uint8_t curr_byte = 0;
	struct packet curr_packet;

	bool packet_finished = false;

	while (cnt > 0) {
		/* ask for 1 packet to be sent */
		if (mouse_write_cmd(MOU_READ_DATA_CMD))
			return 1;

		packet_finished = false;
		while (!packet_finished) {
			mouse_ih();

			/* sync mouse packet's bytes for parsing */
			if (byte_valid) {
				if (curr_byte == 2){
					if (mouse_sync_packet(&curr_byte, &curr_packet, &cnt))
						return 1;
					else
						packet_finished = true;  // leave while loop when we get 1 full packet
				}
				else {
					if (mouse_sync_packet(&curr_byte, &curr_packet, &cnt))
						return 1;
				}
			}
			else {
				/* if reading fails, we start reading a new packet from the beggining */
				packet_finished = true;  // leave while loop
				curr_byte = 0;
			}
		}

		tickdelay(micros_to_ticks(DELAY_US));  // nighty night

		tickdelay(delay);  // nighty night
	}

	return 0;
}


int
mouse_remote(uint16_t *period, uint8_t *cnt)
{
	/* check if variables are valid */
	if (!period ||
	    !cnt)  // period must be an integer (0, 999]
		return 1;

	/* call poll loop */
	if (mouse_remote_poll(period, *cnt))
		return 1;

	/* restore minix's defaults kbc and mouse configurations */
	/* set stream mode with data reporting disabled */
	if (mouse_set_stream_mode())
		return 1;

	/* restore kbc's minix default configuration */
	if (kbc_restore_dflt_conf())
		return 1;

	return 0;
}


int
mouse_async_iloop(const int *mou_irq_sel, const int *timer0_irq_sel, uint8_t *idle_time)
{
	int ipc_status, r;
	message msg;
	uint8_t curr_byte = 0;
	struct packet pp;

	unsigned int elapsed_time = 0;
	const unsigned int timer0_freq = sys_hz();  // get timer0's frequency

	while (elapsed_time < *idle_time) {
		/* get a request message. */
		r = driver_receive(ANY, &msg, &ipc_status);
		if (r != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {  // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:	// hardware interrupt notification
					if (msg.m_notify.interrupts & *mou_irq_sel) {  // subscribed mouse interrupt
						/* calls mouse interupt handler */
						mouse_ih();


						/* sync mouse packet's bytes for parsing */
						if (byte_valid) {
							if (mouse_sync_packet(&curr_byte, &pp, NULL))
								return 1;

							/* reset the elapsed time since last mouse movement */
							if (curr_byte == 0) {  // movement only ocurres on the first byte
								elapsed_time = 0;
								g_timer_cnt = 0;  // reset current time couting
							}
						}
						else {
							/* if reading fails, we start reading the packet from the beggining */
							curr_byte = 0;
						}


					}
					else if (msg.m_notify.interrupts & *timer0_irq_sel) {  // subscribed timer0 interrupt
						/* calls timer0 interrupt handler */
						timer_int_handler();

						/* counts seconds and increments 'elapsed_time' for each one */
						if (g_timer_cnt == timer0_freq) {
							elapsed_time++;
							g_timer_cnt = 0;
						}
					}

					break;

				default:
					break;	// no other notifications expected: do nothing
			}
		}
	}

	return 0;
}



int
mouse_async(uint8_t *idle_time)
{
	if (!idle_time)
		return 1;

	/* set stream mode and enable data reporting */
	if (mouse_en_data_report())
		return 1;

	/* subscribe mouse interrupts */
	int mou_hook_id = MOU_IRQ;
	const int mou_irq_sel = BIT(MOU_IRQ);
	if (subscribe_int(&mou_hook_id, MOU_IRQ, true))  // subscribe the interupts in EXCLUSIVE mode
		return 1;

	/* subscribe timer0 interrupts */
	int timer0_hook_id = TIMER0_IRQ;
	const int timer0_irq_sel = BIT(TIMER0_IRQ);
	if (subscribe_int(&timer0_hook_id, TIMER0_IRQ, false))	// subscribe the interupts
		return 1;

	/* handle the incoming mouse packets */
	if (mouse_async_iloop(&mou_irq_sel, &timer0_irq_sel, idle_time))
		return 1;

	/* unsubscribe mouse interrupts */
	if (unsubscribe_int(&mou_hook_id))
		return 1;

	/* unsubscribe timer0 interrupts */
	if (unsubscribe_int(&timer0_hook_id))
		return 1;

	/* disable data reporting */
	if (mouse_write_cmd(MOU_DISABLE_DATA_REPORT_CMD))
		return 1;

	return 0;
}


int
mouse_gesture_iloop(const int *irq_sel, uint8_t *x_len, uint8_t *tolerance)
{
	int ipc_status, r;
	message msg;

	uint8_t curr_byte = 0;
	int curr_x = 0;
	struct packet pp;
	struct packet pp_old = {{0, 0, 0}, 0, 0, 0, 0, 0, 0, 0};  // default constructor
	state_enum curr_state = INIT;

	while (curr_state != ACCEPT) {
		/* get a request message. */
		r = driver_receive(ANY, &msg, &ipc_status);
		if (r != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {  // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:	// hardware interrupt notification
					if (msg.m_notify.interrupts & *irq_sel) {  // subscribed interrupt
						/* calls mouse interupt handler */
						mouse_ih();

						/* sync mouse packet's bytes for parsing */
						if (byte_valid) {
							if (mouse_sync_packet(&curr_byte, &pp, NULL))
								return 1;

							if (curr_byte == 0) {  // just processed last byte of packet
								event_ih(&pp, &pp_old, &curr_state, *x_len, &curr_x, *tolerance);
							}
						}
						else {
							/* if reading fails, we start reading the packet from the beggining */
							curr_byte = 0;
						}

						pp_old = pp;
					}
					break;

				default:
					break;	// no other notifications expected: do nothing
			}
		}
	}

	return 0;
}


int
mouse_gesture(uint8_t *x_len, uint8_t *tolerance)
{
	/* check if function arguments are valid */
	if (!x_len || !tolerance)
		return 1;

	/* set stream mode and enable data reporting */
	if (mouse_en_data_report())
		return 1;

	/* subscribe mouse interrupts */
	int hook_id = MOU_IRQ;
	const int irq_sel = BIT(MOU_IRQ);
	if (subscribe_int(&hook_id, MOU_IRQ, true))  // subscribe the interupts in EXCLUSIVE mode
		return 1;

	/* handle the incoming mouse packets */
	if (mouse_gesture_iloop(&irq_sel, x_len, tolerance))
		return 1;

	/* unsubscribe mouse interrupts */
	if (unsubscribe_int(&hook_id))
		return 1;

	/* disable data reporting */
	if (mouse_write_cmd(MOU_DISABLE_DATA_REPORT_CMD))
		return 1;

	return 0;
}
