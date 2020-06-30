#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

typedef struct  
{
  RTC_TimeTypeDef RTC_Time;
  RTC_DateTypeDef RTC_Date;  
}RTC_TIME;                                   

uint8_t RTC_CheckAndConfig(RTC_TIME *rtc_time,uint8_t flag);

#endif // __RTC_H__
