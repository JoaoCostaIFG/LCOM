/** @file i8042.h */
#ifndef __I8042_H__
#define __I8042_H__

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

/** @defgroup kbc_grp i8042 KBC */

/** @addtogroup kbc_grp
 *
 * @brief  Code and constants related to the keyboard and mouse of the i8042
 * KBC.
 *
 * @{
 */

/* KBC registers/buffers */
#define KBD_IRQ      1    /**< @brief keyboard IRQ line. */
#define MOU_IRQ      12   /**< @brief mouse IRQ line. */
#define KBC_ST_PORT  0x64 /**< @brief KBC status port. */
#define KBC_IO_BUF   0x60 /**< @brief KBC output/input buffer. */
#define KBC_CMD_PORT 0x64 /**< @brief KBC command port. */

/* KBC commands */
#define KBC_READ_CMD  0x20 /**< @brief read command byte command. */
#define KBC_WRITE_CMD 0x60 /**< @brief write command byte command. */
/** @brief send commands straight to the mouse controller. */
#define KBC_MOUSE_CMD 0xD4

/* KBD commands */
#define KBD_DISABLE_CMD 0xAD /**< @brief disable keyboard interface command.*/
#define KBD_ENABLE_CMD  0xAE /**< @brief enable keyboard interface command. */

/* KBC command byte */
/** @brief enable interrupt on OBF from keyboard. */
#define KBC_CONF_KBDINT (BIT(0))
/** @brief enable interrupt on OBF from mouse. */
#define KBC_CONF_MOUINT (BIT(1))
#define KBC_CONF_KBDDIS (BIT(4)) /**< @brief disable keyboard interface. */
#define KBC_CONF_MOUDIS (BIT(5)) /**< @brief disable mouse. */
/** @brief enable interrupt on OBF from both keyboard and mouse. */
#define KBC_CONF_INTEN (BIT(0) | BIT(1))
/** @brief disable both keyboard and mouse interfaces. */
#define KBC_CONF_FULDIS (BIT(4) | BIT(5))

/* KBC status constants */
/** @brief invalid data (parity and/or timeout error(s)). */
#define KBC_STATUS_FAIL (BIT(7) | BIT(6))
#define KBC_STATUS_MDAT (BIT(5)) /**<@brief interrupt originated from mouse.*/
#define KBC_STATUS_IBF  (BIT(1)) /**< @brief input buffer full. */
#define KBC_STATUS_OBF  (BIT(0)) /**< @brief output buffer full. */

/* Mouse commands */
/** @brief disable mouse data reporting (in stream mode should be sent before
 * any other command). */
#define MOU_DISABLE_DATA_REPORT_CMD 0xF5
/** @brief enable mouse data reporting (stream mode only). */
#define MOU_ENABLE_DATA_REPORT_CMD 0xF4
#define MOU_SET_STREAM_MODE_CMD    0xEA /**< @brief set mouse streaming mode. */
#define MOU_SET_REMOTE_MODE_CMD    0xF0 /**< @brief set mouse remote mode. */
#define MOU_READ_DATA_CMD          0xEB /**< @brief set mouse remote mode. */

/* Mouse acknowlegements */
#define MOU_CMD_ACK   0xFA /**< @brief mouse command was successful. */
#define MOU_CMD_NACK  0xFE /**< @brief mouse command should be resent. */
#define MOU_CMD_ERROR 0xFC /**< @brief mouse command reported an error. */

/* Mouse packet constants */
#define MOU_LB (BIT(0)) /**< @brief signals left button down. */
#define MOU_RB (BIT(1)) /**< @brief signals right button down. */
#define MOU_MB (BIT(2)) /**< @brief signals middle button down. */
/** @brief bit that must be set on the first byte of a packet. */
#define MOU_1ST_PACKET_BIT (BIT(3))
/** @brief the MSB of the movement in x (9th bit). */
#define MOU_MSB_X_DELTA (BIT(4))
/** @brief the MSB of the movement in y (9th bit). */
#define MOU_MSB_Y_DELTA (BIT(5))
/** @brief signals an overflow of the movement in x. */
#define MOU_X_OV (BIT(6))
/** @brief signals an overflow of the movement in y. */
#define MOU_Y_OV (BIT(7))

/* other consts */
#define DELAY_US   20000 /**< @brief delay between polls (in micro seconds). */
#define SECONDBYTE 0xE0  /**< @brief first byte of a 2 byte scancode. */
#define BREAKCODE  (BIT(7))  /**< @brief break codes have this bit set */
#define MAKECODE   (~BIT(7)) /**< @brief break codes have this bit set */
#define ESCBREAK   0x81      /**< @brief breakcode of the \<escape\> key. */

/**@}*/

#endif // __I8042_H__
