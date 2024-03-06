#include "SensorMessageStructure.h"
#include <string.h>
#include <math.h>
#include "SystemGlobalVar.h"
#include "CRC16.h"
#include "SIMIT_TCM_LoRa.h"
#include "E28_2G4T12S_LoRa.h"
#include "timestamp.h"
#include "ADC_App.h"
#include "ADXL355.h"


TowerTilted_Def TowerTilted	=	//杆塔倾斜报文
{
	{0,0x882,0},									// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
	{0,0x883,0},									// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	{0,0x884,0},									// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	{0,0x885,0},									// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	{0,0x886,0}										// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
};

Status_Def SensorStatus =  			//状态参量
{
	{1,29,2,100},	//电池剩余电量
	{0,30,4.2},		//电池电压
	{1,31,1,1},		//传感器自检状态
	{1,32,1,1},		//网络连接状态
	{1,33,1,1},		//电源状态
	{1,34,2,30},	//数据采集周期 000 100010 2 无符号整型
	{0,35,100},		//信号强度
	{0,5,25.5}    //温度
};

const TT_AlarmLimit_Def AlarmLimit_Init= //杆塔倾斜报警上下阈值
{
	{0,0x882,30,-30},									// 4 倾斜度 			上限值30mm/m,	下限值-30mm/m
	{0,0x883,30,-30},									// 4 顺线倾斜度 上限值30mm/m,	下限值-30mm/m
	{0,0x884,30,-30},									// 4 横向倾斜度 上限值30mm/m,	下限值-30mm/m
	{0,0x885,0,0},									// 4 顺线倾斜角 上限值30°,		下限值-40°
	{0,0x886,0,0}									// 4 横向倾斜角 上限值30°,		下限值-30°
};
/*
*【报文组帧】
	参数
		ParaLen		:												//参量个数
		FragInd		:												//分片指示
		PacketType:												//报文类型
	返回：
		无
*/
static void MsgFrameConstructAndSend(uint8_t	ParaLen,uint8_t	FragInd,uint8_t	PacketType,uint8_t * pbuf,uint16_t len)
{
	uint8_t SendBuf[128];
	uint16_t i;
	uint16_t crc;
	uint8_t *p;
	struct MessageStructure_Def *pMessage;
	pMessage = (struct MessageStructure_Def*)SendBuf;
	//ID
	p = (uint8_t *)&SystemGlobalVar.SensorID.ManufacturerID;
	SendBuf[0] = p[1];		//SystemGlobalVar.SensorID.ManufacturerID>>8;  	//高位在前
	SendBuf[1] = p[0];		//SystemGlobalVar.SensorID.ManufacturerID&0xff;  //
	SendBuf[2] = p[5];  	//高位在前
	SendBuf[3] = p[4];  	//高位在前
	SendBuf[4] = p[3];  	//高位在前
	SendBuf[5] = p[2];  	//高位在前
//	pMessage->SensorID.ManufacturerID= SystemGlobalVar.SensorID.ManufacturerID;
//	pMessage->SensorID.VersionLetter = SystemGlobalVar.SensorID.VersionLetter;
//	pMessage->SensorID.VersionNumber = SystemGlobalVar.SensorID.VersionNumber;
//	pMessage->SensorID.SerialNumber = SystemGlobalVar.SensorID.SerialNumber;	
	pMessage->DataLen = ParaLen;
	pMessage->FragInd = FragInd;
	pMessage->PacketType = PacketType;
	
	for(i=0;i<len;i++)
		pMessage->Data[i] = pbuf[i];
	
	crc = CRC16(SendBuf,i+7);  //报文头7字节
	
	pMessage->Data[i++] = crc>>8;  //高字节在前
	pMessage->Data[i++] = crc&0xff;
	
	LoRa_SendData(SendBuf,i+7);		//发送数据
}


//控制报文解析
void SensorControlAnalysis(uint8_t *pbuf)
{
	struct MessageStructure_Def *pMessage;
	struct GeneralParaQuerySetRes_Def *pCtlMsg;
	pMessage = (struct MessageStructure_Def*)pbuf;
	pCtlMsg = (struct GeneralParaQuerySetRes_Def*)pMessage->Data;
	uint8_t err;
	err = 0;
	SystemGlobalVar.Retry |= CMD_CONTROL;    //收到控制指令
	if(pCtlMsg->CtrlType==GeneralParaQuerySet_CTL)	// 	1	//传感器通用参数查询/设置
	{
		if(pCtlMsg->RequestSetFlag == SET_FLAG)
		{
			APP_LOG("传感器通用参数设置\r\n");
			SensorStatusSet(pCtlMsg->Data,pMessage->DataLen);
		}
		else
		{
			APP_LOG("传感器通用参数查询\r\n");
			SensorStatusUp(1);				 //传感器状态量上传给汇聚节点
		}
		
	}
	else if(pCtlMsg->CtrlType==MonitorDataQuery_CTL)	//	2 //传感器监测数据查询报文
	{
		if(pMessage->DataLen==0xf)		//取值为“1111”时，表示请求该传感器所能采集的所有参量。
		{
			APP_LOG("传感器监测数据查询报文响应-查询所有参量\r\n");
			pCtlMsg->Data[0] = STATUS_SUCCEED;
		}
		else
		{
			APP_LOG("传感器监测数据查询报文响应-错误\r\n");
			pCtlMsg->Data[0] = STATUS_FAIL;
			err = 1;
		}
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pCtlMsg,2); //
		if(err == 0)
		{
			HAL_Delay(100);
			SensorMonitoringDataUp(1);				 //监测数据报文上传给汇聚节点
		}
	}
		
	else if(pCtlMsg->CtrlType==AlarmPara_CTL)					//	3 //传感器告警参数查询/设置
	{
		if(pCtlMsg->RequestSetFlag == SET_FLAG)
		{
			APP_LOG("传感器告警参数设置\r\n");
			SensorAlarmLimitDataSet(pCtlMsg->Data,pMessage->DataLen);				 //报警上下限值设置
		}
		else
		{
			APP_LOG("传感器告警参数查询\r\n");
			SensorAlarmLimitDataRequest(pCtlMsg->Data,pMessage->DataLen);		//报警上下限值查询
		}
	}
		
	else if(pCtlMsg->CtrlType==TimePara_CTL)					//	4 //传感器时间参数查询/设置
	{
		struct TimeQuerySetRes_Def *pTimeMsg;
		pTimeMsg = (struct TimeQuerySetRes_Def*)pCtlMsg;
		if(SET_FLAG==pCtlMsg->RequestSetFlag)
		{
			APP_LOG("传感器时间参数设置响应\r\n");
			#if STM_RTC_USING
			if(pTimeMsg->SamplingPeriod>0)   //采样唤醒时间
			{
				__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
				HAL_RTCEx_SetWakeUpTimer(&hrtc, pTimeMsg->SamplingPeriod-1, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
				APP_LOG("传感器%d秒后唤醒采样\r\n",pTimeMsg->SamplingPeriod);
			}
			RTC_Set_Stamp(pTimeMsg->Timestamp);
			SystemGlobalVar.Retry &= 0x0f;
			SystemGlobalVar.Retry |= CMD_TIMING_END;    //时间同步完成
//			APP_LOG("传感器%d秒后唤醒采样\r\n",pTimeMsg->Timestamp);
			#endif
		}
		else
		{
			APP_LOG("传感器时间参数查询响应\r\n");
			#if STM_RTC_USING
			pTimeMsg->Timestamp = RTC_Get_Stamp(); //SystemGlobalVar.SamplingPeriod;  // //采样时间
			#endif
		}
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pTimeMsg,5); //
	}
	
	else if(pCtlMsg->CtrlType==ID_CTL)							  //	5 //传感器 ID 查询/设置
	{
		struct ID_QuerySetRes_Def *pID_Msg;
		uint8_t *pid;
		pID_Msg = (struct ID_QuerySetRes_Def*)pCtlMsg;
		if(SET_FLAG==pCtlMsg->RequestSetFlag)
		{
			APP_LOG("传感器ID设置\r\n");
			pid = (uint8_t *)&SystemGlobalVar.SensorID.ManufacturerID;;
//			SystemGlobalVar.SensorID.ManufacturerID=pID_Msg->NewSensorID.ManufacturerID;
//			SystemGlobalVar.SensorID.VersionLetter=pID_Msg->NewSensorID.VersionLetter;
//			SystemGlobalVar.SensorID.VersionNumber=pID_Msg->NewSensorID.VersionNumber;
//			SystemGlobalVar.SensorID.SerialNumber=pID_Msg->NewSensorID.SerialNumber;
			  pid[0] = pMessage->Data[2];  //高位在前
				pid[1] = pMessage->Data[1];
				pid[2] = pMessage->Data[6];
				pid[3] = pMessage->Data[5];
				pid[4] = pMessage->Data[4];
				pid[5] = pMessage->Data[3];
		}
//		pID_Msg->NewSensorID.ManufacturerID = SystemGlobalVar.SensorID.ManufacturerID;
//		pID_Msg->NewSensorID.VersionLetter = SystemGlobalVar.SensorID.VersionLetter;
//		pID_Msg->NewSensorID.VersionNumber = SystemGlobalVar.SensorID.VersionNumber;
//		pID_Msg->NewSensorID.SerialNumber = SystemGlobalVar.SensorID.SerialNumber;
		pMessage->Data[1] = pid[1];
		pMessage->Data[2] = pid[0];
		pMessage->Data[3] = pid[5];
		pMessage->Data[4] = pid[4];
		pMessage->Data[5] = pid[3];
		pMessage->Data[6] = pid[2];
		APP_LOG("传感器ID查询/设置响应\r\n");
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pID_Msg,7); //
	}
	
	else if(pCtlMsg->CtrlType==Reset_CLT)							//	6 //传感器复位设置
	{
		pCtlMsg->Data[0] = STATUS_SUCCEED;
		APP_LOG("传感器零点设置响应\r\n");
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pCtlMsg,2); //
		//复位传感器角度值
		I1_TowerTilted_StructDef pVals;
    SystemGlobalVar.TT_Offset.Angle_X =0;
		SystemGlobalVar.TT_Offset.Angle_Y = 0;
		GetTowerTiltedVals(&pVals);  //获取杆塔倾斜值
		SystemGlobalVar.TT_Offset.Inclination = pVals.Inclination;				// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
		SystemGlobalVar.TT_Offset.Inclination_X =	pVals.Inclination_X;		// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
		SystemGlobalVar.TT_Offset.Inclination_Y =	pVals.Inclination_Y;		// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
		SystemGlobalVar.TT_Offset.Angle_X =	pVals.Angle_X;								// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
		SystemGlobalVar.TT_Offset.Angle_Y =	pVals.Angle_Y;								// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
		SystemConfigSave();//保存配置
		SensorMonitoringDataAndStatusUp();							//监测数据报文与传感器状态量上传给汇聚节点
	}
	else if(pCtlMsg->CtrlType==Timing_CTL)						//	7 //传感器请求校时
	{	}
}

//控制报文
void SensorControl(uint8_t CtrlType,uint8_t RequestSetFlag,uint8_t *paralist,uint16_t len)
{
	uint8_t buf[128];
	uint16_t i;
	struct GeneralParaQuerySetRes_Def  *GeneralParaQuerySet;
	GeneralParaQuerySet = (struct GeneralParaQuerySetRes_Def *)buf;
	if(CtrlType==GeneralParaQuerySet_CTL)	// 	1	//传感器通用参数查询/设置
	{
		//APP_LOG("传感器通用参数查询/设置\r\n");
		GeneralParaQuerySet->CtrlType = GeneralParaQuerySet_CTL;
		GeneralParaQuerySet->RequestSetFlag = 0 ;  //该字段在当前报文中无意义， 默认取0
		MsgFrameConstructAndSend(0x0f,0,Control_MSG_TYPE,buf,1);
	}
		
	else if(CtrlType==MonitorDataQuery_CTL)	//	2 //传感器监测数据查询报文
	{
		GeneralParaQuerySet->CtrlType = MonitorDataQuery_CTL;
		GeneralParaQuerySet->RequestSetFlag = 0 ;  //该字段在当前报文中无意义， 默认取0
		MsgFrameConstructAndSend(0x0f,0,Control_MSG_TYPE,buf,1);
	}
		
	else if(CtrlType==AlarmPara_CTL)					//	3 //传感器告警参数查询/设置
	{
		GeneralParaQuerySet->CtrlType = AlarmPara_CTL;
		if(RequestSetFlag == REQUEST_FLAG)
		{
			GeneralParaQuerySet->RequestSetFlag = REQUEST_FLAG ;  //
			MsgFrameConstructAndSend(0x0f,0,Control_MSG_TYPE,buf,1);
		}
		else
		{
			GeneralParaQuerySet->RequestSetFlag = SET_FLAG ;  //
			if(paralist!=NULL)
			{
				for(i=0;i<len;i++)
					GeneralParaQuerySet->Data[i] = paralist[i];
			}
			else
			{
				for(i=0;i<len;i++)
					GeneralParaQuerySet->Data[i] = 0;
			}
			MsgFrameConstructAndSend(0x0f,0,Control_MSG_TYPE,buf,len+1);
		}
	}
		
	else if(CtrlType==TimePara_CTL)					//	4 //传感器时间参数查询/设置
	{

	}
	
	else if(CtrlType==ID_CTL)							  //	5 //传感器 ID 查询/设置
	{
	
	}
	
	else if(CtrlType==Reset_CLT)							//	6 //传感器复位设置
	{
		
	}
	else if(CtrlType==Timing_CTL)						//	7 //传感器请求校时
	{	
		GeneralParaQuerySet->CtrlType = Timing_CTL;
		GeneralParaQuerySet->RequestSetFlag = 0 ;  //该字段在当前报文中无意义， 默认取0
		MsgFrameConstructAndSend(0,0,Control_MSG_TYPE,buf,1);
	}
}

void SensorID_BigEnd(uint8_t *sID,uint8_t *pbuf)
{
	pbuf[0] = sID[1];
	pbuf[1] = sID[0];
	pbuf[2] = sID[5];
	pbuf[3] = sID[4];
	pbuf[4] = sID[3];
	pbuf[5] = sID[2];
}

void SensorMessagePro(uint8_t *pbuf,uint16_t len)
{
	struct MessageStructure_Def *pMessage;
	uint8_t id_buf[6];
	uint8_t i;
	uint16_t crc;
	APP_LOG("LoRaReceiveData(%d):",len);
	for(i=0;i<len;i++)	printf("0x%02X ",pbuf[i]);
	printf("\r\n\r\n");
	if(len>=9)
	{
		crc = (pbuf[len-2]<<8)|pbuf[len-1];  //高字节在前
		if(CRC16(pbuf,len-2)!= crc)  return ;//报文-2byte crc
		pMessage = (struct MessageStructure_Def*)pbuf;
		//pL_ID = (uint8_t*)&SystemGlobalVar.SensorID;
		//pF_ID = (uint8_t*)&pMessage->SensorID;
		SensorID_BigEnd((uint8_t*)&SystemGlobalVar.SensorID,id_buf);
		for(i=0;i<6;i++)  //ID 6个字节
		{
			if(id_buf[i]!=pbuf[i]) {APP_LOG("非本传感器ID\r\n");return ;}
		}
		
		switch(pMessage->PacketType)
		{
			case MonitoringData_MSG_TYPE:			//	0	 //监测数据报文
				APP_LOG("收到监测数据报文\r\n");
				break;
			case MonitoringDataRes_MSG_TYPE:	//	1	 //监测数据响应报文
				if(pMessage->Data[0]==STATUS_SUCCEED)	APP_LOG("监测数据响应Succeed\r\n");
				else 																	APP_LOG("监测数据响应Fail\r\n");
			  if((SystemGlobalVar.Retry&CMD_CONTROL)!=CMD_CONTROL)  //非控制命令下发送数据影响
				{
					SystemGlobalVar.Retry = 0;
					//SystemGlobalVar.Retry |= CMD_SEND_END;		//收到响应，取消重发
					SystemSleepTime = 0;
				}
				break;
			case Alarm_MSG_TYPE:							//	2	 //告警数据报文
				APP_LOG("收到告警数据报文\r\n");
				break;
			case AlarmRes_MSG_TYPE:						//	3	 //告警数据响应报文
				if(pMessage->Data[0]==STATUS_SUCCEED)	APP_LOG("告警数据响应Succeed\r\n");
				else 																	APP_LOG("告警数据响应Fail\r\n");
			  if((SystemGlobalVar.Retry&CMD_CONTROL)!=CMD_CONTROL)  //非控制命令下发送数据影响
				{
					SystemGlobalVar.Retry = 0;
					//SystemGlobalVar.Retry |= CMD_SEND_END;		//收到响应，取消重发
					SystemSleepTime = 0;
				}
				break;
			case Control_MSG_TYPE:						//	4	 //控制报文
				SensorControlAnalysis(pbuf);    //控制报文解析
				break;
			case ControlRes_MSG_TYPE:					//	5	 //控制响应报文
				break;
			case FragmentedResponse_MSG_TYPE:	//	6	 //分片数据应答报文
				break;
			case Reserved_MSG_TYPE:						//	7	 //预留
				break;
			default:
				break;
		}
	}
}


void GetTowerTiltedVals(I1_TowerTilted_StructDef *pVals)  //杆塔倾斜报文
{
	struct ADXL355_Typedef ADXL355;
	float Inclination_X_tan,Inclination_Y_tan;
	ADXL355_Data_Scan(&ADXL355);
	
	pVals->Angle_X=ADXL355.x_ang-SystemGlobalVar.TT_Offset.Angle_X;				// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	pVals->Angle_Y=ADXL355.y_ang-SystemGlobalVar.TT_Offset.Angle_Y;				// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）	
	
	
	Inclination_X_tan = (float)tan((double)(pVals->Angle_X*PI/180));
	Inclination_Y_tan = (float)tan((double)(pVals->Angle_Y*PI/180));
	pVals->Inclination_X = Inclination_X_tan*1000;	// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	pVals->Inclination_Y = Inclination_Y_tan*1000;	// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	
	pVals->Inclination=(float)sqrt(Inclination_X_tan*Inclination_X_tan + Inclination_Y_tan*Inclination_Y_tan)*1000;	// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
	
}

void SensorMonitoringDataUp(uint8_t flag)				 //监测数据报文上传给汇聚节点
{
	I1_TowerTilted_StructDef pVals;
	GetTowerTiltedVals(&pVals);
	TowerTilted.Inclination.Val_f = pVals.Inclination;				// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
	TowerTilted.Inclination_X.Val_f =	pVals.Inclination_X;		// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	TowerTilted.Inclination_Y.Val_f =	pVals.Inclination_Y;		// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	TowerTilted.Angle_X.Val_f =	pVals.Angle_X;								// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	TowerTilted.Angle_Y.Val_f =	pVals.Angle_Y;								// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	APP_PRINTF("\r\n【杆塔倾斜传感器参量列表】\r\n");
//	APP_PRINTF("第一个参数【倾斜度】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.1f\r\n",TowerTilted.Inclination.SensorType,TowerTilted.Inclination.LengthFlag,TowerTilted.Inclination.Val_f);
//	APP_PRINTF("第二个参数【顺线倾斜度】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.1f\r\n",TowerTilted.Inclination_X.SensorType,TowerTilted.Inclination_X.LengthFlag,TowerTilted.Inclination_X.Val_f);
//	APP_PRINTF("第三个参数【横向倾斜度】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.1f\r\n",TowerTilted.Inclination_Y.SensorType,TowerTilted.Inclination_Y.LengthFlag,TowerTilted.Inclination_Y.Val_f);
//	APP_PRINTF("第四个参数【顺线倾斜角】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.2f\r\n",TowerTilted.Angle_X.SensorType,TowerTilted.Angle_X.LengthFlag,TowerTilted.Angle_X.Val_f);
//	APP_PRINTF("第五个参数【横向倾斜角】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.2f\r\n",TowerTilted.Angle_Y.SensorType,TowerTilted.Angle_Y.LengthFlag,TowerTilted.Angle_Y.Val_f);
	APP_PRINTF("【倾斜度】：%.1f\r\n",TowerTilted.Inclination.Val_f);
	APP_PRINTF("【顺线倾斜度】：%.1f\r\n",TowerTilted.Inclination_X.Val_f);
	APP_PRINTF("【横向倾斜度】：%.1f\r\n",TowerTilted.Inclination_Y.Val_f);
	APP_PRINTF("【顺线倾斜角】：%.2f\r\n",TowerTilted.Angle_X.Val_f);
	APP_PRINTF("【横向倾斜角】：%.2f\r\n",TowerTilted.Angle_Y.Val_f);
	if(flag)
		//杆塔数据上传，5个参量，不分片，监测数据报文，杆塔参量列表，列表长度
		MsgFrameConstructAndSend(5,0,MonitoringData_MSG_TYPE,(uint8_t*)&TowerTilted,sizeof(TowerTilted_Def));
}

void SensorStatusUp(uint8_t flag)				 //传感器状态量上传给汇聚节点
{
	SensorStatus.BatVoltage.BatVoltage_f = Read_Bat_Voltage();		//电池电压
	SensorStatus.BatCapacity.BatCapacity_u16 = (uint16_t)((SensorStatus.BatVoltage.BatVoltage_f-3.6)/0.6*100);	//电池剩余电量=(V-3.6)/(4.2-3.6)*100;
	if(SensorStatus.BatCapacity.BatCapacity_u16>100)			SensorStatus.BatCapacity.BatCapacity_u16 = 100;
	else if(SensorStatus.BatCapacity.BatCapacity_u16<1)	SensorStatus.BatCapacity.BatCapacity_u16 = 1;
	SensorStatus.Sensor.Status_u8 = 1;		//传感器自检状态
	SensorStatus.NetWork.Status_u8 = LoRa.Status;		//网络连接状态
	SensorStatus.Power.Status_u8 = 1;			//电源状态
	SensorStatus.SamplingPeriod.SamplingPeriod_u16 = SystemGlobalVar.SamplingPeriod;	//数据采集周期 000 100010 2 无符号整型
	SensorStatus.RSSI.RSSI_f = 0 - (float)(256-LoRa_RSSI_Read());	//信号强度
	SensorStatus.Temp.Val_f = ADXL355_Read_Temp();
	
//	APP_PRINTF("\r\n【传感器状态参量列表】\r\n");
//	APP_PRINTF("第一个参数【电池剩余电量】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.BatCapacity.SensorType,SensorStatus.BatCapacity.LengthFlag,SensorStatus.BatCapacity.length,SensorStatus.BatCapacity.BatCapacity_u16);
//	APP_PRINTF("第二个参数【电池电压】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:NULL  Data:%.2f\r\n",SensorStatus.BatVoltage.SensorType,SensorStatus.BatVoltage.LengthFlag,SensorStatus.BatVoltage.BatVoltage_f);
//	APP_PRINTF("第三个参数【传感器自检状态】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.Sensor.SensorType,SensorStatus.Sensor.LengthFlag,SensorStatus.Sensor.length,SensorStatus.Sensor.Status_u8);
//	APP_PRINTF("第四个参数【网络连接状态】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.NetWork.SensorType,SensorStatus.NetWork.LengthFlag,SensorStatus.NetWork.length,SensorStatus.NetWork.Status_u8);
//	APP_PRINTF("第五个参数【电源状态】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.Power.SensorType,SensorStatus.Power.LengthFlag,SensorStatus.Power.length,SensorStatus.Power.Status_u8);
//	APP_PRINTF("第六个参数【数据采集周期】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.SamplingPeriod.SensorType,SensorStatus.SamplingPeriod.LengthFlag,SensorStatus.SamplingPeriod.length,SensorStatus.SamplingPeriod.SamplingPeriod_u16);
//	APP_PRINTF("第七个参数【信号强度】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:NULL  Data:%.2f\r\n",SensorStatus.RSSI.SensorType,SensorStatus.RSSI.LengthFlag,SensorStatus.RSSI.RSSI_f);
//	APP_PRINTF("第八个参数【温度】：");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:NULL  Data:%.2f\r\n\r\n",SensorStatus.Temp.SensorType,SensorStatus.Temp.LengthFlag,SensorStatus.Temp.Val_f);
	APP_PRINTF("\r\n【传感器状态参量列表】\r\n");
	APP_PRINTF("【电池剩余电量】：%d\r\n",SensorStatus.BatCapacity.BatCapacity_u16);
	APP_PRINTF("【电池电压】：%.2f\r\n",SensorStatus.BatVoltage.BatVoltage_f);
	APP_PRINTF("【传感器自检状态】：%d\r\n",SensorStatus.Sensor.Status_u8);
	APP_PRINTF("【LoRa连接状态】：%d\r\n",SensorStatus.NetWork.Status_u8);
	APP_PRINTF("【电源状态】：%d\r\n",SensorStatus.Power.Status_u8);
	APP_PRINTF("【数据采集周期(秒)】：%d\r\n",SensorStatus.SamplingPeriod.SamplingPeriod_u16);
	APP_PRINTF("【信号强度】：%.2f\r\n",SensorStatus.RSSI.RSSI_f);
	APP_PRINTF("【温度】：%.2f\r\n\r\n",SensorStatus.Temp.Val_f);
	if(flag)
		//状态量上传，7个参量，不分片，监测数据报文，状态参量列表，列表长度
		MsgFrameConstructAndSend(8,0,MonitoringData_MSG_TYPE,(uint8_t*)&SensorStatus,sizeof(Status_Def));
}

/*
*【通用参数设置】
	参数
		pbuf			:				//数据指针
		ParaLen		:			  //参量个数
	返回：
		无
*/
void SensorStatusSet(uint8_t *pbuf,uint8_t ParaLen)
{
	ParaLen = ParaLen;
	struct SamplingPeriod_Status_Def *pSamplingPeriod_Status;
	pSamplingPeriod_Status = (struct SamplingPeriod_Status_Def *)pbuf;
	if(pSamplingPeriod_Status->SensorType == SensorStatus.SamplingPeriod.SensorType)  //采样周期
	{
		SystemGlobalVar.SamplingPeriod = pSamplingPeriod_Status->SamplingPeriod_u16;	//数据采集周期
		APP_LOG("采样周期设置：%d\r\n\r\n",SystemGlobalVar.SamplingPeriod);
		//通用参数上传，最多8个参量，不分片，控制响应报文，杆塔参量列表，列表长度
		MsgFrameConstructAndSend(1,0,ControlRes_MSG_TYPE,pbuf,sizeof(struct SamplingPeriod_Status_Def));
		__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
		HAL_RTCEx_SetWakeUpTimer(&hrtc, SystemGlobalVar.SamplingPeriod-60, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);  //提前1分钟唤醒
		SystemConfigSave();  //保存配置
	}
}

//监测数据报文与传感器状态量上传给汇聚节点
void SensorMonitoringDataAndStatusUp(void)				 
{
	uint8_t i,index=0;
	uint8_t Buf[128],*p;
	SensorMonitoringDataUp(0);				 //监测数据报文
	SensorStatusUp(0);			 						//传感器状态量上传给汇聚节点
	p = (uint8_t*)&TowerTilted;
	for(i=0;i<sizeof(TowerTilted_Def);i++) Buf[index++] = *p++;
	p = (uint8_t*)&SensorStatus;
	for(i=0;i<sizeof(Status_Def);i++) Buf[index++] = *p++;
	//杆塔数据上传，5+7个参量，不分片，监测数据报文，杆塔参量列表，列表长度
	MsgFrameConstructAndSend(13,0,MonitoringData_MSG_TYPE,Buf,index);
}

/*
*【报警上下限值查询】
	参数
		pbuf			:				//数据指针
		ParaLen		:			  //参量个数
	返回：
		无
*/
void SensorAlarmLimitDataRequest(uint8_t *pbuf,uint8_t ParaLen)				 //报警上下限值查询
{
	uint8_t i,j;
	struct AlarmLimitPara4Byte_2f_Def *pAlarmTab;
	struct AlarmLimitPara4Byte_2f_Def	*pAlarmLimit; //杆塔倾斜报警上下阈值
	pAlarmLimit = (struct AlarmLimitPara4Byte_2f_Def *)&SystemGlobalVar.AlarmLimit;
	pAlarmTab = (struct AlarmLimitPara4Byte_2f_Def *)pbuf;
	if(ParaLen==0xf)  //所有参数
		ParaLen = 5;		//最多5个参数
	for(i=0;i<ParaLen;i++)
	{
		for(j=0;j<5;j++)
		{
			if(pAlarmTab[i].SensorType == pAlarmLimit[j].SensorType)
			{
				pAlarmTab[i].AlarmUpperLimit = pAlarmLimit[j].AlarmUpperLimit;
				pAlarmTab[i].AlarmLowerLimit = pAlarmLimit[j].AlarmLowerLimit;
				APP_LOG("报警上下限值查询：ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n\r\n",pAlarmLimit[j].SensorType,pAlarmLimit[j].LengthFlag,pAlarmLimit[j].AlarmUpperLimit,pAlarmLimit[j].AlarmLowerLimit);
				break;
			}	
		}
	}
	
	APP_LOG("【报警上下限值参量列表总览】\r\n");
	APP_PRINTF("第一个参数【倾斜度】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.SensorType,SystemGlobalVar.AlarmLimit.Inclination.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
	APP_PRINTF("第二个参数【顺线倾斜度】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.SensorType,SystemGlobalVar.AlarmLimit.Inclination_X.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
	APP_PRINTF("第三个参数【横向倾斜度】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.SensorType,SystemGlobalVar.AlarmLimit.Inclination_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
	APP_PRINTF("第四个参数【顺线倾斜角】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.SensorType,SystemGlobalVar.AlarmLimit.Angle_X.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
	APP_PRINTF("第五个参数【横向倾斜角】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.SensorType,SystemGlobalVar.AlarmLimit.Angle_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
	//报警上下限值上传，5个参量，不分片，告警数据响应报文，杆塔参量列表，列表长度
	MsgFrameConstructAndSend(ParaLen,0,AlarmRes_MSG_TYPE,pbuf,ParaLen*sizeof(struct AlarmLimitPara4Byte_2f_Def));
}

/*
*【报警上下限值设置】
	参数
		pbuf			:				//数据指针
		ParaLen		:			  //参量个数
	返回：
		无
*/
void SensorAlarmLimitDataSet(uint8_t *pbuf,uint8_t ParaLen)				 //报警上下限值设置
{
	uint8_t i,j;
	struct AlarmLimitPara4Byte_2f_Def *pAlarmTab;
	struct AlarmLimitPara4Byte_2f_Def	*pAlarmLimit; //杆塔倾斜报警上下阈值
	pAlarmLimit = (struct AlarmLimitPara4Byte_2f_Def *)&SystemGlobalVar.AlarmLimit;
	pAlarmTab = (struct AlarmLimitPara4Byte_2f_Def *)pbuf;
	if(ParaLen==0xf)  //所有参数
		ParaLen = 5;		//最多5个参数
	for(i=0;i<ParaLen;i++)
	{
		for(j=0;j<5;j++)
		{
			if(pAlarmTab[i].SensorType == pAlarmLimit[j].SensorType)
			{
				pAlarmLimit[j].AlarmUpperLimit = pAlarmTab[i].AlarmUpperLimit;
				pAlarmLimit[j].AlarmLowerLimit = pAlarmTab[i].AlarmLowerLimit;
				APP_LOG("报警上下限值设置：ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n\r\n",pAlarmLimit[j].SensorType,pAlarmLimit[j].LengthFlag,pAlarmLimit[j].AlarmUpperLimit,pAlarmLimit[j].AlarmLowerLimit);
				break;
			}	
		}
	}
	
	APP_LOG("【报警上下限值参量列表总览】\r\n");
	APP_PRINTF("第一个参数【倾斜度】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.SensorType,SystemGlobalVar.AlarmLimit.Inclination.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
	APP_PRINTF("第二个参数【顺线倾斜度】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.SensorType,SystemGlobalVar.AlarmLimit.Inclination_X.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
	APP_PRINTF("第三个参数【横向倾斜度】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.SensorType,SystemGlobalVar.AlarmLimit.Inclination_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
	APP_PRINTF("第四个参数【顺线倾斜角】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.SensorType,SystemGlobalVar.AlarmLimit.Angle_X.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
	APP_PRINTF("第五个参数【横向倾斜角】：");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.SensorType,SystemGlobalVar.AlarmLimit.Angle_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
	//报警上下限值上传，5个参量，不分片，告警数据响应报文，杆塔参量列表，列表长度
	MsgFrameConstructAndSend(ParaLen,0,AlarmRes_MSG_TYPE,pbuf,ParaLen*sizeof(struct AlarmLimitPara4Byte_2f_Def));
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************/
void Shell_SensorMonitoringDataUp(void)				 //监测数据报文上传给汇聚节点
{
	I1_TowerTilted_StructDef pVals;
	GetTowerTiltedVals(&pVals);
	TowerTilted.Inclination.Val_f = pVals.Inclination;				// 4 倾斜度（浮点数，精确到小数点后 1 位，单位： mm/m）
	TowerTilted.Inclination_X.Val_f =	pVals.Inclination_X;		// 4 顺线倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	TowerTilted.Inclination_Y.Val_f =	pVals.Inclination_Y;		// 4 横向倾斜度（浮点数，精确到小数点后 1 位，单位：mm/m）
	TowerTilted.Angle_X.Val_f =	pVals.Angle_X;								// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	TowerTilted.Angle_Y.Val_f =	pVals.Angle_Y;								// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
	printf("【杆塔倾斜传感器参量列表】\r\n");
	printf("【倾斜度】:%.1f\r\n",TowerTilted.Inclination.Val_f);
	printf("【顺线倾斜度】:%.1f\r\n",TowerTilted.Inclination_X.Val_f);
	printf("【横向倾斜度】:%.1f\r\n",TowerTilted.Inclination_Y.Val_f);
	printf("【顺线倾斜角】:%.2f\r\n",TowerTilted.Angle_X.Val_f);
	printf("【横向倾斜角】:%.2f\r\n\r\n",TowerTilted.Angle_Y.Val_f);
}

void Shell_SensorStatusUp(void)				 //传感器状态量上传给汇聚节点
{
	SensorStatus.BatVoltage.BatVoltage_f = Read_Bat_Voltage();		//电池电压
	SensorStatus.BatCapacity.BatCapacity_u16 = (uint16_t)((SensorStatus.BatVoltage.BatVoltage_f-3.6)/0.6*100);	//电池剩余电量=(V-3.6)/(4.2-3.6)*100;
	if(SensorStatus.BatCapacity.BatCapacity_u16>100)			SensorStatus.BatCapacity.BatCapacity_u16 = 100;
	else if(SensorStatus.BatCapacity.BatCapacity_u16<1)	SensorStatus.BatCapacity.BatCapacity_u16 = 1;
	SensorStatus.Sensor.Status_u8 = 1;		//传感器自检状态
	SensorStatus.NetWork.Status_u8 = 1;		//网络连接状态
	SensorStatus.Power.Status_u8 = 1;		//电源状态
	SensorStatus.SamplingPeriod.SamplingPeriod_u16 = SystemGlobalVar.SamplingPeriod;	//数据采集周期 000 100010 2 无符号整型
	SensorStatus.RSSI.RSSI_f = LoRa_RSSI_Read();		//信号强度
	SensorStatus.Temp.Val_f = ADXL355_Read_Temp();
	
	printf("【传感器状态参量列表】\r\n");
	printf("【电池剩余电量】:%d%%\r\n",SensorStatus.BatCapacity.BatCapacity_u16);
	printf("【电池电压】:%.2f\r\n",SensorStatus.BatVoltage.BatVoltage_f);
	printf("【传感器自检状态】:%d\r\n",SensorStatus.Sensor.Status_u8);
	printf("【网络连接状态】:%d\r\n",SensorStatus.NetWork.Status_u8);
	printf("【电源状态】:%d\r\n",SensorStatus.Power.Status_u8);
	printf("【数据采集周期】:%d\r\n",SensorStatus.SamplingPeriod.SamplingPeriod_u16);
	printf("【信号强度】:%.2f\r\n",SensorStatus.RSSI.RSSI_f);
	printf("【温度】:%.2f\r\n\r\n",SensorStatus.Temp.Val_f);
}

/*
*【报警上下限值查询】
	参数
		pbuf			:				//数据指针
		ParaLen		:			  //参量个数
	返回：
		无
*/
void Shell_SensorAlarmLimitDataRequest(void)				 //报警上下限值查询
{
	printf("【报警上下限值参量列表总览】\r\n");
//	printf("1.【倾斜度】   ： 上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
//	printf("2.【顺线倾斜度】：上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
//	printf("3.【横向倾斜度】：上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
//	printf("4.【顺线倾斜角】：上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
//	printf("5.【横向倾斜角】：上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
	printf("1.【顺线、横向倾斜角】：上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
}

/*
*【报警上下限值设置】
	参数
		pbuf			:				//数据指针
		ParaLen		:			  //参量个数
	返回：
		无
*/
//void Shell_SensorAlarmLimitDataSet(uint8_t *pbuf,uint8_t ParaLen)				 //报警上下限值设置
//{
//	uint8_t i,j;
//	struct AlarmLimitPara4Byte_2f_Def *pAlarmTab;
//	struct AlarmLimitPara4Byte_2f_Def	*pAlarmLimit; //杆塔倾斜报警上下阈值
//	pAlarmLimit = (struct AlarmLimitPara4Byte_2f_Def *)&SystemGlobalVar.AlarmLimit;
//	pAlarmTab = (struct AlarmLimitPara4Byte_2f_Def *)pbuf;
//	if(ParaLen==0xf)  //所有参数
//		ParaLen = 5;		//最多5个参数
//	for(i=0;i<ParaLen;i++)
//	{
//		for(j=0;j<5;j++)
//		{
//			if(pAlarmTab[i].SensorType == pAlarmLimit[j].SensorType)
//			{
//				pAlarmLimit[j].AlarmUpperLimit = pAlarmTab[i].AlarmUpperLimit;
//				pAlarmLimit[j].AlarmLowerLimit = pAlarmTab[i].AlarmLowerLimit;
//				APP_LOG("报警上下限值设置：ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n\r\n",pAlarmLimit[j].SensorType,pAlarmLimit[j].LengthFlag,pAlarmLimit[j].AlarmUpperLimit,pAlarmLimit[j].AlarmLowerLimit);
//				break;
//			}	
//		}
//	}
//	
//	printf("【报警上下限值参量列表总览】\r\n");
//	printf("第一个参数【倾斜度】：");
//	printf("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.SensorType,SystemGlobalVar.AlarmLimit.Inclination.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
//	printf("第二个参数【顺线倾斜度】：");
//	printf("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.SensorType,SystemGlobalVar.AlarmLimit.Inclination_X.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
//	printf("第三个参数【横向倾斜度】：");
//	printf("ParameterType:%x  LengthFlag:%d  上限值:%.1f  下限值:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.SensorType,SystemGlobalVar.AlarmLimit.Inclination_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
//	printf("第四个参数【顺线倾斜角】：");
//	printf("ParameterType:%x  LengthFlag:%d  上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.SensorType,SystemGlobalVar.AlarmLimit.Angle_X.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
//	printf("第五个参数【横向倾斜角】：");
//	printf("ParameterType:%x  LengthFlag:%d  上限值:%.2f  下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.SensorType,SystemGlobalVar.AlarmLimit.Angle_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
//	//报警上下限值上传，5个参量，不分片，告警数据响应报文，杆塔参量列表，列表长度
//	MsgFrameConstructAndSend(ParaLen,0,AlarmRes_MSG_TYPE,pbuf,ParaLen*sizeof(struct AlarmLimitPara4Byte_2f_Def));
//}

