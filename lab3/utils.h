#ifndef _LCOM_UTILS_H_
#define _LCOM_UTILS_H_

#include <lcom/lcf.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb);

int(util_get_MSB)(uint16_t val, uint8_t *msb);

int (util_sys_inb)(int port, uint8_t *value);

int subscribe_int(int *bit_no, int irq_line, bool is_exclusive);

int unsubscribe_int(int *hook_id);

#endif /* _LCOM_UTILS_H_ */
