/** @file kbc_utils.h */
#ifndef __KBC_UTILS_H__
#define __KBC_UTILS_H__

#include <lcom/lcf.h>

/** @addtogroup	kbc_grp
  * @{
  */


/* STATUS */
/**
 * @brief	Gets the current KBC status byte.
 *
 * @param status	Current status byte.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int get_status(uint8_t *status);

/**
 * @brief	Checks whether the KBC has data to be read in the output buffer.
 *
 * @param ismouse	Indicates if we want to read mouse data (true) or keyboard data (false)
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int check_status_read(bool ismouse);

/**
 * @brief	Checks whether the KBC is ready to accept input.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int check_status_write(void);

/* KBC COMMANDS */
/**
 * @brief	Sends command to the KBC.
 *
 * @param command	Command to send to the KBC.
 * @param isarg		Indicates whether the command we're sending is a command
 *			argument to a previous command.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int kbc_write_cmd(uint8_t command, bool isarg);

/**
 * @brief	Get current KBC configuration byte.
 *
 * @param conf	The returned current configuration byte.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int kbc_get_conf(uint8_t *conf);

/**
 * @brief	Set a new configuration on the KBC.
 *
 * @param conf	Configuration byte to set.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int kbc_write_conf(uint8_t conf);

/**
 * @brief	Restore the KBC configuration to MINIX's default.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int kbc_restore_dflt_conf(void);

/* MOUSE CONTROLLER COMMANDS */
/**
 * @brief	Check command acknowlegement from the mouse.
 * @note	Should retry sending the whole command.
 *
 * @return	0, on ACK\n
 *		1, on ERROR.
 *		-1, on NACK.
 */
int mouse_check_ack(void);

/**
 * @brief	Send a command to the mouse.
 *
 * @param command	Command to send to the mouse.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int mouse_write_cmd(uint8_t command);

/**@}*/

#endif  // __KBC_UTILS_H__
