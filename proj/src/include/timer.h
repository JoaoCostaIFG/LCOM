/** @file timer.h */
#ifndef __TIMER_H__
#define __TIMER_H__

#include <lcom/timer.h>
#include <stdint.h>

#include "include/i8254.h"

/** @addtogroup	timer_grp
  * @{
  */

/* INTERRUPT HANDLER */
/** @brief	Interrupt handler for the timer 0. */
void timer_ih(void);

/**
 * @brief	Get the number of timer ticks since the last reset.
 * @return	The number of timer ticks since the last reset.
 */
uint32_t get_timer_cnt(void);

/** @brief	Reset timer tick counter. */
void timer_reset(void);

/** @brief	Reset timer 0's tick count. */
void timer_reset(void);

/* TIMER CONFIG FUNCTIONS */
/**
 * @brief	Set a given timer's frequency to the given one.
 *
 * @param timer	Timer to set the frequency [0, 2].
 * @param freq	Frequency to set.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int timer_set_freq(uint8_t timer, uint32_t freq);

/**
 * @brief	Get a given timer's configuration byte.
 *
 * @param timer	Timer to get the configuration byte of [0, 2].
 * @param st	Returned configuration byte.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int timer_get_config(uint8_t timer, uint8_t *st);

/**
 * @brief	Display a given timer's current configuration
 *
 * @param timer Timer to display the configuration of [0, 2].
 * @param field	The field we want to display.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int timer_display_curr_conf(uint8_t timer, enum timer_status_field field);

/** @} */

#endif /* __TIMER_H__ */
