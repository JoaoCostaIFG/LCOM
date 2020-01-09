#include <lcom/lcf.h>

#include "i8042.h"
#include "i8254.h"
#include "kbc_utils.h"
#include "utils.h"

/* global variables */
extern uint8_t g_out_buf;
uint8_t g_out_buf;
static unsigned int g_timer_cnt = 0;


void
(timer_int_handler)()
{
	g_timer_cnt++;
}


int
kbd_restore_conf()
{
	/* get current keyboard config */
	uint8_t current_conf;
	if (kbc_get_conf(&current_conf))
		return 1;

	/* editing curent conf */
	current_conf |= KBC_CONF_INTEN;  // enable interrupts
	current_conf &= ~KBC_CONF_FULDIS;  // enable interfaces

	/* write new keyboard config byte */
	if (kbc_write_conf(current_conf))
		return 1;

	return 0;
}


int
kbd_scancode(bool *make, uint8_t *size, uint8_t *bytes)
{
	/* if the scancode is a 2 byte scancode, save first byte in array and next time this function is called, save next byte */
	if (g_out_buf == SECONDBYTE) {  // first byte of 2 byte scancode
		bytes[0] = g_out_buf;
		*size = 1;
		return 1;
	}

	bytes[*size] = g_out_buf;

	*make = true;
	if (g_out_buf & BIT(7))
		*make = false;

	return 0;
}


int
kbd_iloop(const int *irq_sel)
{
	int ipc_status, r;
	message msg;

	/* variables that control the print of the scancode */
	uint8_t size = 0;
	uint8_t bytes[2];
	bool make;

	while (g_out_buf != ESCBREAK) {
		/* get a request message. */
		r = driver_receive(ANY, &msg, &ipc_status);
		if (r != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {  // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:  // hardware interrupt notification
					if (msg.m_notify.interrupts & *irq_sel) {  // subscribed interrupt
						if (!check_status_read(false)) {  // test if the byte was read correctly
							kbc_ih();  // call keyboard handler

							if (!kbd_scancode(&make, &size, bytes)) {  // test if 2 byte scancode
								size++;
								if (kbd_print_scancode(make, size, bytes))
									return 1;
								size = 0;

							}
						}

					}

					break;

				default:
					break;  // no other notifications expected: do nothing
			}
		}
	}

	return 0;
}


int
kbd_scan()
{
	/* subscribe to keyboard */
	int hook_id = KBD_IRQ;  // Initialize
	const int irq_sel = BIT(KBD_IRQ);

	if (subscribe_int(&hook_id, KBD_IRQ, true))  // we want exclusivity with the ints
		return 1;

	if (kbd_iloop(&irq_sel))
		return 1;

	if (unsubscribe_int(&hook_id))
		return 1;

	return 0;
}


int
kbd_poll()
{
	/*
	 * Polling make more 'sys_inb' calls than using interrupts because it is constantly trying to read the
	 * keyboard status and output buffer, even if no keys have pressed at the time (unlike the interrupt method)
	 */

	/* variables that control the print of the scancode */
	uint8_t size = 0;
	uint8_t bytes[2];
	bool make;

	while (g_out_buf != ESCBREAK) {
		tickdelay(micros_to_ticks(DELAY_US));  // nighty night
		// porquê? Quem manda? Ai ai ai ai ai.. Tão-me a ligar, perem aí.
		// ou uma coisa ou outra nao é a mesma coisa que uma coisa ou outra

		if (!check_status_read(false)) {  // test if the byte was read correctly
			kbc_ih();  // call keyboard handler

			if (!kbd_scancode(&make, &size, bytes)) {  // test if 2 byte scancode
				size++;
				if (kbd_print_scancode(make, size, bytes))
					return 1;
				size = 0;
			}
		}
	}

	if (kbd_restore_conf())
		return 1;

	return 0;
}


int
timed_kbd_iloop(const int *kbd_irq_sel, const int *timer0_irq_sel, uint8_t delay)
{
	uint8_t elapsed_time = 0;
	int ipc_status, r;
	message msg;

	/* variables that control the print of the scancode */
	uint8_t size = 0;
	uint8_t bytes[2];
	bool make;

	/* loops stops when ESC key is released or no keystroke was registered for a duration >= delay */
	while (elapsed_time < delay && g_out_buf != ESCBREAK) {
		/* get a request message. */
		r = driver_receive(ANY, &msg, &ipc_status);
		if (r != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {  // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:  // hardware interrupt notification
					if (msg.m_notify.interrupts & *kbd_irq_sel) {  // subscribed keyboard interrupt
						if (!check_status_read(false)) {  // test if the byte was read correctly
							kbc_ih();  // call keyboard handler

							if (!kbd_scancode(&make, &size, bytes)) {  // test if 2 byte scancode
								size++;
								if (kbd_print_scancode(make, size, bytes))
									return 1;
								size = 0;

							}
							elapsed_time = 0;  // reset the elapsed time since last successful keystroke
							g_timer_cnt = 0;  // reset current time couting
						}

					}
					else if (msg.m_notify.interrupts & *timer0_irq_sel) {  // subscribed timer0 interrupt
						timer_int_handler();  // call timer0 handler

						/* counts seconds and increments 'elapsed_time' for each one */
						if (g_timer_cnt == TIMER0_FREQ) {
							elapsed_time++;
							g_timer_cnt = 0;
						}

					}


					break;

				default:
					break;  // no other notifications expected: do nothing
			}
		}
	}

	return 0;
}


int
kbd_timed_scan(uint8_t *delay)
{
	/* subscribing keyboard ints */
	int kbd_hook_id = KBD_IRQ;
	const int kbd_irq_sel = BIT(KBD_IRQ);

	if (subscribe_int(&kbd_hook_id, KBD_IRQ, true))  // we want exclusive ints
		return 1;

	/* subscribing timer ints */
	int timer0_hook_id = TIMER0_IRQ;
	const int timer0_irq_sel = BIT(TIMER0_IRQ);

	if (subscribe_int(&timer0_hook_id, TIMER0_IRQ, false))  // not exclusive use
		return 1;


	/* interrupt loop */
	if (timed_kbd_iloop(&kbd_irq_sel, &timer0_irq_sel, *delay))
		return 1;


	/* unsubscribe keyboard and timer */
	if (unsubscribe_int(&kbd_hook_id))
		return 1;
	if (unsubscribe_int(&timer0_hook_id))
		return 1;

	return 0;
}

