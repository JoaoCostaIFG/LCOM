#include <lcom/lcf.h>

#include "i8042.h"
#include "utils.h"

/* global variables */
extern uint8_t g_out_buf;
uint8_t g_out_buf;


int
get_status(uint8_t *status)
{
	if (util_sys_inb(KBC_ST_PORT, status))
		return 1;

	return 0;
}


int
check_status_read(bool ismouse)
{
	uint8_t status;
	if (get_status(&status))
		return 1;

	/* if output buffer is empty or the is in a failed state, return error code 1 */
	if ((status & KBC_STATUS_FAIL) ||
	   !(status & KBC_STATUS_OBF))
		return 1;

	/* check whether we got mouse data and expected mouse data (or vice-versa */
	if (ismouse) {
		if (status & KBC_STATUS_MDAT)
			return 0;
	}
	else if (!(status & KBC_STATUS_MDAT))
			return 0;

	return 1;
}


int
check_status_write()
{
	uint8_t status;
	if (get_status(&status))
		return 1;

	/* if output buffer is empty or the is in a failed state, return error code 1 */
	if (!(status & KBC_STATUS_IBF))
		return 0;

	return 1;
}


int
kbc_write_cmd(uint32_t command)
{
	uint8_t tries = 50;  // 20ms at a time, 50 tries = 1sec
	bool retry = true;
	do {
		/* check if input buffer is empty */
		if (!check_status_write()) {  // can we write?
			if (!sys_outb(KBC_CMD_PORT, command))
				retry = false;
		}

		if (retry) {
			tickdelay(micros_to_ticks(DELAY_US));  // nighty night
			tries--;
		}

	} while (tries > 0 && retry);

	if (tries == 0)
		return 1;

	return 0;
}


int
kbc_get_conf(uint8_t *conf)
{
	/* disable keyboard */
	if (kbc_write_cmd(KBD_DISABLE_CMD))
		return 1;

	/* ask for the current config */
	if (kbc_write_cmd(KBC_READ_CMD))
		return 1;

	/* read keyboard current config byte */
	if (util_sys_inb(KBC_IO_BUF, conf))
		return 1;

	/* enable keyboard */
	if (kbc_write_cmd(KBD_ENABLE_CMD))
		return 1;

	return 0;
}


int
kbc_write_conf(uint8_t conf)
{
	/* command keyboard to enter configuration reading mode */
	if (kbc_write_cmd(KBC_WRITE_CMD))
		return 1;

	/* write new configuration byte to keyboard */
	if (sys_outb(KBC_IO_BUF, conf))
		return 1;

	return 0;
}


void
(kbc_ih)()
{
	/* get current byte on the output buffer */
	util_sys_inb(KBC_IO_BUF, &g_out_buf);
}
