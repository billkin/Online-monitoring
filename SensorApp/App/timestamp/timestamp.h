#ifndef	_TIMESTAMP_H_
#define	_TIMESTAMP_H_
#include "main.h"

#if STM_RTC_USING
#include "rtc.h"


/*系统时间结构体定义*/
struct bjTime
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
};


struct bjTime RTC_Get_Time(void);
unsigned long RTC_Get_Stamp(void);
void RTC_Set_Stamp(unsigned long stamp);					//将世纪秒装换成本地时间并更新到RTC
void RTC_Set_Time(struct bjTime bj); 							//将本地时间更新到RTC

unsigned long Time2Stamp(struct bjTime bj);    						//北京时间转时间戳
void Stamp2Time(struct bjTime *bj, unsigned long stamp);  //时间戳转北
void RTC_Set_AlarmTimeInterval(uint16_t s);				//设置闹钟唤醒时间间隔
#endif

#endif
