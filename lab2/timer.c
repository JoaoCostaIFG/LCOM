#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

/* global variables */
extern uint8_t g_timer_counter;
uint8_t g_timer_counter = 0;
static int g_hook_id;  // hook_id needs to be global in this file because we have no other way to pass it timer_unsubscribe_int


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
	/*
	 * Changes the operating frequency of a timer.
	 */

	/* freq need to be >=19 because the largest supported counter value is 2^16 counter=clock/frequency */
	/* freq also needs to be lower than the CPU's clock */
	if (freq < 19 ||
	    freq > 1193182)
		return 1;

	/* reads the current config to be changed (can't change last 4 bits) */
	/* the 'timer_get_conf' function already verifies 'timer' range */
	uint8_t timer_conf;
	if (timer_get_conf(timer, &timer_conf))
		return 1;

	/* Keeping the last 4 bits of the current config */
	timer_conf &= (TIMER_OP_MASK | BIT(0));  // 0b1111

	/* Selecting the timer, 'timer', to config */
	int port;
	if (timer == 0) {
		port = TIMER2PORT(0);
		timer_conf |= SEL_TIMER(0);

	}
	else if (timer == 1) {
		port = TIMER2PORT(1);
		timer_conf |= SEL_TIMER(1);

	}
	else {
		port = TIMER2PORT(2);
		timer_conf |= SEL_TIMER(2);

	}

	/* selecting init. mode */
	uint16_t timer_ival =  TIMER_FREQ / freq;  // freq = clock / timer_ival

	uint8_t lsb_load, msb_load;
	util_get_LSB(timer_ival, &lsb_load);  // get least significant bits from 'timer_ival'
	util_get_MSB(timer_ival, &msb_load);  // get most significant bits from 'timer_ival'

	/* write counter to 'TIMER_CTRL' register */
	timer_conf |= TIMER_LSB_MSB;
	sys_outb(TIMER_CTRL, timer_conf);
	sys_outb(port, lsb_load);
	sys_outb(port, msb_load);
	/*
	 * we tried different initialization modes (only LSB or only MSB) depending on the calculated 'timer_ival'
	 * (counter value) but the test failed when doing so.
	 * we believe this is due to the fact that lcom_run utility was expecting us to always write both LSB and MSB.
	 */

	return 0;
}


int (timer_subscribe_int)(uint8_t *bit_no) {
	/*
	 * Subscribes and enables Timer 0 interrupts.
	 */

	/* check if input for 'sys_irqsetpolicy' was given */
	if (!bit_no)
		return 1;

	/* initialize timer counter */
	g_timer_counter = 0;

	/* subscribes Timer 0 to 'bit_no' */
	g_hook_id = (int) *bit_no;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &g_hook_id))
		return 1;

	/*
	 * return the casted 'hook_id' to the caller function but keep it saved in a global variable for
	 * use in 'timer_unsubscribe_int'
	 */
	*bit_no = (uint8_t) g_hook_id;

	return 0;
}


int (timer_unsubscribe_int)() {
	/*
	 * Unsubscribes Timer 0 interrupts.
	 */

	if (sys_irqrmpolicy(&g_hook_id))
		return 1;

	return 0;
}


void (timer_int_handler)() {
	/*
	 * Timer 0 interrupt handler.
	 */

	g_timer_counter++;
}


int (timer_get_conf)(uint8_t timer, uint8_t *st) {
	/*
	 * Reads the input timer configuration (status) via read-back command.
	*/

	/* 'timer' must be a value in range: [0, 2] */
	if (timer > 2 || timer < 0)
		return 1;

	/* check for NULL/uninitialized pointer */
	if (!st)
		return 1;

	/* Convert 'timer' to port addr */
	int port;  // TIMER2PORT(timer);

	/* Create writeback command */
	uint8_t writeback_cmd=TIMER_RB_CMD | TIMER_RB_COUNT_; // 0b11100000
	if (timer == 0) {
		port = TIMER2PORT(0);
		writeback_cmd |= TIMER_RB_SEL(0); // 0b10
	}
	else if (timer == 1) {
		port = TIMER2PORT(1);
		writeback_cmd |= TIMER_RB_SEL(1); // 0b100
	}
	else {
		port = TIMER2PORT(2);
		writeback_cmd |= TIMER_RB_SEL(2); // 0b1000
	}

	sys_outb(TIMER_CTRL, writeback_cmd); // Writing writeback_cmd ti TIMER_CTRL addr
	if (util_sys_inb(port, st))
		return 1;

	return 0;
}


int (timer_display_conf)(uint8_t timer, uint8_t st,
			enum timer_status_field field) {
	/*
	 * Shows timer configuration.
	 */

	/* 'timer' must be a value in range: [0, 2] */
	if (timer > 2 || timer < 0)
		return 1;

	union timer_status_field_val timer_info;
	/* 'field' must be a value in range: [0, 3] */
	if (field == 0)
		/* tsf_all */
		timer_info.byte = st;

	else if (field == 1) {
		/* tsf_initial */
		st &= TIMER_INIT_MASK;
		st = st >> 4;

		enum timer_init input_mode_if[] = {INVAL_val, LSB_only, MSB_only, MSB_after_LSB};
		timer_info.in_mode = input_mode_if[st]; // St is always a number between [0, 3]

	}
	else if (field == 2) {
		/* tsf_mode */
		st &= TIMER_OP_MASK;
		st = st >> 1;

		/* Special cases that assure compatibility with future intel products */
		if (st >= 6) // 0b110 -> 0b010 && 0b111 -> 0b011 (subtracting 4)
			st -= 4;
		timer_info.count_mode = st;

	}
	else if (field == 3) {
		/* tsf_base */
		st &= BIT(0);
		timer_info.bcd = st;

	}
	else
		return 1;

	/* Print config in a human friendly way */
	timer_print_config(timer, field, timer_info);

	return 0;
}
