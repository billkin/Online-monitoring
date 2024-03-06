#ifndef __E28_2G4T12S_LORA_H__
#define __E28_2G4T12S_LORA_H__

#include "main.h"
#include "LoRa_uart.h"

#if E28_2G4T12S_LORA_USING

#define LORA_M2 		RF_M2_ACTIN_Pin
#define LORA_M1 		RF_M1_Pin
#define LORA_M0 		RF_M0_Pin
#define LORA_M_PORT RF_M0_GPIO_Port

#define LORA_AUX 		LORA_AUX_Pin
#define LORA_AUX_PORT LORA_AUX_GPIO_Port

typedef enum
{
	Trans_Mode 		= 0,		//����ģʽ
	RSSI_Mode 		= 1,		//RSSIģʽ
	Ranging_Mode 	= 2,		//���ģʽ������
	Config_Mode	 	= 3,		//����ģʽ
	Lowpower_Mode = 4			//�͹���ģʽ
}LoRaMode_enum;

#pragma pack (1) /*ָ����1�ֽڶ���*/

struct E28_2G4T_CONFIG_Typedef
{
	uint8_t Head;								//����ͷ
	uint16_t Addr;						  //ģ���ַ
	uint8_t AirSpeed			:	3	;	//bit0~2 ��������
	uint8_t SerialInfBaud	:	3	;	//bit3~5 ���ڲ�����
	uint8_t SerialInfMode	: 2 ;	//bit6~7 ����ģʽ
	uint8_t Channel;							//�ŵ�
	uint8_t Option;							//ѡ��
	
};

#pragma pack ()

struct LORA_Typedef
{
	uint8_t Status;
	uint8_t SleepTime;
	LoRaMode_enum Mode;
	uint16_t	RxLen;
	uint8_t RxData[LORA_BUF_LEN];
};

extern struct LORA_Typedef LoRa;

uint8_t LoRa_Init(void);				//��ʼ��
uint8_t LoRa_InterSleep(uint16_t time);	//����˯��ģʽ
uint8_t LoRa_SendData(uint8_t *pbuf,uint8_t len);		//��������

int8_t  LoRa_DataPro(uint8_t *rxbuf,uint16_t *pLen);					//�Խ��յ������ݽ��д���
void LoRa_ReadConfigPara(struct E28_2G4T_CONFIG_Typedef *config);
void LoRa_WriteConfigPara(struct E28_2G4T_CONFIG_Typedef config);
uint32_t LoRa_ParaToBaud(uint8_t SerialInfBaud);
uint8_t LoRa_RSSI_Read(void);
uint8_t LoRa_ModeExchange(LoRaMode_enum Mode,uint8_t check);
#endif //#if E28_2G4T12S_LORA_USING

#endif


