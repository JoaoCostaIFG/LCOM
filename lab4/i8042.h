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
#define MOU_IRQ		12    // mouse IRQ line
#define KBC_ST_PORT	0x64  // KBC status port
#define KBC_IO_BUF	0x60  // KBC output/input buffer
#define KBC_CMD_PORT	0x64  // KBC command port

/* KBC commands */
#define KBC_READ_CMD	0x20  // read command byte command
#define KBC_WRITE_CMD	0x60  // write command byte command
#define KBC_MOUSE_CMD	0xD4  // send commands straight to the mouse controller

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
#define KBC_STATUS_MDAT	(BIT(5))	   // interrupt originated from mouse
#define KBC_STATUS_IBF	(BIT(1))	   // input buffer full
#define KBC_STATUS_OBF	(BIT(0))	   // output buffer full

/* Mouse commands */
#define MOU_DISABLE_DATA_REPORT_CMD	0xF5  // disable mouse data reporting (in stream mode should be sent before any other command)
#define MOU_ENABLE_DATA_REPORT_CMD	0xF4  // enable mouse data reporting (stream mode only)
#define MOU_SET_STREAM_MODE_CMD		0xEA  // set mouse streaming mode
#define MOU_SET_REMOTE_MODE_CMD		0xF0  // set mouse remote mode
#define MOU_READ_DATA_CMD		0xEB  // set mouse remote mode

/* Mouse acknowlegements */
#define MOU_CMD_ACK	0xFA  // mouse command was successful
#define MOU_CMD_NACK	0xFE  // mouse command should be resent
#define MOU_CMD_ERROR	0xFC  // mouse command reported an error

/* Mouse packet constants */
#define MOU_LB			(BIT(0))
#define MOU_RB			(BIT(1))
#define MOU_MB			(BIT(2))
#define MOU_1ST_PACKET_BIT	(BIT(3))  // bit that must be set on the first byte of a packet
#define MOU_MSB_X_DELTA		(BIT(4))
#define MOU_MSB_Y_DELTA		(BIT(5))
#define MOU_X_OV		(BIT(6))
#define MOU_Y_OV		(BIT(7))

/* other consts */
#define DELAY_US	20000  // delay between polls (in micro seconds)
#define SECONDBYTE	0xE0   // first byte of a 2 byte scancode
#define ESCBREAK	0x81   // breakcode of the <escape> key

/**@}*/

#endif /* _LCOM_I8042_H */
