#ifndef _LCOM_UTILS_H_
#define _LCOM_UTILS_H_

#include <lcom/lcf.h>

#define SEC2MICRO(x) (x * 1000000)  // converts seconds to microseconds


/* INTERRUPT (UN)SUBSCRIBE */
int subscribe_int(int *bit_no, int irq_line, bool is_exclusive);

int unsubscribe_int(int *hook_id);

/* UTILITIES */
int(util_get_LSB)(uint16_t val, uint8_t *lsb);

int(util_get_MSB)(uint16_t val, uint8_t *msb);

int (util_sys_inb)(int port, uint8_t *value);

int16_t ninebit2sixteen(bool msb_bit, uint8_t lsb_bit);

float slope_calc(int x, int y);

float flabs(float x);

int wait_for_keypress(uint8_t keytowait);

#endif /* _LCOM_UTILS_H_ */
