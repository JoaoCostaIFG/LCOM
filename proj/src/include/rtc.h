/** @file rtc.h */
#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

#include "rtc_def.h"

/** @addtogroup	rtc_grp
 * @{
 */

/** @struct RTCTIME_T
 *  Struct used to store a date and time point by the RTC. */
typedef struct RTCTIME_T
{
  uint8_t secs;  /**< @brief Current number of seconds [00-59]. */
  uint8_t mins;  /**< @brief Current number of minutes [00-59]. */
  uint8_t hours; /**< @brief Current number of hours [00-23]. */
  uint8_t dow;   /**< @brief Current day of the week [01-07]. */
  uint8_t dom;   /**< @brief Current date of the month [01-31]. */
  uint8_t month; /**< @brief Current month [01-12]. */
  uint8_t year;  /**< @brief Current year [00-99]. */
} rtctime_t;

/** @brief  Interrupt handler for the RTC. */
void rtc_ih(void);

/** @brief  Get the value on the RTC's C Register as of the last time the
 * interrupt handler was called. */
uint8_t rtc_get_creg(void);

/**
 * @brief Get the current RTC date and time.
 *
 * @param ret Pointer to the RTC time struct to be used as the output.
 *
 * @return  0, on success (both at reading the data and reenabling RTC
 *          interrupts).\n
 *          1, if getting the date and time info failed.\n
 *          -1, if everything failed (reading the date and time info and
 *          reenabling RTC interrupts).\n
 *          -2, if only reenabling the RTC interrupts failed (reading the date
 *          and time info was successful).
 */
int rtc_get_datetime(rtctime_t* ret);

/**
 * @brief Get the current RTC time.
 *
 * @param ret Pointer to the RTC time struct to be used as the output.
 *
 * @return  0, on success (both at reading the data and reenabling RTC
 *          interrupts).\n
 *          1, if getting the time info failed.\n
 *          -1, if everything failed (reading the time info and
 *          reenabling RTC interrupts).\n
 *          -2, if only reenabling the RTC interrupts failed (reading the
 *          time info was successful).
 */
int rtc_get_time(rtctime_t* ret);

/**
 * @brief Get the current RTC date.
 *
 * @param ret Pointer to the RTC time struct to be used as the output.
 *
 * @return  0, on success (both at reading the data and reenabling RTC
 *          interrupts).\n
 *          1, if getting the date info failed.\n
 *          -1, if everything failed (reading the date info and
 *          reenabling RTC interrupts).\n
 *          -2, if only reenabling the RTC interrupts failed (reading the date
 *          info was successful).
 */
int rtc_get_date(rtctime_t* ret);

/**
 * @brief Fast forward gien rtctime_t givens seconds to the future (can also go
 * back in time with negative numbers).
 * @note  Secs must given in decimal format.
 *
 * @param curr  Time struct to be given as input and output.
 * @param secs  Seconds to advance (or go back) in time.
 */
void time_ff(rtctime_t* curr, uint32_t secs);

/**
 * @brief Set RTC alarm for the next given secs.
 * @note  Secs must given in decimal format.
 *
 * @param secs  Seconds to advance in time.
 *
 * @return  0, on success.\n
 *          1, if getting the time info and setting the alarm failed.\n
 *          -1, if everything failed (reading the time info, reenabling RTC
 *          interrupts and setting the alarm).\n
 *          -2, if only reenabling the RTC interrupts failed (reading the time
 *          info and setting the alarm were successful).
 */
int rtc_set_alarm_ff(uint32_t secs);

/**
 * @brief Set RTC alarm for the next given secs.
 * @note  Secs must given in decimal format.
 *
 * @param curr_time Current time.
 * @param secs      Seconds to advance in time.
 *
 * @return  0, on success.\n
 *          1, if getting the time info and setting the alarm failed.\n
 *          -1, if everything failed (reading the time info, reenabling RTC
 *          interrupts and setting the alarm).\n
 *          -2, if only reenabling the RTC interrupts failed (reading the time
 *          info and setting the alarm were successful).
 */
int rtc_set_alarm_ff_curr(rtctime_t* curr_time, uint32_t secs);

/**
 * @brief Checks if the RTC is currently updating his 'current time' info.
 *
 * @return  true, if the RTC is currently updating,\n
 *          false, otherwise.
 */
bool rtc_is_update_in_progress(void);

/**
 * @brief Writes the given value to the given RTC register.
 *
 * @param reg Target register.
 * @param val Value to write.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_write_reg(uint8_t reg, uint8_t val);

/**
 * @brief Reads the current value of a given RTC register.
 *
 * @param reg Target register.
 * @param val Pointer to byte where info will be written to.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_read_reg(uint8_t reg, uint8_t* val);

/**
 * @brief Set the current RTC alarm time to the given values.
 *
 * @note  If a given value is has its 2 MSB set, it will match any possible
 *        value, i.e.: if one like this is passed as 'hour' will match the
 *        passed min and sec values for every hours.\n
 *        The defined 'RTC_IDC_VAL' macro contains value like this (0xC0).
 *
 * @param hour  Alarm hours to match.
 * @param min   Alarm minutes to match.
 * @param sec   Alarm seconds to match.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_set_alarm(uint8_t hour, uint8_t min, uint8_t sec);

/**
 * @brief Get the current RTC alarm time.
 *
 * @note  If a given value is 0xC0, it means it matches any possible value.\n
 *        The defined 'RTC_IDC_VAL' macro contains this value.
 *
 * @param hour  Output of the alarm hours.
 * @param min   Output of the alarm minutes.
 * @param sec   Output of the alarm seconds.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_get_alarm(uint8_t* hour, uint8_t* min, uint8_t* sec);

/**
 * @brief Enables the RTC's periodic interrupts.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_enable_periodicint(void);

/**
 * @brief Disables the RTC's periodic interrupts.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_disable_periodicint(void);

/**
 * @brief Enables the RTC's alarm interrupts.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_enable_alrm(void);

/**
 * @brief Disables the RTC's alarm interrupts.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_disable_alrm(void);

/**
 * @brief Enables the RTC's update interrupts.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_enable_updateint(void);

/**
 * @brief Disables the RTC's update interrupts.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_disable_updateint(void);

/**
 * @brief Inhibits the RTC's ability to update its current time.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_inhibit_update(void);

/**
 * @brief Allows the RTC to update its current time.
 *
 * @return  0, on success,\n
 *          1, otherwise.
 */
int rtc_allow_update(void);

/** @brief Enables all the RTC interrupt types */
int rtc_enable_all_int(void);

/** @brief Disables all the RTC interrupt types */
int rtc_disable_all_int(void);

/**@}*/

#endif // __RTC_H__
