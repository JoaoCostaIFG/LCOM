#ifndef _LCOM_KBC_UTILS_H_
#define _LCOM_KBC_UTILS_H_

int get_status(uint8_t *status);

int check_status_read(bool ismouse);

int check_status_write();

int kbc_write_cmd(uint32_t command);

int kbc_get_conf(uint8_t *conf);

int kbc_write_conf(uint8_t conf);

void (kbc_ih)();

#endif /* _LCOM_KBC_UTILS_H_ */
