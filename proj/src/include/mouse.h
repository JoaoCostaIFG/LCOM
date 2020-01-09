/** @file mouse.h */
#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include "include/i8042.h"

/** @defgroup mouse_grp	i8042 KBC's mouse */

/** @addtogroup	kbc_grp
 * @{
 *	@addtogroup	mouse_grp
 *	@{
 */

/** @brief	Interrupt handler for the mouse. */
void mouse_ih(void);

/** @brief	Get latest byte read from the mouse output buffer.
 * @return	The latest read byte.
 */
uint8_t get_mout_buf(void);

/** @brief	Get whether the latest byte read from the mouse output buffer is
 *valid.
 * @return	true, if the byte is valid\n
 *		false, otherwise.
 */
bool get_mbyte_valid(void);

/**
 * @brief	Reads and parses the information of a whole mouse key
 * press/release.
 *
 * @param pp		Struct where the info will be saved.
 *
 * @return  0, on success,\n
 *          1, on failure,\n
 *          -1, when expecting more bytes for the current packet.
 */
int mouse_sync_packet(struct packet* pp);

/**
 * @brief	Parse mouse data packet information (stored in bytes field).
 *
 * @param pp	Struct to save the parsed information to.
 */
void mouse_parse_packet(struct packet* pp);

/**
 * @brief	Set mouse to stream mode.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int mouse_set_stream_mode(void);

/**
 * @brief	Enable data reporting on the mouse.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int mouse_en_data_report(void);

/**	@}*/
/**@}*/

#endif // __MOUSE_H__
