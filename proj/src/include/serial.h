/** @file serial.h */
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "uart.h"

/** @addtogroup uart_grp
 * @{
 */

/** @union float2uint32_t
 *  Union to help convert between bytes and floats
 */
typedef union float2uint32_t {
  float f;
  uint32_t i;
} float2uint32;

/* MINITX DEFAULT CONFIGS */
#define DFLT_LCR 0x03 /**< MINIX's default LCR */
#define DFLT_IER 0x0F /**< MINIX's default IER */
#define DFLT_DLM 0x00 /**< MINIX's default DLM */
#define DFLT_DLL 0x01 /**< MINIX's default DLL */

#define HTCHECK  0xA0 /**< First 4 bits of the header and trailer byte */
#define SYNC_RDY 0x0F /**< Last 4 bits of the sync ready byte */
#define SYNC_OK  0x0E /**< Last 4 bits of the sync ok byte */

/* QUEUES */
/**
 * @brief Returns whether or not we can transmit information.
 *
 * @return  true, if we can transmit information,\n
 *          false, otherwise.
 */
bool serial_can_transmit(void);

/** Deletes the serial receive queue */
void serial_receive_delete(void);

/** Deletes the serial send queue */
void serial_send_delete(void);

/**
 * @brief Returns whether or not the recieve queue is empty.
 *
 * @return  true, if the receive queue is empty,\n
 *          false, otherwise.
 */
bool serial_receive_empty(void);

/** @brief Pop the receive queue. */
void serial_receive_pop(void);

/**
 * @brief Returns the front byte of the receive queue.
 * @warning User needs to check if the queue is empty first.
 *
 * @return  the byte in the front of the queue.
 */
uint8_t serial_receive_front(void);

/**
 * @brief Returns the front byte of the receive queue and pops the queue.
 * @warning User needs to check if the queue is empty first.
 *
 * @return  the byte in the front of the queue.
 */
uint8_t serial_receive_read(void);

/**
 * @brief Returns the front 4 bytes (in expected float form) of the receive
 * queue and pops them.
 * @warning User needs to check if the queue is empty first.
 *
 * @return  the float in the front of the queue.
 */
float serial_receive_read_float(void);

/**
 * @brief Returns the size (in bytes) of the receive queue.
 * @return  the size of the receive queue.
 */
size_t serial_receive_size(void);

/**
 * @brief Returns whether or not the send queue is empty.
 *
 * @return  true, if the send queue is empty,\n
 *          false, otherwise.
 */
bool serial_send_empty(void);

/** @brief Push the given byte to the send queue. */
void serial_send_push(uint8_t data);

/** @brief Push the given int to the send queue. */
void serial_send_push_int(uint32_t data);

/** @brief Push the given float to the send queue. */
void serial_send_push_float(float data);

/** @brief Transmit a byte from the send queue (if can_transmit is true). */
void serial_send(void);

/** @brief Writes directly to the uart output buffer.
 *  @param data Byte to send.
 */
void serial_send_force(uint8_t data);

/** @brief Transmits all data from send queue (or until it can't trasmit
 * anymore). */
int serial_send_all(void);

/* INTERRUPTS HANDLER */
/** @brief Serial port interrupt handler */
void serial_ih(void);

/* INTERRUPT ENABLE REGISTER */
/**
 * @brief Enable serial data receive interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_en_dataint(void);

/**
 * @brief Disable serial data receive interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_dis_dataint(void);

/**
 * @brief Enable serial transmiter holding register empty interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_en_traholdint(void);

/**
 * @brief Disables serial transmiter holding register empty interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_dis_traholdint(void);

/**
 * @brief Enable serial line status interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_en_linestint(void);

/**
 * @brief Disable serial line status interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_dis_linestint(void);

/**
 * @brief Enable serial modem status interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_en_modemint(void);

/**
 * @brief Disable serial modem status interrupts.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_dis_modemint(void);

/* LINE CONTROL REGISTER */
/**
 * @brief Sets 8 bits per char mode.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_8bpc(void);

/**
 * @brief Sets 2 stop bits mode.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_2stopbits(void);

/**
 * @brief Sets 1 stop bits mode.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_1stopbits(void);

/**
 * @brief Sets odd parity.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_oddparity(void);

/**
 * @brief Sets no parity.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_noparity(void);

/**
 * @brief Sets break control.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_BRKCTRL(void);

/**
 * @brief Unsets break control.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_noBRKCTRL(void);

/**
 * @brief Open divisor latch.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_open_divlatch(void);

/**
 * @brief Close divisor latch.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_close_divlatch(void);

/* FIFO CONTROL REGISTER */
/**
 * @brief Enable send and receive FIFO's.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_enable_fifo(void);

/**
 * @brief Disable send and receive FIFO's.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_disable_fifo(void);

/**
 * @brief Clear receive FIFO.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_clear_rcvrfifo(void);

/**
 * @brief Clear send FIFO.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_clear_xmitfifo(void);

/**
 * @brief Sets 1 byte triger level.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_set_1bytetrigger(void);

/**
 * @brief Sets 4 byte triger level.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_set_4bytetrigger(void);

/**
 * @brief Sets 8 byte triger level.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_set_8bytetrigger(void);

/**
 * @brief Sets 16 byte triger level.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_set_16bytetrigger(void);

/**
 * @brief Sets 64 byte fifo size.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_set_64byte_fifo(void);

/**
 * @brief Sets 16 byte fifo size.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_set_16byte_fifo(void);

/* DIVISOR LATCH */
/**
 * @brief Sets serial port max communication bit rate.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_set_maxrate(void);

/* RESTORE DEFAULT MINIX CONFIGS */
/**
 * @brief Restores MINIX's default serial port configurations.
 * @return  0, on success\n
 *          1, otherwise.
 */
int serial_restore_conf(void);

/* SYNC */
/**
 * @brief Handshake 2 computers (syncronizes them).
 *
 * @return  0, on successful handshake, signifying we're player 1\n
 *          1, on successful handshake, signifying we're player 2\n
 *          -1, on failed handshake.
 */
int serial_handshake(void);

/**@}*/

#endif //__SERIAL_H__
