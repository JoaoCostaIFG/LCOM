#ifndef _LCOM_KBC_UTILS_H_
#define _LCOM_KBC_UTILS_H_

#include "i8042.h"
#include "utils.h"
#include <lcom/lcf.h>

/* interupt handler */
void (kbd_ih)(void);

/* status */
int get_status(uint8_t *status);

int check_status_read(bool ismouse);

int check_status_write();

/* kbc commands */
int kbc_write_cmd(uint8_t command, bool isarg);

int kbc_get_conf(uint8_t *conf);

int kbc_write_conf(uint8_t conf);

int kbc_restore_dflt_conf();

/* mouse commands */
int mouse_check_ack();

int mouse_write_cmd(uint8_t command);

int mouse_set_stream_mode();

int mouse_en_data_report();

void mouse_parse_packet(struct packet *pp);

#endif /* _LCOM_KBC_UTILS_H_ */
