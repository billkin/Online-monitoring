#include "E28_2G4T12S_LoRa.h"
#include "LoRa_uart.h"
#include <string.h>
#include <stdio.h>
#include "iwdg.h"

#if E28_2G4T12S_LORA_USING

struct LORA_Typedef LoRa;

uint32_t LoRa_ParaToBaud(uint8_t SerialInfBaud)
{
	if(SerialInfBaud==0) return 1200;
	if(SerialInfBaud==1) return 4800;
	if(SerialInfBaud==2) return 9600;
	if(SerialInfBaud==3) return 19200;
	if(SerialInfBaud==4) return 57600;
	if(SerialInfBaud==5) return 115200;	
	else								 return 9600;
//	printf("  0.波特率为\r\n");
//				printf("  1.波特率为4800\r\n");
//				printf("  2.波特率为9600\r\n");
//				printf("  3.波特率为19200\r\n\r\n");
//				printf("    4.波特率为57600\r\n");
//				printf("    5.波特率为115200\r\n");
}


uint8_t LoRa_ModeExchange(LoRaMode_enum Mode,uint8_t check)
{
	uint16_t timeout=0;
	//uint8_t res=0;
	IWDG_Feed();    			//喂狗
	if(check)
	{
		while(HAL_GPIO_ReadPin(LORA_AUX_PORT,LORA_AUX)==GPIO_PIN_RESET)
		{
			HAL_Delay(1);
			timeout++;
			if(timeout==2000) //2s
			{
				APP_LOG("LoRa模块模式设置超时1，请检查模块是否正常安装\r\n");
				return 1;
			}
		}
	}
	IWDG_Feed();    			//喂狗
	switch(Mode)
	{
		case Trans_Mode: 		// 0 传输模式  100
			LoRa.Mode = Trans_Mode;
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M1|LORA_M0, GPIO_PIN_RESET);
			//APP_LOG("LoRa into Trans Mode\r\n");
			break;
		case RSSI_Mode:			// 1 RSSI模式 101
			LoRa.Mode = RSSI_Mode;
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M2|LORA_M0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M1, GPIO_PIN_RESET);
			break;
		case Ranging_Mode:	// 2 测距模式，保留 110
			LoRa.Mode = Ranging_Mode;
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M2|LORA_M1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M0, GPIO_PIN_RESET);
			break;
		case Config_Mode:		// 3 配置模式 111
			LoRa.Mode = Config_Mode;
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M2|LORA_M1|LORA_M0, GPIO_PIN_SET);
			//APP_LOG("LoRa into Config Mode\r\n");
		  //set_baudrate(&LORA_Handler,9600);
			break;
		case Lowpower_Mode:	// 4 低功耗模式 0xx
			LoRa.Mode = Lowpower_Mode;
			HAL_GPIO_WritePin(LORA_M_PORT,LORA_M2, GPIO_PIN_RESET);
			break;
		default:break;
	}
	if(check)
	{
		timeout=0;
		while(HAL_GPIO_ReadPin(LORA_AUX_PORT,LORA_AUX)==GPIO_PIN_RESET)
		{
			HAL_Delay(10);
			timeout++;
			IWDG_Feed();     //喂狗
			if(timeout==500) //5s
			{
				APP_LOG("LoRa模块模式设置超时2，请检查模块是否正常安装\r\n");
				return 2;
			}
		}
	}
	return 0;
}



void LoRa_ReadConfigPara(struct E28_2G4T_CONFIG_Typedef *config)
{
	uint16_t time=500;	
	uint8_t buf[6]={0xC1,0xC1,0xC1,0,0,0};
	uint8_t *pCfg;
	pCfg = (uint8_t*)config;
	memset(pCfg, 0, 6);  //
	uint16_t len = 6;
	LoRa_ModeExchange(Config_Mode,1);		//进入配置模式
	HAL_Delay(10);
	LoRa_send_data(buf, 3);
	while(time--)
	{
		if(LoRa_DataPro(pCfg,&len))
		{
			for(len=0;len<6;len++)
//				APP_PRINTF("0x%x ",pCfg[len]);
//			APP_PRINTF("\r\n\r\n");
			return;
		}
		HAL_Delay(1);
	}
	APP_LOG("LoRa_ReadConfigPara Timeout!\r\n\r\n");
}

void LoRa_WriteConfigPara(struct E28_2G4T_CONFIG_Typedef config)
{
	uint8_t *pCfg;
	pCfg = (uint8_t*)&config;
	LoRa_ModeExchange(Config_Mode,1);		//进入配置模式
	HAL_Delay(20);
	LoRa_send_data(pCfg, 6);
	HAL_Delay(100);
}

uint8_t LoRa_Config()
{
	uint16_t timeout=0;
	while(LoRa_ModeExchange(Config_Mode,1))
	{
		HAL_Delay(1);
			timeout++;
			if(timeout==5000) //5s
				return 1;
	}
	//添加配置代码
	return 0;
}


uint8_t LoRa_Init(void)
{
	LoRa.SleepTime = 4;  							//4s休眠时间
	//LoRa_Config();
	if(LoRa_ModeExchange(Trans_Mode,1))  //传输模式
		LoRa.Status=0;
	else
		LoRa.Status=1;
	HAL_Delay(10);
	LoRa_UartParaInit();
	return 0;
}	

/**
功能：	LORA进入睡眠模式
参数：
		time：睡眠时间
返回：
		0：设置成功
		1：设置失败
**/
uint8_t LoRa_InterSleep(uint16_t time)
{
	LoRa_ModeExchange(Lowpower_Mode,0);
	return 0;
}

/**
功能：	LORA进入睡眠模式
参数：
		time：睡眠时间
返回：
		0：设置成功
		1：设置失败
**/
uint8_t LoRa_RSSI_Read(void)
{
	uint16_t timeout=0;
	uint8_t rssi=0;
	LoRa_ModeExchange(RSSI_Mode,1); 	//RSSI模式
	LoRa_Buf_clear();
	HAL_Delay(120);
	
	while(1)
	{
		IWDG_Feed();    			//喂狗
		HAL_Delay(1);
		if(LoRa_Rec_TimeOut_Process())	// 如果串口接收完成
		{
			rssi = LoRa_buf[0]; 	//十六进制补码
			break;
		}
		timeout++;
		if(timeout==1000) //1s
			break;
	}
	LoRa_ModeExchange(Trans_Mode,0); 	//传输模式
	return rssi;
}
/*********************************************************
发送数据
**********************************************************/
uint8_t LoRa_SendData(uint8_t *pbuf,uint8_t len)
{
	uint16_t i;
	APP_LOG("LoRaSendData(%d):",len);
	for(i=0;i<len;i++)
		APP_PRINTF("%02x ",pbuf[i]);
	APP_PRINTF("\r\n");
	LoRa_send_data(pbuf, len);
	return 0;
}

int8_t LoRa_DataPro(uint8_t *rxbuf,uint16_t *pLen)
{
	//uint8_t sendbuf[100];
	uint16_t len=0;
	//char *p;
	//uint16_t Index,i;
	if(LoRa_Rec_TimeOut_Process())			// 如果串口接收完成
	{
		for(len=0;len<LoRa_cnt;len++)   	//复制数据
		{
			if(len < *pLen)
			{
				rxbuf[len] = LoRa_buf[len];
			}
			else
			{
				break;
			}
		}
		//LoRa_Buf_clear();
		LoRa_cnt = 0;
		*pLen = len;
		if(LoRa.Mode ==Trans_Mode)				//传输模式
		{
//			APP_LOG("LoRa接收到传输数据:%d\r\n",*pLen);
			return 1;				//接收到数据	
		}	
		else if(LoRa.Mode ==Config_Mode)	//配置模式
		{
//			APP_LOG("LoRa接收到配置数据\r\n");
			return 2;				//接收到配置数据
		}
	}
	return 0;
}

#endif //#if E28_2G4T12S_LORA_USING

