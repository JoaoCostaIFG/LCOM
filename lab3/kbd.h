#ifndef _LCOM_KBD_H_
#define _LCOM_KBD_H_

void (timer_int_handler)();

int kbd_restore_conf();

int kbd_scancode(bool *make, uint8_t *size, uint8_t *bytes);

int kbd_iloop(const int *irq_sel);

int kbd_scan();

int kbd_poll();

int timed_kbd_iloop(const int *irq_sel, uint8_t delay);

int kbd_timed_scan(uint8_t *delay);

#endif /* _LCOM_KBD_H_ */
