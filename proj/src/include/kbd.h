/** @file kbd.h */
#ifndef __KBD_H__
#define __KBD_H__

#include <stdbool.h>
#include <stdint.h>

#include "include/i8042.h"

/** @defgroup kbd_grp	i8042 KBC's keyboard */

/** @addtogroup	kbc_grp
 * @{
 *	@addtogroup	kbd_grp
 *	@{
 */

/** @brief	Interrupt handler for the keyboard. */
void kbd_ih(void);

/** @brief	Get latest byte read from the keyboard output buffer.
 * @return	The latest read byte.
 */
uint8_t get_kout_buf(void);

/** @brief	Get whether the latest byte read from the keyboard output buffer
 *is valid.
 * @return	true, if the byte is valid\n
 *		false, otherwise.
 */
bool get_kbyte_valid(void);

/** @brief	Restores the KBD configuration to MINIX's default. */
int kbd_restore_conf(void);

/**
 * @brief	Assembles scancode bytes into an array.
 * @note	If the scancode is a 2 byte scancode, save first byte in array
 *and the next time this function is called, save next byte.
 *
 * @param make	Indicates if it was a make or a break file.
 * @param bytes Array containing the assembled scancode
 *
 * @return	0, on success,\n
 *          1 when a second byte is needed.
 */
int kbd_scancode(bool* make, uint8_t* bytes);

/**	@} */
/**	@} */

#endif // __KBD_H__
