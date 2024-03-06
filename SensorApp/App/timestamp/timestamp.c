#include "timestamp.h"
#include "main.h"

#if STM_RTC_USING
const unsigned char TAB_LEAP_MONTH[]={31,29,31,30,31,30,31,31,30,31,30,31};
const unsigned char TAB_NORL_MONTH[]={31,28,31,30,31,30,31,31,30,31,30,31};

//-----------------------------------------------------------------------------
//输出:该年份是不是闰年.1,是.0,不是
//-----------------------------------------------------------------------------

unsigned char Is_Leap_Year(unsigned short year)
{			  
	if(year%100==0)
	{
		if(year%400==0)
			return 1;
	}
	else
	{
		if(year%4==0)
			return 1;
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned long Time2Stamp(struct bjTime bj)    //北京时间转时间戳
{
	unsigned long stamp=0;
	unsigned short t,year;
	const unsigned char *month;
	year = bj.sDate.Year+2000;
	if(year<1970||year>2099)
		return 0;

	for(t=1970;t<year;t++)									//把所有年份的秒钟相加
	{
		if(Is_Leap_Year(t))
			stamp+=31622400;																//闰年的秒钟数
		else 
			stamp += 31536000;			  											//平年的秒钟数
	}
	
	if(Is_Leap_Year(t))
		month = TAB_LEAP_MONTH;
	else
		month = TAB_NORL_MONTH;
	
	bj.sDate.Month-=1;
	for(t=0;t<bj.sDate.Month;t++)	   													//把前面月份的秒钟数相加
		stamp += (unsigned long)month[t]*86400;							//月份秒钟数相加
	
	stamp += (unsigned long)(bj.sDate.Date-1)*86400;					//把前面日期的秒钟数相加 
	
	stamp += bj.sTime.Hours*3600;															//小时秒钟数
  stamp += bj.sTime.Minutes*60;	 														//分钟秒钟数
	stamp += bj.sTime.Seconds;																//最后的秒钟加上去    
	
	stamp -= 28800;																						//减去8小时时区差
	
	return stamp;
}
	
//-----------------------------------------------------------------------------
void Stamp2Time(struct bjTime *bj, unsigned long stamp)        //时间戳转北京时间
{
	unsigned short t;
	const unsigned char *month;
	
	stamp += 28800;							      //加上8小时时区差
	
	unsigned long temp1 = stamp/86400;
	
//	//计算星期
	bj->sDate.WeekDay = (temp1%7)>3 ?  (temp1%7)-3 : 4+(temp1%7);
	
	//计算年
	t=1970;
	while(1)
	{
		if(Is_Leap_Year(t))
		{
			if(temp1>=366)	{t++;	temp1-=366;}
			else						{month = TAB_LEAP_MONTH;	break;}
		}
		else
		{
			if(temp1>=365)	{t++;	temp1-=365;}
			else						{month = TAB_NORL_MONTH;	break;}
		}
	}
	//bj->sDate.Year = t-1970;
	bj->sDate.Year = t-2000;
	//计算月
	t = 0;
	while(1)
	{
		if(temp1>=month[t])				{temp1-=month[t++];}
		else											break;
	}
	bj->sDate.Month = t+1;
	
	//计算日
	bj->sDate.Date = temp1+1;
	
	temp1=stamp%86400;	

	bj->sTime.Hours  = temp1/3600;						//小时
	bj->sTime.Minutes   = (temp1%3600)/60; 	//分钟	
	bj->sTime.Seconds   = (temp1%3600)%60; 	//秒钟
}

/*
功能：从RTC中获取时间
*/
struct bjTime RTC_Get_Time(void)
{
	struct bjTime bj;
	HAL_RTC_GetTime(&hrtc, &bj.sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &bj.sDate, RTC_FORMAT_BIN);
	//bj.sDate.Year -=1 ;
	return bj;
}

/*
功能：从RTC中获取世纪秒
*/
unsigned long RTC_Get_Stamp(void)
{
	unsigned long  timeStamp;
	struct bjTime bj;
	bj = RTC_Get_Time();
	APP_LOG("本地时间：20%d年-%02d月-%02d日 星期%d  ",bj.sDate.Year,bj.sDate.Month,bj.sDate.Date,bj.sDate.WeekDay);
	APP_PRINTF("%02d:%02d:%02d\r\n\r\n",bj.sTime.Hours,bj.sTime.Minutes,bj.sTime.Seconds);
	timeStamp = Time2Stamp(bj);
	return timeStamp;
}

/*
功能：将本地时间更新到RTC
参数：bj 本地时间
*/
void RTC_Set_Time(struct bjTime bj)
{
	HAL_RTC_SetDate(&hrtc, &bj.sDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &bj.sTime, RTC_FORMAT_BIN);
	APP_LOG("设置时间：20%d年-%02d月-%02d日 星期%d  ",bj.sDate.Year,bj.sDate.Month,bj.sDate.Date,bj.sDate.WeekDay);
	APP_PRINTF("%02d:%02d:%02d\r\n\r\n",bj.sTime.Hours,bj.sTime.Minutes,bj.sTime.Seconds);
}

/*
功能：将世纪秒装换成本地时间并更新到RTC
参数：stamp 世纪秒
*/
void RTC_Set_Stamp(unsigned long stamp)
{
	struct bjTime bj;
	Stamp2Time(&bj, stamp);
	RTC_Set_Time(bj);
}

/*
功能：设置闹钟唤醒时间
参数：stamp 时间戳
*/
void RTC_Set_AlarmTime(unsigned long stamp)
{
	struct bjTime bj;
	RTC_AlarmTypeDef sAlarm = {0};
	Stamp2Time(&bj, stamp);
	sAlarm.AlarmTime.Hours = bj.sTime.Hours;
	sAlarm.AlarmTime.Minutes = bj.sTime.Minutes;
	sAlarm.AlarmTime.Seconds = bj.sTime.Seconds;
	HAL_RTC_SetAlarm(&hrtc,&sAlarm,RTC_FORMAT_BIN);
	APP_LOG("闹钟时间：%d:%d:%d\r\n\r\n",bj.sTime.Hours,bj.sTime.Minutes,bj.sTime.Seconds);
}

/*
功能：设置闹钟唤醒时间间隔
参数：s 时间间隔，单位秒
*/
void RTC_Set_AlarmTimeInterval(uint16_t s)
{
	unsigned long Stamp;
	Stamp =	RTC_Get_Stamp();
	RTC_Set_AlarmTime(Stamp+s);
}


#endif

