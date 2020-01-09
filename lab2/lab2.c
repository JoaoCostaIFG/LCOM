#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

/* constants for interrupts */
#define TIMER0_IRQ	0		/**< @brief Timer 0 IRQ line */
#define TIMER0_FREQ	60		/**< @brief Timer 0 Frequency */

/* global Variables */
extern uint8_t g_timer_counter;  // counter increased by the interrupt handler


int main(int argc, char *argv[]) {

	// sets the language of LCF messages (can be either EN-US or PT-PT)
	lcf_set_language("EN-US");

	// enables to log function invocations that are being "wrapped" by LCF
	// [comment this out if you don't want/need it]
	// lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

	// enables to save the output of printf function calls on a file
	// [comment this out if you don't want/need it]
	// lcf_log_output("/home/lcom/labs/lab2/output.txt");

	// handles control over to LCF
	// [LCF handles command line arguments and invokes the right function]

	if (lcf_start(argc, argv))
		return 1;

	// LCF clean up tasks
	// [must be the last statement before return]
	lcf_cleanup();

	return 0;
}


int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
	/*
	 * Tests display of timer config.
	 */

	/* getting the timer, 'timer', configuration byte */
	uint8_t timer_conf;
	if (timer_get_conf(timer, &timer_conf))
		return 1;

	/* parsing and printing the configuration byte */
	if(timer_display_conf(timer, timer_conf, field))
		return 1;

	return 0;
}


int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
	/*
	 * Tests change of Timer O interrupt frequency.
	 */

	if (timer_set_frequency(timer, freq))
		return 1;

	return 0;
}


int(timer_test_int)(uint8_t time) {
	/*
	 * Tests Timer 0 interrupt handling.
	 */

	if (time < 0)  // 'time' can't have a negative value
		return 1;

	/* subscribe to timer */
	uint8_t bit_no = TIMER0_IRQ;  // use timer0's irq line as input to make it simpler
	if (timer_subscribe_int(&bit_no))
		return 1;

	int ipc_status, r;
	message msg;
	uint32_t irq_sel = BIT(TIMER0_IRQ);

	while(time > 0) {
		/* get a request message. */
		r = driver_receive(ANY, &msg, &ipc_status);
		if (r != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: // hardware interrupt notification
					if (msg.m_notify.interrupts & irq_sel) { // subscribed interrupt
						timer_int_handler(); // increment counter

						if (g_timer_counter == TIMER0_FREQ) {
							g_timer_counter = 0; // reset Counter
							time--; // decrement 1 second from timer
							timer_print_elapsed_time();
						}

					}
					break;

				default:
					break; // no other notifications expected: do nothing
			}
	       }
	}

	if (timer_unsubscribe_int())
		return 1;

	return 0;
}
