#ifndef _SYSTEM_GLOBAL_VAR_H
#define _SYSTEM_GLOBAL_VAR_H

#include "main.h"
#include "SensorMessageStructure.h"
#include "timestamp.h"
#include "ADXL355.h"
#include "E28_2G4T12S_LoRa.h"

#pragma pack (1) /*指定按1字节对齐*/

struct SystemGlobalVar_StructDef
{
	uint32_t InitFlag_u32;
	uint8_t State;
	#if STM_RTC_USING
	struct bjTime BJ_Time;
	#endif
	uint32_t  TimeStamp;
//	uint8_t StateTime;
//	uint8_t BatChargingFlag;
//	float Inclination;			// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
//	float nclination_X;			// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
//	float Inclination_Y;		// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
//	float Angle_X;						// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
//	float Angle_Y;						// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°） 
	float Bat_Val;						//电压值
	I1_TowerTilted_StructDef TT_Offset;   //杆塔倾斜报文初始值，偏移值
	uint16_t SamplingPeriod;			/*采样周期*/
	uint8_t SamplingFlg;					/*采样周期*/
	char PassWord[4];  						/*终端密码*/
	
	TT_AlarmLimit_Def AlarmLimit; //杆塔倾斜报警上下阈值
	//struct MessageStructure_Def	SensorMessage;
	struct ManufacturerID_Def SensorID;      	//传感终端 ID
	struct ADXL355_XYZ_Offset_Def ADXL355;
	struct E28_2G4T_CONFIG_Typedef LoRa_Config;
	uint8_t Retry;   //
	uint32_t ResetTimeStamps;
	uint32_t PowerOnTimeStamps;
	
//	uint8_t NeedSendMsgFlg;
};
#pragma pack ()

#define SYS_VAR_LEN 	sizeof(struct SystemGlobalVar_StructDef)

//extern struct MessageStructure_Def	SensorMessage;
extern struct SystemGlobalVar_StructDef 	SystemGlobalVar;

void NumToSerialNumberString(uint32_t num,uint8_t *str,uint8_t len);

void SystemGlobalVarInitToFlash(void);
void SystemGlobalVar_init(void);
void SystemConfigSave(void);//保存配置
#endif


