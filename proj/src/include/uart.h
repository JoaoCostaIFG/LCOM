/** @file uart.h */
#ifndef __UART_H__
#define __UART_H__

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

/** @defgroup uart_grp UART and Serial Communication */

/** @addtogroup uart_grp
 *
 * @brief  Code and constants related to the computer serial ports.
 *
 * @{
 */

#define COM1_IRQ      4     /**< @brief  COM line 1 IRQ */
#define COM1_VEC      0x0C  /**< @brief COM line 1 vector */
#define COM1_BASEADDR 0x3F8 /**< @brief  COM line 1 base address */
#define COM2_IRQ      3     /**< @brief  COM line 2 IRQ */
#define COM2_VEC      0x0B  /**< @brief COM line 2 vector */
#define COM2_BASEADDR 0x2F8 /**< @brief  COM line 2 base address */

/* READ-ONLY REGISTERS */
#define UART_RBR 0 /**< @brief Receiver buffer reg. */
#define UART_IIR 2 /**< @brief Interrupt identification reg. */
#define UART_LSR 5 /**< @brief Line status reg. */
#define UART_MSR 6 /**< @brief Modem status reg. */

/* WRITE-ONLY REGISTERS */
#define UART_THR 0 /**< @brief Transmiter holding reg. */
#define UART_FCR 2 /**< @brief FIFO control reg. */

/* READ/WRITE REGISTERS */
#define UART_IER 1 /**< @brief Interrupt enable reg. */
#define UART_LCR 3 /**< @brief Line control reg. */
#define UART_MCR 4 /**< @brief Modem control reg. */
#define UART_SR  7 /**< @brief Scratchpad reg. */

/* READ/WRITE REGS, OVERLOADED IF DLAB OF LCR IS SET */
#define UART_DLL 0 /**< @brief Divisor latch LSB. */
#define UART_DLM 1 /**< @brief Divisor latch MSB . */

/* LINE CONTROL REGISTER */
/* BIT 1, 0 */
#define LCR_5BPC 0                 /**< @brief 5 bits per char */
#define LCR_6BPC (BIT(0))          /**< @brief 6 bits per char */
#define LCR_7BPC (BIT(1))          /**< @brief 7 bits per char */
#define LCR_8BPC (BIT(1) | BIT(0)) /**< @brief 8 bits per char */
/* BIT 2 */
#define LCR_2STOPB BIT(2) /**< @brief 2 stop bits */
/* BIT 5, 4, 3 */
#define LCR_NOPAR   (~BIT(3))         /**< @brief No parity (use and) */
#define LCR_ODDPAR  (BIT(3))          /**< @brief Odd parity */
#define LCR_EVENPAR (BIT(4) | BIT(3)) /**< @brief Even parity */
#define LCR_1PAR    (BIT(5) | BIT(3)) /**< @brief Parity is 1 */
#define LCR_0PAR    (BIT(5) | BIT(4) | BIT(3)) /**< @brief Parity is 0 */
/* BIT 6 */
#define LCR_BREAKCTRL (BIT(6)) /**< @brief Break control */
/* BIT 7 */
#define LCR_DIVLATCHACCESS (BIT(7)) /**< @brief Divisor latch access */

/* LINE STATUS REGISTER (READ ONLY) */
#define LSR_DATA   (BIT(0)) /**< @brief There is data for receiving */
#define LSR_OVRERR (BIT(1)) /**< @brief Overrun error (char overwritten) */
#define LSR_PARERR (BIT(2)) /**< @brief Parity error */
#define LSR_FRERR  (BIT(3)) /**< @brief Framing error (invalid stop bit) */
#define LSR_BRKINT (BIT(4)) /**< @brief Break interrupt (input line) */
/** @brief Transmiter holding reg empty (ready to transmit new char) */
#define LSR_TRAHOLD (BIT(5))
/** @brief THR and Tansmiter Shift reg are both empty */
#define LSR_EMPTYREG (BIT(6))
/** @brief Error in the FIFO (reset when LSR is read) */
#define LSR_FIFOERR (BIT(7))

/* INTERRUPT ENABLE REGISTER */
#define IER_DATAINT    (BIT(0)) /**< @brief Received data available int */
#define IER_TRAHOLDINT (BIT(1)) /**< @brief Tansmiter holding reg empty int */
#define IER_LINESTINT  (BIT(2)) /**< @brief Change in error bits (LSR) int */
#define IER_MODEMINT   (BIT(3)) /**< @brief Modem status int */

/* INTERUPT IDENTIFICATION REGISTER */
/* priority:
   1 - Receiver line status.
   2 - Received data ready.
   3 - Transmiter holding register empty.
   4 - Modem status.
*/
#define IIR_NOINT (BIT(0)) /**< @brief No interrupt is pending */
/** @brief Receiver line status pending */
#define IIR_LINEST (BIT(2) | BIT(1))
/** @brief Receiver line status pending (first bit) */
#define IIR_LINEST_1 (BIT(1))
/** @brief Receiver line status pending (second bit) */
#define IIR_LINEST_2  (BIT(2))
#define IIR_RCVRDATA  (BIT(2)) /**< @brief Receive data available */
#define IIR_FIFO      (BIT(3)) /**< @brief Char timeout (FIFO) */
#define IIR_TRANSHOLD (BIT(1)) /**< @brief Transmiter holding reg empty */
/** @brief Modem status interrupt pending (should negate this macro) */
#define IIR_MODEMST    (BIT(3) | BIT(2) | BIT(1))
#define IIR_IS64       (BIT(5)) /**< @brief Set if 64-byte FIFO enabled */
#define IIR_ISBOTHFIFO (BIT(7) | BIT(6)) /**< @brief Both FIFOs enabled */

/* FIFO CONTROL REGISTER */
#define FCR_BOTHFIFO  (BIT(0)) /**< @brief Enable both FIFOs */
#define FCR_CLRRCVR   (BIT(1)) /**< @brief Clear the RCVR FIFO */
#define FCR_CLRXMIT   (BIT(2)) /**< @brief Clear the XMIT FIFO */
#define FCR_ENBL64    (BIT(5)) /**< @brief Enable 64 byte FIFO (16750 only) */
#define FCR_RCVRTRIG1 0        /**< @brief RCVR FIFO trigger level 1 byte */
#define FCR_RCVRTRIG4 (BIT(6)) /**< @brief RCVR FIFO trigger level 4 bytes */
#define FCR_RCVRTRIG8 (BIT(7)) /**< @brief RCVR FIFO trigger level 8 bytes */
/** @brief RCVR FIFO trigger level 16 bytes */
#define FCR_RCVRTRIG16 (BIT(7) | BIT(6))

/**@}*/

#endif // __UART_H__
