#include "timestamp.h"
#include "main.h"

#if STM_RTC_USING
const unsigned char TAB_LEAP_MONTH[]={31,29,31,30,31,30,31,31,30,31,30,31};
const unsigned char TAB_NORL_MONTH[]={31,28,31,30,31,30,31,31,30,31,30,31};

//-----------------------------------------------------------------------------
//���:������ǲ�������.1,��.0,����
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
unsigned long Time2Stamp(struct bjTime bj)    //����ʱ��תʱ���
{
	unsigned long stamp=0;
	unsigned short t,year;
	const unsigned char *month;
	year = bj.sDate.Year+2000;
	if(year<1970||year>2099)
		return 0;

	for(t=1970;t<year;t++)									//��������ݵ��������
	{
		if(Is_Leap_Year(t))
			stamp+=31622400;																//�����������
		else 
			stamp += 31536000;			  											//ƽ���������
	}
	
	if(Is_Leap_Year(t))
		month = TAB_LEAP_MONTH;
	else
		month = TAB_NORL_MONTH;
	
	bj.sDate.Month-=1;
	for(t=0;t<bj.sDate.Month;t++)	   													//��ǰ���·ݵ����������
		stamp += (unsigned long)month[t]*86400;							//�·����������
	
	stamp += (unsigned long)(bj.sDate.Date-1)*86400;					//��ǰ�����ڵ���������� 
	
	stamp += bj.sTime.Hours*3600;															//Сʱ������
  stamp += bj.sTime.Minutes*60;	 														//����������
	stamp += bj.sTime.Seconds;																//�������Ӽ���ȥ    
	
	stamp -= 28800;																						//��ȥ8Сʱʱ����
	
	return stamp;
}
	
//-----------------------------------------------------------------------------
void Stamp2Time(struct bjTime *bj, unsigned long stamp)        //ʱ���ת����ʱ��
{
	unsigned short t;
	const unsigned char *month;
	
	stamp += 28800;							      //����8Сʱʱ����
	
	unsigned long temp1 = stamp/86400;
	
//	//��������
	bj->sDate.WeekDay = (temp1%7)>3 ?  (temp1%7)-3 : 4+(temp1%7);
	
	//������
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
	//������
	t = 0;
	while(1)
	{
		if(temp1>=month[t])				{temp1-=month[t++];}
		else											break;
	}
	bj->sDate.Month = t+1;
	
	//������
	bj->sDate.Date = temp1+1;
	
	temp1=stamp%86400;	

	bj->sTime.Hours  = temp1/3600;						//Сʱ
	bj->sTime.Minutes   = (temp1%3600)/60; 	//����	
	bj->sTime.Seconds   = (temp1%3600)%60; 	//����
}

/*
���ܣ���RTC�л�ȡʱ��
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
���ܣ���RTC�л�ȡ������
*/
unsigned long RTC_Get_Stamp(void)
{
	unsigned long  timeStamp;
	struct bjTime bj;
	bj = RTC_Get_Time();
	APP_LOG("����ʱ�䣺20%d��-%02d��-%02d�� ����%d  ",bj.sDate.Year,bj.sDate.Month,bj.sDate.Date,bj.sDate.WeekDay);
	APP_PRINTF("%02d:%02d:%02d\r\n\r\n",bj.sTime.Hours,bj.sTime.Minutes,bj.sTime.Seconds);
	timeStamp = Time2Stamp(bj);
	return timeStamp;
}

/*
���ܣ�������ʱ����µ�RTC
������bj ����ʱ��
*/
void RTC_Set_Time(struct bjTime bj)
{
	HAL_RTC_SetDate(&hrtc, &bj.sDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &bj.sTime, RTC_FORMAT_BIN);
	APP_LOG("����ʱ�䣺20%d��-%02d��-%02d�� ����%d  ",bj.sDate.Year,bj.sDate.Month,bj.sDate.Date,bj.sDate.WeekDay);
	APP_PRINTF("%02d:%02d:%02d\r\n\r\n",bj.sTime.Hours,bj.sTime.Minutes,bj.sTime.Seconds);
}

/*
���ܣ���������װ���ɱ���ʱ�䲢���µ�RTC
������stamp ������
*/
void RTC_Set_Stamp(unsigned long stamp)
{
	struct bjTime bj;
	Stamp2Time(&bj, stamp);
	RTC_Set_Time(bj);
}

/*
���ܣ��������ӻ���ʱ��
������stamp ʱ���
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
	APP_LOG("����ʱ�䣺%d:%d:%d\r\n\r\n",bj.sTime.Hours,bj.sTime.Minutes,bj.sTime.Seconds);
}

/*
���ܣ��������ӻ���ʱ����
������s ʱ��������λ��
*/
void RTC_Set_AlarmTimeInterval(uint16_t s)
{
	unsigned long Stamp;
	Stamp =	RTC_Get_Stamp();
	RTC_Set_AlarmTime(Stamp+s);
}


#endif

