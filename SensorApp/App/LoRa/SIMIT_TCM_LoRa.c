#include "SIMIT_TCM_LoRa.h"
#include "LoRa_uart.h"
#include <string.h>
#include <stdio.h>
#include "main.h"

#if SIMIT_TCM_LORA_USING

struct LORA_Typedef LoRa;

uint8_t LoRa_Init(void)
{
	LoRa.IdleFlag=0;
	LoRa.SleepTime = 4;  //4s����ʱ��
	HAL_GPIO_WritePin(RF_NRST_GPIO_Port,RF_NRST_Pin,GPIO_PIN_RESET);
	HAL_Delay(5);
	while(HAL_GPIO_ReadPin(RF_BUZY_GPIO_Port,RF_BUZY_Pin)==GPIO_PIN_SET); //�ȴ���ʼ�����
//	if(!LoRa_send_cmd("AT+GET=GID\r\n","+GET=GID",500,1))
//	{
//		return 1;
//	}
	return 0;
}	

/**
���ܣ�	LORA����˯��ģʽ
������
		time��˯��ʱ��
���أ�
		0�����óɹ�
		1������ʧ��
**/
uint8_t LoRa_InterSleep(uint16_t time)
{
	uint8_t sendbuf[128];
	sprintf((char*)sendbuf,"AT+COM=SLEEP %d\r\n",time);
	if(!LoRa_send_cmd((char*)sendbuf,"OK",5,1)) return 0;
	else	return 1;
}

/*********************************************************
��������
**********************************************************/
uint8_t LoRa_SendData(uint8_t *pbuf,uint8_t len)
{
	uint8_t sendbuf[128];
	uint8_t len_temp,i;
	sprintf((char*)sendbuf,"AT+DATA %x=",len);
	len_temp = strlen((char*)sendbuf);
	for(i=0;i<len;i++)sendbuf[len_temp+i] = pbuf[i];
	sendbuf[len_temp+i]='\r';
	sendbuf[len_temp+i+1]='\n';
	if(!LoRa_send_cmd((char*)sendbuf,"OK",200,1)) return 0;
	else	return 1;
}

int8_t LoRa_DataPro(uint8_t *rxbuf,uint16_t *pLen)
{
	uint8_t sendbuf[100];
	//uint64_t GID;
	uint16_t len=0;
	char *p;
	uint16_t Index,i;
	if(LoRa_Rec_TimeOut_Process())									// ������ڽ������
	{
		p = strstr((const char *)LoRa_buf, "AT+DATA");
		if(p != NULL)		// ����������ؼ���
		{
			Index = 7;
			while(p[Index]!='=')
			{
				len *=10;
				len += p[Index]-'0';
				Index++;
			}
			if(len<=LORA_BUF_LEN)	
			{
				Index++; //ָ������������
				for(i=0;i<len;i++)	
				{
					rxbuf[i]=p[Index];
				}
				*pLen = len;
				APP_LOG("LoRa���յ�����\r\n");
				return 1;				//���յ�����	
			}
			else 
			{
				APP_LOG("LoRa���յ����ݳ���\r\n");
				return -1;		//���ݴ���
			}
		}			
		else if(strstr((const char *)LoRa_buf, "AT+GET=GID\r\n") != NULL)		// ����������ؼ���
		{
//			GID = HAL_GetUIDw1();
//			GID <<=32;
//			GID |= HAL_GetUIDw0();
//			sprintf((char*)sendbuf,"+GET=GID %lld\r\n",GID);
			LoRa_send_data(sendbuf, strlen((char*)sendbuf));
		}
		else	if(strstr((const char *)LoRa_buf, "AT+CMD=IDLE\r\n") != NULL)		// loraģ�����
		{
			LoRa.IdleFlag=1;
		}
		
		LoRa_Buf_clear();
	}
	return 0;		//
}

#endif

