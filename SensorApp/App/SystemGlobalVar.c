#include "SystemGlobalVar.h"
//#include "usart.h"
#include "stmflash.h"
#include "SIMIT_TCM_LoRa.h"
#include "main.h"

#define FLASH_INIT_FLAG 	0x12345679

//struct MessageStructure_Def	SensorMessage;
struct SystemGlobalVar_StructDef 	SystemGlobalVar;
//uint8_t GlobalData[LORA_BUF_LEN];

void NumToSerialNumberString(uint32_t num,uint8_t *str,uint8_t len)
{
	uint8_t i;
	for(i=len;i>0;i--)
	{
		str[i-1] = (num%10)+'0';
		num = num / 10;
	}
}



void SystemGlobalVarInitToFlash()
{
	uint32_t uid;
	uint8_t *pbuf1,*pbuf2;
	uint8_t i;
	APP_LOG("SystemGlobalVar Init To Flash...\r\n");
	HAL_Delay(10);
	SystemGlobalVar.InitFlag_u32 = FLASH_INIT_FLAG;
	SystemGlobalVar.PassWord[0]='1';SystemGlobalVar.PassWord[1]='2';SystemGlobalVar.PassWord[2]='3';SystemGlobalVar.PassWord[3]='4';
	
	SystemGlobalVar.LoRa_Config.Head = 0xC0;
	SystemGlobalVar.LoRa_Config.Addr = 0x00;						//模块地址 默认0
	SystemGlobalVar.LoRa_Config.SerialInfMode  = 0x00;  //串口通信模式：8N1
	SystemGlobalVar.LoRa_Config.SerialInfBaud  = 0x02;	//串口波特率：9600
	SystemGlobalVar.LoRa_Config.AirSpeed  = 0x03;				//空中速率:10K
	SystemGlobalVar.LoRa_Config.Channel  	= 0x18;				//通信信道
	SystemGlobalVar.LoRa_Config.Option    = 0x04;				//选项字(默认0x04)
	LoRa_WriteConfigPara(SystemGlobalVar.LoRa_Config);
	
	SystemGlobalVar.BJ_Time.sDate.Year=22;
	SystemGlobalVar.BJ_Time.sDate.Month = 01;
	SystemGlobalVar.BJ_Time.sDate.Date = 18;
	SystemGlobalVar.BJ_Time.sTime.Hours = 12;
	SystemGlobalVar.BJ_Time.sTime.Minutes = 30;
	SystemGlobalVar.BJ_Time.sTime.Seconds = 0;
	
	SystemGlobalVar.ResetTimeStamps = 604800;    //每七天重启一次硬件
	
	SystemGlobalVar.ADXL355.X_Offset = 0;
	SystemGlobalVar.ADXL355.Y_Offset = 0;
	SystemGlobalVar.ADXL355.Z_Offset = 0;
	SystemGlobalVar.SamplingPeriod = 30*60;    //30分钟
	SystemGlobalVar.SamplingFlg = 0;				//上传数据

	SystemGlobalVar.SensorID.ManufacturerID = ('m'<<8) +'y';//厂商代码
	SystemGlobalVar.SensorID.VersionLetter	=	1;	//版本标签,取值范围为[1-26]（十进制）， 分别依次代表字符[a-z]， 即 1 代表 a， 26 代表 z。
	SystemGlobalVar.SensorID.VersionNumber	=	1;	//版本号,取值范围为[0-63]（十进制）。
	uid = HAL_GetUIDw0();
	SystemGlobalVar.SensorID.SerialNumber	=	uid & 0x1FFFFF;	//产品序列号,取值范围为[0-2097151]

	SystemGlobalVar.TT_Offset.Inclination = 0;			// 4 倾斜度偏移值（浮点数，精确到小数点后 1 位，单位： mm/m）
	SystemGlobalVar.TT_Offset.Inclination_X =	0;		// 4 顺线倾斜度偏移值（浮点数，精确到小数点后 1 位，单位：mm/m）
	SystemGlobalVar.TT_Offset.Inclination_Y =	0;		// 4 横向倾斜度偏移值（浮点数，精确到小数点后 1 位，单位：mm/m）
	SystemGlobalVar.TT_Offset.Angle_X =	0;					// 4 顺线倾斜角偏移值（浮点数，精确到小数点后 2 位，单位：°）
	SystemGlobalVar.TT_Offset.Angle_Y =	0;					// 4 横向倾斜角偏移值（浮点数，精确到小数点后 2 位，单位：°
	
	pbuf1 = (uint8_t*)&SystemGlobalVar.AlarmLimit;
	pbuf2 = (uint8_t*)&AlarmLimit_Init;
	for(i=0;i<sizeof(TT_AlarmLimit_Def);i++)
		pbuf1[i] = pbuf2[i];
	STM_FLASH_Save(FLASH_SAVE_ADDR,(uint8_t*)&SystemGlobalVar,SYS_VAR_LEN);
	
	HAL_Delay(500);
//	__disable_irq() ;
//	__HAL_RCC_CLEAR_RESET_FLAGS();
	//HAL_NVIC_SystemReset();
	//HAL_Delay(500);
}

void SystemGlobalVar_init(void)
{
	uint8_t str[8];
	SystemGlobalVar.State=0;
	//APP_LOG("SYS_VAR_LEN:%d\r\n",SYS_VAR_LEN);
	HAL_Delay(10);
	STMFLASH_Read(FLASH_SAVE_ADDR,(uint8_t*)&SystemGlobalVar,SYS_VAR_LEN);   		//从指定地址开始读出指定长度的数据
	if(SystemGlobalVar.InitFlag_u32 != FLASH_INIT_FLAG)
	{
		SystemGlobalVarInitToFlash();
	}
	
	APP_LOG("ManufacturerID:%d\r\n",SystemGlobalVar.SensorID.ManufacturerID);
	APP_LOG("VersionLetter:%d\r\n",SystemGlobalVar.SensorID.VersionLetter);
	APP_LOG("VersionNumber:%d\r\n",SystemGlobalVar.SensorID.VersionNumber);
	NumToSerialNumberString(SystemGlobalVar.SensorID.SerialNumber,str,7);
	str[7] = '\0';
	APP_LOG("SerialNumber:%s\r\n\r\n",str);
	
//	APP_LOG("ADXL355.X_Offset: %d \r\n",SystemGlobalVar.ADXL355.X_Offset);
//	APP_LOG("ADXL355.Y_Offset: %d \r\n",SystemGlobalVar.ADXL355.Y_Offset);
//	APP_LOG("ADXL355.Z_Offset: %d \r\n\r\n",SystemGlobalVar.ADXL355.Z_Offset);
	HAL_Delay(10);
}


void SystemConfigSave(void)  //保存配置
{
	STM_FLASH_Save(FLASH_SAVE_ADDR,(uint8_t*)&SystemGlobalVar,SYS_VAR_LEN);
}

