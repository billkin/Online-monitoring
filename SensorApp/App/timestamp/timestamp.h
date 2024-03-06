#ifndef	_TIMESTAMP_H_
#define	_TIMESTAMP_H_
#include "main.h"

#if STM_RTC_USING
#include "rtc.h"


/*ϵͳʱ��ṹ�嶨��*/
struct bjTime
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
};


struct bjTime RTC_Get_Time(void);
unsigned long RTC_Get_Stamp(void);
void RTC_Set_Stamp(unsigned long stamp);					//��������װ���ɱ���ʱ�䲢���µ�RTC
void RTC_Set_Time(struct bjTime bj); 							//������ʱ����µ�RTC

unsigned long Time2Stamp(struct bjTime bj);    						//����ʱ��תʱ���
void Stamp2Time(struct bjTime *bj, unsigned long stamp);  //ʱ���ת��
void RTC_Set_AlarmTimeInterval(uint16_t s);				//�������ӻ���ʱ����
#endif

#endif
