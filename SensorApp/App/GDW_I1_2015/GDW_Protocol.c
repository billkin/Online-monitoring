#include "GDW_Protocol.h"
#include "RTU_CRC.h"
#include "stdlib.h"

Frame_StructDef *pGDW_Frame;
unsigned char Frame_No=0;
//unsigned char CMD_ID[17];
#if (USE_Meteorological)
	Meteorological_StructDef Meteorological;     //气象报文
#endif
#if (USE_TowerTilted)
	TowerTilted_StructDef TowerTilted;   //杆塔倾斜报文
#endif
NetworkAdapterQuerySetting_StructDef  *pNetworkAdapterQuerySetting;     //网络适配器查询/设置数据报文

//控制数据报（上位机to监测装置）
void ControlDataItem(unsigned char Packet_Type,unsigned char *pdata)
{
	switch(Packet_Type)
	{
		case 0xA1:								//状态监测装置网络适配器查询/设置
			pNetworkAdapterQuerySetting = (NetworkAdapterQuerySetting_StructDef*)pdata;
			//网络设置，并响应主机系统
			break;
		case 0xA2:								//上级设备请求状态监测装置历史数据
			
			break;
		case 0xA3:								//状态监测装置采样周期查询/设置
			break;
		case 0xA4:								//状态监测装置指向上位机的信息查询/设置
			break;
		case 0xA5:								//状态监测装置 ID 查询/设置
			break;
		case 0xA6:								//状态监测装置复位
			break;
		case 0xA7:								//状态监测装置模型参数配置信息查询/设置
			break;
		case 0xA8:								//控制数据报预留字段
		case 0xAF:	
			break;
		default:break;
	}
}
/*************************************************************************************
功能 ：解析一串数据是否包含一帧数据报文
参数
		pdata		：数据指针
		len			：数据长度
返回值：帧数据指针，当指针不为空时，frame指向的数据报文有效
**************************************************************************************/
Frame_StructDef *DataAnalysis(unsigned char *pdata,unsigned short len)
{
	unsigned char *p=NULL;
	unsigned short i,f_len;
	unsigned short  crc;
	for(i=0;i<len-27;i++)   //一帧数据至少包含28个数
	{
		if(pdata[i]==0xA5&&pdata[i+1]==0x5A)  //帧同步头
		{
			p=&pdata[i];
			break;
		}
	}
	if(p==NULL) return NULL;
	f_len = (unsigned short)(p[3]<<8) | p[2];
	if(p[f_len+26]!=0x96)  return NULL;  										//帧尾不是0x96，视为错误帧
        crc = RTU_CRC(&p[2],f_len+22);   												//CRC校验
	if(crc !=((p[f_len+26]<<25)|p[f_len+24])) return NULL; 	//CRC校验错误
	return (Frame_StructDef*)p;      												//帧数据有效
}

/*************************************************************************************
功能 ：对帧数据进行解析
参数
		frame					：指向有效帧数据指针
**************************************************************************************/
void FrameAnalysis(Frame_StructDef *frame)
{
	//比较 pGDW_Frame->CMD_ID
	switch(frame->Frame_Type)  //帧类型—参考附表 C8-1 相关含义
	{
		case 0x01:			// 监测数据报（监测装置to上位机）
			break;
		case 0x02:			//数据响应报（上位机to监测装置）
			if(frame->Payload[0]==0x00)   //数据发送状态： ①0xFF 成功  ②0x00 失败
			{
					//数据发送给失败，需要重发
			}
			break;
		case 0x03:			//控制数据报（上位机to监测装置）
			ControlDataItem(frame->Packet_Type,frame->Payload);
			break;
		case 0x04:			//控制响应报（监测装置to上位机）
			break;
		case 0x05:			//图像数据报（监测装置to上位机）
			break;
		case 0x06:			//图像数据响应报（ 上位机to监测装置）（暂未使用）
			break;
		case 0x07:			//图像控制报（上位机to监测装置）
			break;
		case 0x08:			//图像控制响应报（监测装置to上位机）
			break;
		case 0x09:			//工作状态报（监测装置to上位机）
			break;
		case 0x0A:			//工作状态响应报（上位机to监测装置）
			break;
		default:break;
	}
}

//打包数据
Frame_StructDef *DataPackingIntoFrames(unsigned char Frame_Type,unsigned char Packet_Type,unsigned char *pdata,unsigned short len)
{
	unsigned char *p;
        unsigned short i,crc;
	Frame_StructDef *pframe;
	pframe = (Frame_StructDef*)malloc(sizeof(Frame_StructDef)+len+3*sizeof(unsigned char));//变长分配内存: 报文内容(变长) + 校验位(2位) + 报文尾(1位) 最大1420
	pframe->Sync = 0x5AA5;   				//报文头
	pframe->Packet_Length = len;   	//报文长度
	for(i=0;i<17;i++)										//17 状态监测装置 ID（17 位编码）
	{
		//pframe->CMD_ID[i] = CMD_ID[i];
	}
	pframe->Frame_Type = Frame_Type;			//帧类型
	pframe->Packet_Type = Packet_Type;		// 报文类型	
	Frame_No++;
	pframe->Frame_No = Frame_No;					//帧序列号
	p = pdata;     													//气象报文
	for(i=0;i<pframe->Packet_Length;i++) //复制报文数据
	{
		pframe->Payload[i] = *p++;
	}
	p = (unsigned char*)&pframe->Packet_Length;  //计算crc
        crc = RTU_CRC(p,pframe->Packet_Length+22);
        pframe->Payload[pframe->Packet_Length] = crc&0xff;
        pframe->Payload[pframe->Packet_Length+1] = crc>>8;
	pframe->Payload[pframe->Packet_Length+2] = 0x96;  //报文尾：0x96
	return pframe;  //
	//free(pframe);   //释放内存
}


//监测数据报
//void MonitoringDatagram(unsigned char Frame_Type,unsigned char Packet_Type)
//{
//	unsigned char *p, *p2;
//	unsigned short i,crc;
//	
//	switch(Packet_Type)
//	{
//		case 0x01:				//气象环境类数据报
//			#if (USE_Meteorological)
//				DataPacking(Frame_Type,Packet_Type,(unsigned char*)&Meteorological,sizeof(Meteorological_StructDef));
//			#endif
//			break;
//		case 0x02:				//杆塔倾斜数据报
//			#if (USE_TowerTilted)
//				DataPacking(Frame_Type,Packet_Type,(unsigned char*)&TowerTilted,sizeof(TowerTilted_StructDef));   //杆塔倾斜报文
//			#endif
//			break;
//		case 0x03:				//导地线微风振动特征量数据报
//			break;
//		case 0x04:				//导地线微风振动波形信号数据报
//			break;
//		case 0x05:				//导线弧垂数据报
//			break;
//		case 0x06:				//导线温度数据报
//			break;
//		case 0x07:				//覆冰及不均衡张力差数据报
//			break;
//		case 0x08:				//导线风偏数据报
//			break;
//		case 0x09:				//导地线舞动特征量数据报
//			break;
//		case 0x0A:				//导地线舞动轨迹数据报
//			break;
//		case 0x0B:				//现场污秽度数据报
//			break;
//		case 0x0C:				//新类型数据报预留字段
//		case 0x0D:
//		case 0x0E:
//		case 0x0F:
//			break;
//		default:break;
//	}
//	
//}

////（监测装置to上位机）
//void CMD_to_CMA(unsigned char Frame_Type,unsigned char Packet_Type)
//{
//	switch(Frame_Type) //帧类型—参考附表 C8-1 相关含义
//	{
//		case 0x01:				// 监测数据报（监测装置to上位机）
//			switch(Packet_Type)
//			{
//				case 0x01:
//					break;
//			}
//			break;
//		case 0x04:				//控制响应报（监测装置to上位机）
//			break;
//		case 0x05:				//图像数据报（监测装置to上位机）
//			break;
//		case 0x08:				//图像控制响应报（监测装置to上位机）
//			break;
//		case 0x09:				//工作状态报（监测装置to上位机）
//			break;
//		default:break;
//	}
//}

//（监测装置to上位机）
unsigned char CMD_to_CMA(unsigned char Frame_Type,unsigned char Packet_Type,unsigned char *pdata,unsigned short len)
{
	Frame_StructDef *frame;
//	unsigned short frame_len;
	unsigned char *p;
	frame = DataPackingIntoFrames(Frame_Type,Packet_Type,pdata,len);
	p = (unsigned char*)frame;
	//SendData(p,frame_len);   //发送数据	
	return 0;
}

void GDW_Protocol_test(unsigned short Packet_Length)
{

}

