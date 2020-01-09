/** @file i8254.h */
#ifndef __I8254_H__
#define __I8254_H__

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

/** @defgroup timer_grp	i8254 timers */

/** @addtogroup	timer_grp
  *
  * @brief	Code and constants related to the i8254 timers.
  *
  * @{
  */

#define TIMER_FREQ	1193182		/**< @brief clock frequency for timer in PC and AT */
#define TIMER0_IRQ	0		/**< @brief Timer 0 IRQ line */
#define TIMER0_FREQ	60		/**< @brief Timer 0 Frequency */

/* I/O port addresses */

#define TIMER_0    0x40			/**< @brief Timer 0 count register */
#define TIMER_1    0x41			/**< @brief Timer 1 count register */
#define TIMER_2    0x42			/**< @brief Timer 2 count register */
#define TIMER_CTRL 0x43			/**< @brief Control register */

#define TIMER2PORT(timer) TIMER_##timer /** @brief Converts the number that identifies a register to a port */

#define SPEAKER_CTRL 0x61 /**< @brief Register for speaker control  */

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0   0x00			/**< @brief Control Word for Timer 0 */
#define TIMER_SEL1   BIT(6)			/**< @brief Control Word for Timer 1 */
#define TIMER_SEL2   BIT(7)			/**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD (BIT(7) | BIT(6))		/**< @brief Read Back Command */

#define SEL_TIMER(timer) TIMER_SEL##timer	/**< @brief Converts the number that identifies a register to a port */

/* Register selection: bits 5 and 4 */

#define TIMER_LSB     BIT(4)		       /**< @brief Initialize Counter LSB only */
#define TIMER_MSB     BIT(5)		       /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB (TIMER_LSB | TIMER_MSB)  /**< @brief Initialize LSB first and MSB afterwards */
#define TIMER_INIT_MASK (BIT(4) | BIT(5))      /**< @brief Mask used to parse a timer's init mode */

/* Operating mode: bits 3, 2 and 1 */

#define TIMER_SQR_WAVE (BIT(2) | BIT(1))  /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN BIT(2)		  /**< @brief Mode 2: rate generator */

/* Counting mode: bit 0 */

#define TIMER_BCD 0x01					/**< @brief Count in BCD */
#define TIMER_BIN 0x00					/**< @brief Count in binary */
#define TIMER_OP_MASK (BIT(3) | BIT(2) | BIT(1))	/**< @brief Mask used to parse a timer's operating mode */

/* READ-BACK COMMAND FORMAT */

#define TIMER_RB_COUNT  BIT(5)		/**< @brief Count bit for the read-back command */
#define TIMER_RB_STATUS BIT(4)		/**< @brief Status bit for the read-back command */
#define TIMER_RB_SEL(n)  BIT((n) + 1)	/**< @brief Gives the bit that should be selected for a read-back cmd for a given timer */

/**@}*/

#endif	// __I8254_H__
