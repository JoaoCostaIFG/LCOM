#include <lcom/lcf.h>

#include "include/kbc_utils.h"
#include "include/i8042.h"
#include "include/utils.h"


/* STATUS */
int
get_status(uint8_t *status)
{
	if (!status)
		return 1;

	if (util_sys_inb(KBC_ST_PORT, status))
		return 1;

	return 0;
}

int
check_status_read(bool ismouse)
{
	/*
	 * Check if we can read from the output buffer
	 * 0 - can read
	 * 1 - can't read
	 */

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
check_status_write(void)
{
	uint8_t status;
	if (get_status(&status))
		return 1;

	/* if output buffer is empty or the is in a failed state, return error code 1 */
	if (!(status & KBC_STATUS_IBF))
		return 0;

	return 1;
}


/* KBC COMMANDS */
int
kbc_write_cmd(uint8_t command, bool isarg)
{
	uint8_t port = KBC_CMD_PORT;
	if (isarg)
		port = KBC_IO_BUF;

	uint8_t tries = 50;  // 20ms at a time, 50 tries = 1sec
	bool retry = true;
	do {
		/* check if input buffer is empty */
		if (!check_status_write()) {  // can we write?
			if (!sys_outb(port, command))
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
	if (!conf)
		return 1;

	/* disable keyboard */
	if (kbc_write_cmd(KBD_DISABLE_CMD, false))
		return 1;

	/* ask for the current config */
	if (kbc_write_cmd(KBC_READ_CMD, false))
		return 1;

	/* read keyboard current config byte */
	if (util_sys_inb(KBC_IO_BUF, conf))
		return 1;

	/* enable keyboard */
	if (kbc_write_cmd(KBD_ENABLE_CMD, false))
		return 1;

	return 0;
}

int
kbc_write_conf(uint8_t conf)
{
	/* command keyboard to enter configuration reading mode */
	if (kbc_write_cmd(KBC_WRITE_CMD, false))
		return 1;

	/* write new configuration byte to keyboard */
	if (kbc_write_cmd(conf, true))
		return 1;

	return 0;
}

int
kbc_restore_dflt_conf(void)
{
	/* get default kbc configuration byte */
	/*uint8_t kbc_default_conf = minix_get_dflt_kbc_cmd_byte();*/
	uint8_t kbc_default_conf;
	if (kbc_get_conf(&kbc_default_conf))
		return 1;

	/* editing curent conf */
	kbc_default_conf |= KBC_CONF_INTEN;  // enable interrupts
	kbc_default_conf &= ~KBC_CONF_FULDIS;  // enable interfaces

	/* write new keyboard config byte */
	if (kbc_write_conf(kbc_default_conf))
		return 1;

	/* write kbc configuration byte */
	if (kbc_write_conf(kbc_default_conf))
		return 1;

	return 0;
}


/* MOUSE CONTROLLER COMMANDS */
int
mouse_check_ack(void)
{
	/*
	 * Check if the command given to the mouse controller was accepted
	 * Returns:
	 *  0 - ACK
	 *  1 - ERROR
	 * -1 - NACK
	 */
	uint8_t tries = 50;  // 20ms at a time, 50 tries = 1sec
	bool retry = true;
	uint8_t answer = 0;

	do {
		if (!check_status_read(true))
			retry = false;

		if (retry) {
			tickdelay(micros_to_ticks(DELAY_US));  // nighty night
			tries--;
		}

	} while (tries > 0 && retry);

	if (retry || util_sys_inb(KBC_IO_BUF, &answer))
		return 1;

	if (answer == MOU_CMD_ACK)
		return 0;

	if (answer == MOU_CMD_NACK)
		return -1;

	return 1;
}

int
mouse_write_cmd(uint8_t command)
{
	/*
	 * keep trying to send command to mouse controller until it is acknowleged
	 * (gives up trying after 50 failed attempts)
	 */

	uint8_t tries = 50;  // 20ms at a time, 50 tries = 1sec
	bool retry = true;
	int answer = 0;

	do {
		if (kbc_write_cmd(KBC_MOUSE_CMD, false))
			return 1;

		if (kbc_write_cmd(command, true))
			return 1;

		if ((answer = mouse_check_ack()) != -1)
			retry = false;

		if (retry) {
			tickdelay(micros_to_ticks(DELAY_US));  // nighty night
			tries--;
		}

	} while (tries > 0 && retry);

	if (answer == 1)  // check if it was an ERROR
		return 1;

	return 0;
}
