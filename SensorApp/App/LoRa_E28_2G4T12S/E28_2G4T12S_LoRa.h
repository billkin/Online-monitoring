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
	Trans_Mode 		= 0,		//传输模式
	RSSI_Mode 		= 1,		//RSSI模式
	Ranging_Mode 	= 2,		//测距模式，保留
	Config_Mode	 	= 3,		//配置模式
	Lowpower_Mode = 4			//低功耗模式
}LoRaMode_enum;

#pragma pack (1) /*指定按1字节对齐*/

struct E28_2G4T_CONFIG_Typedef
{
	uint8_t Head;								//命令头
	uint16_t Addr;						  //模块地址
	uint8_t AirSpeed			:	3	;	//bit0~2 空中速率
	uint8_t SerialInfBaud	:	3	;	//bit3~5 串口波特率
	uint8_t SerialInfMode	: 2 ;	//bit6~7 串口模式
	uint8_t Channel;							//信道
	uint8_t Option;							//选项
	
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

uint8_t LoRa_Init(void);				//初始化
uint8_t LoRa_InterSleep(uint16_t time);	//进入睡眠模式
uint8_t LoRa_SendData(uint8_t *pbuf,uint8_t len);		//发送数据

int8_t  LoRa_DataPro(uint8_t *rxbuf,uint16_t *pLen);					//对接收到的数据进行处理
void LoRa_ReadConfigPara(struct E28_2G4T_CONFIG_Typedef *config);
void LoRa_WriteConfigPara(struct E28_2G4T_CONFIG_Typedef config);
uint32_t LoRa_ParaToBaud(uint8_t SerialInfBaud);
uint8_t LoRa_RSSI_Read(void);
uint8_t LoRa_ModeExchange(LoRaMode_enum Mode,uint8_t check);
#endif //#if E28_2G4T12S_LORA_USING

#endif


