#ifndef __BSP_RTC_H
#define __BSP_RTC_H
#include "imx6u.h"

#define SECONDS_IN_A_DAY    (86400)
#define SECONDS_IN_A_HOUR   (3600)
#define SECONDS_IN_A_MINUTE (60)
#define DAYS_IN_A_YEAR       (365)
#define YEAR_RANGE_START    (1970)
#define YEAR_RANGE_END      (2099)

/*跟时间有关的结构体*/
typedef struct _rtc_date_time
{
    unsigned int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
}rtc_date_time_t;
void rtc_init(void);
void rtc_enable(void);
void rtc_disable(void);
unsigned int rtc_coverdate_to_seconds(rtc_date_time_t *datetime);
void rtc_convertseconds_to_datetime(u64 seconds, rtc_date_time_t *datetime);
unsigned char rtc_isleapyear(unsigned short year);
void rtc_set_date_time(rtc_date_time_t *datetime);
void rtc_get_date_time(rtc_date_time_t *date_time);
unsigned int rtc_get_seconds(void);
#endif