/** @file rtc_def.h */
#ifndef __RTC_DEF_H__
#define __RTC_DEF_H__

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

/** @defgroup rtc_grp Real time clock */

/** @addtogroup rtc_grp
 *
 * @brief  Code and constants related to the computer real time clock.
 *
 * @{
 */

#define RTC_IRQ 8 /**< @brief RTC IRQ line */

/* RTC buffers */
/** @brief Register that selects the RTC register to work with. */
#define RTC_REGB 0x70
/** @brief Register that allows I/O operations with the RTC registers. */
#define RTC_IOB 0x71

/* RTC registers */
#define SEC_REG   0x00 /**<@brief Register that contains seconds info */
#define ASEC_REG  0x01 /**<@brief Register that contains seconds alarm info */
#define MIN_REG   0x02 /**<@brief Register that contains minutes info */
#define AMIN_REG  0x03 /**<@brief Register that contains minutes alarm info */
#define HOUR_REG  0x04 /**<@brief Register that contains hours info */
#define AHOUR_REG 0x05 /**<@brief Register that contains hours alarm info */
#define DOW_REG   0x06 /**<@brief Register that contains day of the week info */
#define DOM_REG   0x07 /**<@brief Register that contains day of the month info*/
#define MONTH_REG 0x08 /**<@brief Register that contains month info */
#define YEAR_REG  0x09 /**<@brief Register that contains year info */
#define RTC_AREG  0x0A /**<@brief RTC A register */
#define RTC_BREG  0x0B /**<@brief RTC B register */
#define RTC_CREG  0x0C /**<@brief RTC C register */
#define RTC_DREG  0x0D /**<@brief RTC D register */

/* alarm interrupts constants */
#define RTC_IDC_VAL (BIT(7) | BIT(6)) /**<@brief Value that matches any time*/

/* register A constants */
/** @brief if set, the RTC is currently updating the date/time info */
#define RTC_UIP (BIT(7))
#define RTC_RS3 (BIT(3)) /**< @brief Rate selector 3 */
#define RTC_RS2 (BIT(2)) /**< @brief Rate selector 2 */
#define RTC_RS1 (BIT(1)) /**< @brief Rate selector 1 */
#define RTC_RS0 (BIT(0)) /**< @brief Rate selector 0 */

/* register B constants */
#define RTC_SET  (BIT(7)) /**< @brief Inhibit RTC updates */
#define RTC_PIE  (BIT(6)) /**< @brief Enable RTC periodic interrupts */
#define RTC_AIE  (BIT(5)) /**< @brief Enable RTC alarm interrupts */
#define RTC_UIE  (BIT(4)) /**< @brief Enable RTC update interrupts */
#define RTC_SQWE (BIT(3)) /**< @brief Enable RTC square wave generation */

/* register C constants */
#define RTC_IRQF (BIT(7)) /**< @brief Flag signifying there was an interrupt*/
#define RTC_PF   (BIT(6)) /**< @brief Periodic interrupt */
#define RTC_AF   (BIT(5)) /**< @brief Alarm interrupt */
#define RTC_UF   (BIT(4)) /**< @brief Update interrupt */

/* register D constants */
#define RTC_VRT (BIT(7)) /**< @brief If set, there's a problem with RAM/TIME*/

/**@}*/

#endif // __RTC_DEF_H__
