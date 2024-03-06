#ifndef __SENSOR_MESSAGE_H__
#define __SENSOR_MESSAGE_H__

#include "main.h"
#include "GDW_I1_2015.h"

////传感器的通信地址，高字节地址：0x00~0xFD 低字节地址：0x00~0xFF 
//#define MESSAGE_FRAME_TYPE	0	//业务信道信息帧
//#define REQ_FRAME_TYPE	 		1	//控制信道请求帧
//#define RSP_FRAME_TYPE 			2	//控制信道应答帧
//#define RSP_END_FRAME_TYPE 	3	//控制信道应答终帧
//#define BURST_FRAME_TYPE 		4	//突发数据帧
//#define ACK_FRAME_TYPE 			5	//控制信道确认帧
////#define RFU 保留备用

//据发送状态： ①0xFF成功 ②0x00失败。
#define STATUS_SUCCEED	0xFF
#define STATUS_FAIL			0x00

//参数配置类型标识 RequestSetFlag 1比特 0： 查询； 1： 设置
#define REQUEST_FLAG	0
#define SET_FLAG			1

#define CMD_CONTROL						0x80	    //控制命令
#define CMD_CONTROL_WAIT			0x40	    //等待控制命令
#define CMD_TIMING_END				0x20	    //时间同步完成
#define CMD_RETRY							0x10	    //指令/数据重发

#define MonitoringData_MSG_TYPE			0	 //监测数据报文
#define MonitoringDataRes_MSG_TYPE	1	 //监测数据响应报文
#define Alarm_MSG_TYPE							2	 //告警数据报文
#define AlarmRes_MSG_TYPE						3	 //告警数据响应报文
#define Control_MSG_TYPE						4	 //控制报文
#define ControlRes_MSG_TYPE					5	 //控制响应报文
#define FragmentedResponse_MSG_TYPE	6	 //分片数据应答报文
#define Reserved_MSG_TYPE						7	 //预留

#define GeneralParaQuerySet_CTL 1		//传感器通用参数查询/设置
#define MonitorDataQuery_CTL		2 	//传感器监测数据查询报文
#define AlarmPara_CTL						3 	//传感器告警参数查询/设置
#define TimePara_CTL						4 	//传感器时间参数查询/设置
#define ID_CTL									5 	//传感器 ID 查询/设置
#define Reset_CLT								6 	//传感器复位设置
#define Timing_CTL							7 	//传感器请求校时
//8-99 //协议扩展预留字段
//100-127 //厂商自定义预留字段

#pragma pack (1) /*指定按1字节对齐*/

//通用状态参量类型表
struct Para4Byte_f_Def           //参量头部
{
	uint16_t LengthFlag:	2		;	//bit0~1	数据长度字段指示位
	uint16_t SensorType:	14	;	//bit2~15	参量类型 000 11110
	float Val_f;								//4 浮点型
};
struct Para4Byte_u32_Def           //参量头部
{
	uint16_t LengthFlag:	2		;	//bit0~1	数据长度字段指示位
	uint16_t SensorType:	14	;	//bit2~15	参量类型 000 11110
	uint32_t Val_u32;						//4 浮点型
};

//电池剩余电量
struct BatCapacity_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 数据长度字段指示位, 1
	uint16_t SensorType:	14	;	//bit2~15	29 参量类型 29
	uint8_t	length;							//2 数据长度,依数据长度字段指示位而定， 该字段的长度依据数据长度字段标示位， x可以分别： NULL、 1、 2或3个字节。
	uint16_t BatCapacity_u16;		//数据采集周期  2 无符号整型
};
//电池电压
struct BatVoltage_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 数据长度字段指示位, 0
	uint16_t SensorType:	14	;	//bit2~15	30 参量类型 30
	float BatVoltage_f;					//电池电压  4 浮点型
};

//传感器自检状态
struct Sensor_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 数据长度字段指示位, 1
	uint16_t SensorType:	14	;	//bit2~15	31 参量类型 31
	uint8_t	length;							//1 数据长度,依数据长度字段指示位而定， 该字段的长度依据数据长度字段标示位， x可以分别： NULL、 1、 2或3个字节。
	uint8_t Status_u8;		//传感器自检状态 1、 正常； 2、异常；
};

//网络连接状态
struct NetWork_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 数据长度字段指示位,1
	uint16_t SensorType:	14	;	//bit2~15	32 参量类型
	uint8_t	length;							//1 数据长度,依数据长度字段指示位而定， 该字段的长度依据数据长度字段标示位， x可以分别： NULL、 1、 2或3个字节。
	uint8_t Status_u8;					//网络连接状态 1、 正常； 2、异常；
};

//电源状态
struct Power_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 数据长度字段指示位,1
	uint16_t SensorType:	14	;	//bit2~15	33 参量类型 33
	uint8_t	length;							//1 数据长度,依数据长度字段指示位而定， 该字段的长度依据数据长度字段标示位， x可以分别： NULL、 1、 2或3个字节。
	uint8_t Status_u8;					//电源状态 1、 正常； 2、异常；
};

//数据采集周期 000 100010 2 无符号整型
struct SamplingPeriod_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 数据长度字段指示位,1
	uint16_t SensorType:	14	;	//bit2~15	34 参量类型
	uint8_t	length;							//2 数据长度,依数据长度字段指示位而定， 该字段的长度依据数据长度字段标示位， x可以分别： NULL、 1、 2或3个字节。
	uint16_t SamplingPeriod_u16;//数据采集周期  2 无符号整型
};

//信号强度
struct RSSI_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1 	数据长度字段指示位,0
	uint16_t SensorType:	14	;	//bit2~15	35 参量类型
	float RSSI_f;					//信号强度 4 浮点型
};

/**
**状态参量
**/
typedef struct
{
	//电池剩余电量
	struct BatCapacity_Status_Def BatCapacity;
	//电池电压
	struct BatVoltage_Status_Def	BatVoltage;
	//传感器自检状态
	struct Sensor_Status_Def			Sensor;
	//网络连接状态
	struct NetWork_Status_Def			NetWork;
	//电源状态
	struct Power_Status_Def				Power;
	//数据采集周期 000 100010 2 无符号整型
	struct SamplingPeriod_Status_Def	SamplingPeriod;
	//信号强度
	struct RSSI_Status_Def			RSSI;
	//温度
	struct Para4Byte_f_Def  Temp;
}Status_Def;   										//状态参量


/**
**输电业务状态参量类型表
**/
//杆塔倾斜
typedef struct
{
	struct Para4Byte_f_Def Inclination;										// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
	struct Para4Byte_f_Def Inclination_X;									// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	struct Para4Byte_f_Def Inclination_Y;									// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	struct Para4Byte_f_Def Angle_X;												// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	struct Para4Byte_f_Def Angle_Y;												// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
}TowerTilted_Def;   										//杆塔倾斜报文


struct AlarmLimitPara4Byte_2f_Def      //参量头部
{
	uint16_t LengthFlag:	2		;	//bit0~1 	数据长度字段指示位
	uint16_t SensorType:	14	;	//bit2~15	参量类型 000 11110
	float AlarmUpperLimit;			//4 浮点型
	float AlarmLowerLimit;
};

typedef struct
{
	struct AlarmLimitPara4Byte_2f_Def Inclination;			// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
	struct AlarmLimitPara4Byte_2f_Def Inclination_X;		// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	struct AlarmLimitPara4Byte_2f_Def Inclination_Y;		// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	struct AlarmLimitPara4Byte_2f_Def Angle_X;					// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	struct AlarmLimitPara4Byte_2f_Def Angle_Y;					// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
}TT_AlarmLimit_Def;   										//杆塔倾斜报警上下阈值


//传感器通用参数查询/设置报文响应格式规范
struct GeneralParaQuerySetRes_Def
{
	uint8_t RequestSetFlag:	1;					//bit0	参数配置类型标识,0： 查询； 1： 设置
	uint8_t CtrlType:	7;								//bit1~7	控制报文类型
	uint8_t Data[];											//响应数据
};

//传感器时间查询/设置报文格式
struct TimeQuerySetRes_Def
{
	uint8_t RequestSetFlag:	1;					//bit0	参数配置类型标识,0： 查询； 1： 设置
	uint8_t CtrlType:	7;								//bit1~7	控制报文类型
	uint32_t Timestamp;									//时间戳
	uint16_t SamplingPeriod;						//为方便对时同步唤醒而添加的字段/*采样周期*/（标准协议没有这个字段）
};

struct ManufacturerID_Def      	//传感终端 ID
{
	uint16_t 	ManufacturerID;			//厂商代码
	uint32_t	SerialNumber :	21;	//0~20bit  产品序列号
	uint32_t	VersionNumber:	6	;	//21~26bit 版本号
	uint32_t	VersionLetter:	5	;	//27~31bit 版本标签
};

//传感器 ID 查询/设置报文格式
struct ID_QuerySetRes_Def
{
	uint8_t RequestSetFlag:	1;								//bit0    参数配置类型标识,0： 查询； 1： 设置
	uint8_t CtrlType:	7;											//bit 1~7 控制报文类型
	struct ManufacturerID_Def NewSensorID;		//传感终端 ID									
};

struct MessageStructure_Def
{
	struct ManufacturerID_Def SensorID;		//传感终端ID
	uint8_t	PacketType:	3;								//bit0~2 	报文类型
	uint8_t	FragInd		:	1;								//bit3   	分片指示
	uint8_t	DataLen		:	4;								//bit4~7	参量个数
	uint8_t Data[];												//报文内容+crc16
};

struct FragInd_PDU_Def
{
	/*****分片头*********************************************/
	uint8_t SSEQ		:	6;	//bit0~5 	业务数据单元序号,取值从1开始， 连续取值。
	uint8_t FLAG		:	2;	//bit6~7	分片属性
	uint8_t PSEQ		:	7;	//bit0~6 	协议数据单元序号
	uint8_t Priority:	1;	//bit7 		协议数据单元优先级,0表示低， 1表示高
	/*****分片数据内容*********************************************/
	uint16_t SIZE;				//数据内容长度,数据内容（分片） 的长度（字节数） 。
	uint8_t Data[];				//分片后的PDU数据内容
};

#pragma pack ()

extern const TT_AlarmLimit_Def AlarmLimit_Init; //杆塔倾斜报警上下阈值

void GetTowerTiltedVals(I1_TowerTilted_StructDef *pVals);  //获取杆塔倾斜值
void SensorMonitoringDataUp(uint8_t flag);				 //监测数据报文上传给汇聚节点
void SensorStatusUp(uint8_t flag);								 //传感器状态量上传给汇聚节点
void SensorStatusSet(uint8_t *pbuf,uint8_t ParaLen);
void SensorMonitoringDataAndStatusUp(void);					//监测数据报文与传感器状态量上传给汇聚节点

void SensorAlarmLimitDataRequest(uint8_t *pbuf,uint8_t ParaLen);		//报警上下限值查询
void SensorAlarmLimitDataSet(uint8_t *pbuf,uint8_t ParaLen);				 //报警上下限值设置

void SensorMessagePro(uint8_t *pbuf,uint16_t len);

void SensorControl(uint8_t CtrlType,uint8_t RequestSetFlag,uint8_t *paralist,uint16_t len); //控制报文

/*********************************************************************************************/
/**     终端打印调用                                                                         **/
/*********************************************************************************************/
void Shell_SensorMonitoringDataUp(void);				 //监测数据报文上传给汇聚节点
void Shell_SensorStatusUp(void);				 					//传感器状态量上传给汇聚节点
void Shell_SensorAlarmLimitDataRequest(void);			//报警上下限值查询

#endif /* __SENSOR_MESSAGE_H__ */


