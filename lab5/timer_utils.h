#ifndef _LCOM_TIMER_UTILS_H_
#define _LCOM_TIMER_UTILS_H_

#include <lcom/lcf.h>

/* INTERRUPT HANDLER */
void (timer_int_handler)(void);

/* TIMER CONFIG FUNCTIONS */
int timer_set_freq(uint8_t timer, uint32_t freq);

int timer_get_config(uint8_t timer, uint8_t *st);

int timer_display_config(uint8_t timer, uint8_t st, enum timer_status_field field);

#endif /* _LCOM_TIMER_UTILS_H_ */
