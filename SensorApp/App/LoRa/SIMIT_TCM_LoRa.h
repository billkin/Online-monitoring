#ifndef __SIMIT_TCM_LORA_H__
#define __SIMIT_TCM_LORA_H__

#include "main.h"
#include "LoRa_uart.h"

#if SIMIT_TCM_LORA_USING

//#define LoRa_Timeout_Process()	LoRa_Rec_TimeOut_Process()


struct LORA_Typedef
{
	uint8_t IdleFlag;
	uint8_t SleepTime;
	uint16_t	RxLen;
	uint8_t RxData[LORA_BUF_LEN];
};

extern struct LORA_Typedef LoRa;

uint8_t LoRa_Init(void);				//��ʼ��
uint8_t LoRa_InterSleep(uint16_t time);	//����˯��ģʽ
uint8_t LoRa_SendData(uint8_t *pbuf,uint8_t len);		//��������

int8_t LoRa_DataPro(uint8_t *rxbuf,uint16_t *pLen);					//�Խ��յ������ݽ��д���

#endif  //#if SIMIT_TCM_LORA_USING

#endif

