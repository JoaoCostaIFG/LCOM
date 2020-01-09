#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 KBC.
 */

/* KBC registers/buffers */
#define KBD_IRQ		1     // keyboard IRQ line
#define KBC_ST_PORT	0x64  // KBC status port
#define KBC_IO_BUF	0x60  // KBC output/input buffer
#define KBC_CMD_PORT	0x64  // KBC command port

/* KBC commands */
#define KBC_READ_CMD	0x20  // read command byte command
#define KBC_WRITE_CMD	0x60  // write command byte command

/* KBD commands */
#define KBD_DISABLE_CMD	0xAD  // disable keyboard interface command
#define KBD_ENABLE_CMD	0xAE  // enable keyboard interface command

/* KBC command byte */
#define KBC_CONF_KBDINT	(BIT(0))	   // enable interrupt on OBF from keyboard
#define KBC_CONF_MOUINT	(BIT(1))	   // enable interrupt on OBF from mouse
#define KBC_CONF_KBDDIS	(BIT(4))	   // disable keyboard interface
#define KBC_CONF_MOUDIS	(BIT(5))	   // disable mouse
#define KBC_CONF_INTEN	(BIT(0) | BIT(1))  // enable interrupt on OBF from both keyboard and mouse
#define KBC_CONF_FULDIS	(BIT(4) | BIT(5))  // disable both keyboard and mouse interfaces

/* KBC status constants */
#define KBC_STATUS_FAIL	(BIT(7) | BIT(6))  // invalid data (parity and/or timeout error(s))
#define KBC_STATUS_MDAT	(BIT(1))	   // interrupt originated from mouse
#define KBC_STATUS_IBF	(BIT(1))	   // input buffer full
#define KBC_STATUS_OBF	(BIT(0))	   // output buffer full

/* other consts */
#define DELAY_US	20000  // delay between polls (in micro seconds)
#define SECONDBYTE	0xE0   // first byte of a 2 byte scancode
#define ESCBREAK	0x81   // breakcode of the <escape> key

/**@}*/

#endif /* _LCOM_I8042_H */
