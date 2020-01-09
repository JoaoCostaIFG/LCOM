#include <lcom/lcf.h>

#include "include/err_utils.h"
#include "include/rtc.h"
#include "include/utils.h"

#define GET_TIME_TRY_LEN 61

static uint8_t ih_creg;

void
rtc_ih(void)
{
  rtc_read_reg(RTC_CREG, &ih_creg);
}

uint8_t
rtc_get_creg(void)
{
  return ih_creg;
}

int
rtc_get_datetime(rtctime_t* ret)
{
  /* we wait 61 microsecs per try, 4 tries max
   * = 244 microsecs total
   */
  uint8_t tries = 4;
  bool update_st;
  while ((update_st = rtc_is_update_in_progress()) && tries) {
    --tries;

    if (tries)
      tickdelay(micros_to_ticks(GET_TIME_TRY_LEN)); // nighty night
  }
  if (update_st)
    return 1;

  /* disable updates while we read the current time */
  if (rtc_inhibit_update())
    return 1;

  /* get all the info if the update finished successfully */
  if (rtc_read_reg(SEC_REG, &ret->secs) || rtc_read_reg(MIN_REG, &ret->mins) ||
      rtc_read_reg(HOUR_REG, &ret->hours) || rtc_read_reg(DOW_REG, &ret->dow) ||
      rtc_read_reg(DOM_REG, &ret->dom) ||
      rtc_read_reg(MONTH_REG, &ret->month) ||
      rtc_read_reg(YEAR_REG, &ret->year)) {
    if (rtc_allow_update())
      return -1;

    return 1;
  }

  /* reenable updates */
  if (rtc_allow_update())
    return -2;

  return 0;
}

int
rtc_get_time(rtctime_t* ret)
{
  /* we wait 61 microsecs per try, 4 tries max
   * = 244 microsecs total
   */
  uint8_t tries = 4;
  bool update_st;
  while ((update_st = rtc_is_update_in_progress()) && tries) {
    --tries;
    tickdelay(micros_to_ticks(GET_TIME_TRY_LEN)); // nighty night
  }
  if (update_st)
    return 1;

  /* disable updates while we read the current time */
  if (rtc_inhibit_update())
    return 1;

  /* get all the info if the update finished successfully */
  if (rtc_read_reg(SEC_REG, &ret->secs) || rtc_read_reg(MIN_REG, &ret->mins) ||
      rtc_read_reg(HOUR_REG, &ret->hours)) {
    if (rtc_allow_update())
      return -1;

    return 1;
  }

  /* reenable updates */
  if (rtc_allow_update())
    return -2;

  warn("time: %X %X %X", ret->hours, ret->mins, ret->secs);  //TODO
  return 0;
}

int
rtc_get_date(rtctime_t* ret)
{
  /* we wait 61 microsecs per try, 4 tries max
   * = 244 microsecs total
   */
  uint8_t tries = 4;
  bool update_st;
  while ((update_st = rtc_is_update_in_progress()) && tries) {
    --tries;
    tickdelay(micros_to_ticks(GET_TIME_TRY_LEN)); // nighty night
  }
  if (update_st)
    return 1;

  /* disable updates while we read the current time */
  if (rtc_inhibit_update())
    return 1;

  /* get all the info if the update finished successfully */
  if (rtc_read_reg(DOW_REG, &ret->dow) || rtc_read_reg(DOM_REG, &ret->dom) ||
      rtc_read_reg(MONTH_REG, &ret->month) ||
      rtc_read_reg(YEAR_REG, &ret->year)) {
    if (rtc_allow_update())
      return -1;

    return 1;
  }

  /* reenable updates */
  if (rtc_allow_update())
    return -2;

  return 0;
}

void
time_ff(rtctime_t* curr, uint32_t secs)
{
  secs += BCD2DEC(curr->secs) + BCD2DEC(curr->mins) * 60 +
          BCD2DEC(curr->hours) * 3600;

  curr->secs  = DEC2BCD(secs % 60);
  curr->mins  = DEC2BCD((secs / 60) % 60);
  curr->hours = DEC2BCD(secs / 3600);
  /* warn("%X %X %X", curr->hours, curr->mins, curr->secs); */
}

int
rtc_set_alarm_ff(uint32_t secs)
{
  /* get current time */
  rtctime_t curr_time;
  if (rtc_get_time(&curr_time))
    return 1;

  /* fast forward time */
  time_ff(&curr_time, secs);

  /* set given time alarm */
  return rtc_set_alarm(curr_time.hours, curr_time.mins, curr_time.secs);
}

int
rtc_set_alarm_ff_curr(rtctime_t* curr_time, uint32_t secs)
{
  /* fast forward time */
  time_ff(curr_time, secs);

  /* set given time alarm */
  warn("set alarm: %X %X %X", curr_time->hours, curr_time->mins, curr_time->secs);  //TODO
  return rtc_set_alarm(curr_time->hours, curr_time->mins, curr_time->secs);
}

bool
rtc_is_update_in_progress(void)
{
  uint8_t curr_areg;
  if (rtc_read_reg(RTC_AREG, &curr_areg))
    return true; // assume update in case of reading failure

  return (curr_areg & RTC_UIP);
}

int
rtc_write_reg(uint8_t reg, uint8_t val)
{
  return (sys_outb(RTC_REGB, reg) || sys_outb(RTC_IOB, val));
}

int
rtc_read_reg(uint8_t reg, uint8_t* val)
{
  return (sys_outb(RTC_REGB, reg) || util_sys_inb(RTC_IOB, val));
}

int
rtc_set_alarm(uint8_t hour, uint8_t min, uint8_t sec)
{
  /* I don't care values (cron job like behaviour) */
  /* if (hour == 0xFF) */
  /* hour = RTC_IDC_VAL; */
  /* if (min == 0xFF) */
  /* min = RTC_IDC_VAL; */
  /* if (sec == 0xFF) */
  /* sec = RTC_IDC_VAL; */

  /* disable alarm interrupts (for safety) */
  rtc_disable_alrm();

  /* set hours */
  if (rtc_write_reg(AHOUR_REG, hour))
    return 1;

  /* set min */
  if (rtc_write_reg(AMIN_REG, min))
    return 1;

  /* set sec */
  if (rtc_write_reg(ASEC_REG, sec))
    return 1;

  /* reenable alarm interrupts */
  rtc_enable_alrm();

  return 0;
}

int
rtc_get_alarm(uint8_t* hour, uint8_t* min, uint8_t* sec)
{
  if (hour && rtc_read_reg(AHOUR_REG, hour))
    return 1;

  if (min && rtc_read_reg(AMIN_REG, min))
    return 1;

  if (sec && rtc_read_reg(ASEC_REG, sec))
    return 1;

  return 0;
}

int
rtc_enable_periodicint(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf |= RTC_PIE;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_disable_periodicint(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf &= ~RTC_PIE;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_enable_alrm(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf |= RTC_AIE;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_disable_alrm(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf &= ~RTC_AIE;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_enable_updateint(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf |= RTC_UIE;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_disable_updateint(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf &= ~RTC_UIE;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_inhibit_update(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf |= RTC_SET;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_allow_update(void)
{
  uint8_t curr_conf;
  if (rtc_read_reg(RTC_BREG, &curr_conf))
    return 1;

  curr_conf &= ~RTC_SET;
  return (rtc_write_reg(RTC_BREG, curr_conf));
}

int
rtc_enable_all_int(void)
{
  bool failed = false;
  if (rtc_enable_alrm())
    failed = true;

  if (rtc_enable_updateint())
    failed = true;

  if (rtc_enable_periodicint())
    failed = true;

  return failed;
}

int
rtc_disable_all_int(void)
{
  bool failed = false;
  if (rtc_disable_alrm())
    failed = true;

  if (rtc_disable_updateint())
    failed = true;

  if (rtc_disable_periodicint())
    failed = true;

  return failed;
}
