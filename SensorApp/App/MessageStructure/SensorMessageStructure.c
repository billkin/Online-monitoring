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


TowerTilted_Def TowerTilted	=	//������б����
{
	{0,0x882,0},									// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
	{0,0x883,0},									// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	{0,0x884,0},									// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	{0,0x885,0},									// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	{0,0x886,0}										// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
};

Status_Def SensorStatus =  			//״̬����
{
	{1,29,2,100},	//���ʣ�����
	{0,30,4.2},		//��ص�ѹ
	{1,31,1,1},		//�������Լ�״̬
	{1,32,1,1},		//��������״̬
	{1,33,1,1},		//��Դ״̬
	{1,34,2,30},	//���ݲɼ����� 000 100010 2 �޷�������
	{0,35,100},		//�ź�ǿ��
	{0,5,25.5}    //�¶�
};

const TT_AlarmLimit_Def AlarmLimit_Init= //������б����������ֵ
{
	{0,0x882,30,-30},									// 4 ��б�� 			����ֵ30mm/m,	����ֵ-30mm/m
	{0,0x883,30,-30},									// 4 ˳����б�� ����ֵ30mm/m,	����ֵ-30mm/m
	{0,0x884,30,-30},									// 4 ������б�� ����ֵ30mm/m,	����ֵ-30mm/m
	{0,0x885,0,0},									// 4 ˳����б�� ����ֵ30��,		����ֵ-40��
	{0,0x886,0,0}									// 4 ������б�� ����ֵ30��,		����ֵ-30��
};
/*
*��������֡��
	����
		ParaLen		:												//��������
		FragInd		:												//��Ƭָʾ
		PacketType:												//��������
	���أ�
		��
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
	SendBuf[0] = p[1];		//SystemGlobalVar.SensorID.ManufacturerID>>8;  	//��λ��ǰ
	SendBuf[1] = p[0];		//SystemGlobalVar.SensorID.ManufacturerID&0xff;  //
	SendBuf[2] = p[5];  	//��λ��ǰ
	SendBuf[3] = p[4];  	//��λ��ǰ
	SendBuf[4] = p[3];  	//��λ��ǰ
	SendBuf[5] = p[2];  	//��λ��ǰ
//	pMessage->SensorID.ManufacturerID= SystemGlobalVar.SensorID.ManufacturerID;
//	pMessage->SensorID.VersionLetter = SystemGlobalVar.SensorID.VersionLetter;
//	pMessage->SensorID.VersionNumber = SystemGlobalVar.SensorID.VersionNumber;
//	pMessage->SensorID.SerialNumber = SystemGlobalVar.SensorID.SerialNumber;	
	pMessage->DataLen = ParaLen;
	pMessage->FragInd = FragInd;
	pMessage->PacketType = PacketType;
	
	for(i=0;i<len;i++)
		pMessage->Data[i] = pbuf[i];
	
	crc = CRC16(SendBuf,i+7);  //����ͷ7�ֽ�
	
	pMessage->Data[i++] = crc>>8;  //���ֽ���ǰ
	pMessage->Data[i++] = crc&0xff;
	
	LoRa_SendData(SendBuf,i+7);		//��������
}


//���Ʊ��Ľ���
void SensorControlAnalysis(uint8_t *pbuf)
{
	struct MessageStructure_Def *pMessage;
	struct GeneralParaQuerySetRes_Def *pCtlMsg;
	pMessage = (struct MessageStructure_Def*)pbuf;
	pCtlMsg = (struct GeneralParaQuerySetRes_Def*)pMessage->Data;
	uint8_t err;
	err = 0;
	SystemGlobalVar.Retry |= CMD_CONTROL;    //�յ�����ָ��
	if(pCtlMsg->CtrlType==GeneralParaQuerySet_CTL)	// 	1	//������ͨ�ò�����ѯ/����
	{
		if(pCtlMsg->RequestSetFlag == SET_FLAG)
		{
			APP_LOG("������ͨ�ò�������\r\n");
			SensorStatusSet(pCtlMsg->Data,pMessage->DataLen);
		}
		else
		{
			APP_LOG("������ͨ�ò�����ѯ\r\n");
			SensorStatusUp(1);				 //������״̬���ϴ�����۽ڵ�
		}
		
	}
	else if(pCtlMsg->CtrlType==MonitorDataQuery_CTL)	//	2 //������������ݲ�ѯ����
	{
		if(pMessage->DataLen==0xf)		//ȡֵΪ��1111��ʱ����ʾ����ô��������ܲɼ������в�����
		{
			APP_LOG("������������ݲ�ѯ������Ӧ-��ѯ���в���\r\n");
			pCtlMsg->Data[0] = STATUS_SUCCEED;
		}
		else
		{
			APP_LOG("������������ݲ�ѯ������Ӧ-����\r\n");
			pCtlMsg->Data[0] = STATUS_FAIL;
			err = 1;
		}
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pCtlMsg,2); //
		if(err == 0)
		{
			HAL_Delay(100);
			SensorMonitoringDataUp(1);				 //������ݱ����ϴ�����۽ڵ�
		}
	}
		
	else if(pCtlMsg->CtrlType==AlarmPara_CTL)					//	3 //�������澯������ѯ/����
	{
		if(pCtlMsg->RequestSetFlag == SET_FLAG)
		{
			APP_LOG("�������澯��������\r\n");
			SensorAlarmLimitDataSet(pCtlMsg->Data,pMessage->DataLen);				 //����������ֵ����
		}
		else
		{
			APP_LOG("�������澯������ѯ\r\n");
			SensorAlarmLimitDataRequest(pCtlMsg->Data,pMessage->DataLen);		//����������ֵ��ѯ
		}
	}
		
	else if(pCtlMsg->CtrlType==TimePara_CTL)					//	4 //������ʱ�������ѯ/����
	{
		struct TimeQuerySetRes_Def *pTimeMsg;
		pTimeMsg = (struct TimeQuerySetRes_Def*)pCtlMsg;
		if(SET_FLAG==pCtlMsg->RequestSetFlag)
		{
			APP_LOG("������ʱ�����������Ӧ\r\n");
			#if STM_RTC_USING
			if(pTimeMsg->SamplingPeriod>0)   //��������ʱ��
			{
				__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//���RTC WAKE UP�ı�־
				HAL_RTCEx_SetWakeUpTimer(&hrtc, pTimeMsg->SamplingPeriod-1, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
				APP_LOG("������%d����Ѳ���\r\n",pTimeMsg->SamplingPeriod);
			}
			RTC_Set_Stamp(pTimeMsg->Timestamp);
			SystemGlobalVar.Retry &= 0x0f;
			SystemGlobalVar.Retry |= CMD_TIMING_END;    //ʱ��ͬ�����
//			APP_LOG("������%d����Ѳ���\r\n",pTimeMsg->Timestamp);
			#endif
		}
		else
		{
			APP_LOG("������ʱ�������ѯ��Ӧ\r\n");
			#if STM_RTC_USING
			pTimeMsg->Timestamp = RTC_Get_Stamp(); //SystemGlobalVar.SamplingPeriod;  // //����ʱ��
			#endif
		}
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pTimeMsg,5); //
	}
	
	else if(pCtlMsg->CtrlType==ID_CTL)							  //	5 //������ ID ��ѯ/����
	{
		struct ID_QuerySetRes_Def *pID_Msg;
		uint8_t *pid;
		pID_Msg = (struct ID_QuerySetRes_Def*)pCtlMsg;
		if(SET_FLAG==pCtlMsg->RequestSetFlag)
		{
			APP_LOG("������ID����\r\n");
			pid = (uint8_t *)&SystemGlobalVar.SensorID.ManufacturerID;;
//			SystemGlobalVar.SensorID.ManufacturerID=pID_Msg->NewSensorID.ManufacturerID;
//			SystemGlobalVar.SensorID.VersionLetter=pID_Msg->NewSensorID.VersionLetter;
//			SystemGlobalVar.SensorID.VersionNumber=pID_Msg->NewSensorID.VersionNumber;
//			SystemGlobalVar.SensorID.SerialNumber=pID_Msg->NewSensorID.SerialNumber;
			  pid[0] = pMessage->Data[2];  //��λ��ǰ
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
		APP_LOG("������ID��ѯ/������Ӧ\r\n");
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pID_Msg,7); //
	}
	
	else if(pCtlMsg->CtrlType==Reset_CLT)							//	6 //��������λ����
	{
		pCtlMsg->Data[0] = STATUS_SUCCEED;
		APP_LOG("���������������Ӧ\r\n");
		MsgFrameConstructAndSend(0,0,ControlRes_MSG_TYPE,(uint8_t*)pCtlMsg,2); //
		//��λ�������Ƕ�ֵ
		I1_TowerTilted_StructDef pVals;
    SystemGlobalVar.TT_Offset.Angle_X =0;
		SystemGlobalVar.TT_Offset.Angle_Y = 0;
		GetTowerTiltedVals(&pVals);  //��ȡ������бֵ
		SystemGlobalVar.TT_Offset.Inclination = pVals.Inclination;				// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
		SystemGlobalVar.TT_Offset.Inclination_X =	pVals.Inclination_X;		// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
		SystemGlobalVar.TT_Offset.Inclination_Y =	pVals.Inclination_Y;		// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
		SystemGlobalVar.TT_Offset.Angle_X =	pVals.Angle_X;								// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
		SystemGlobalVar.TT_Offset.Angle_Y =	pVals.Angle_Y;								// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
		SystemConfigSave();//��������
		SensorMonitoringDataAndStatusUp();							//������ݱ����봫����״̬���ϴ�����۽ڵ�
	}
	else if(pCtlMsg->CtrlType==Timing_CTL)						//	7 //����������Уʱ
	{	}
}

//���Ʊ���
void SensorControl(uint8_t CtrlType,uint8_t RequestSetFlag,uint8_t *paralist,uint16_t len)
{
	uint8_t buf[128];
	uint16_t i;
	struct GeneralParaQuerySetRes_Def  *GeneralParaQuerySet;
	GeneralParaQuerySet = (struct GeneralParaQuerySetRes_Def *)buf;
	if(CtrlType==GeneralParaQuerySet_CTL)	// 	1	//������ͨ�ò�����ѯ/����
	{
		//APP_LOG("������ͨ�ò�����ѯ/����\r\n");
		GeneralParaQuerySet->CtrlType = GeneralParaQuerySet_CTL;
		GeneralParaQuerySet->RequestSetFlag = 0 ;  //���ֶ��ڵ�ǰ�����������壬 Ĭ��ȡ0
		MsgFrameConstructAndSend(0x0f,0,Control_MSG_TYPE,buf,1);
	}
		
	else if(CtrlType==MonitorDataQuery_CTL)	//	2 //������������ݲ�ѯ����
	{
		GeneralParaQuerySet->CtrlType = MonitorDataQuery_CTL;
		GeneralParaQuerySet->RequestSetFlag = 0 ;  //���ֶ��ڵ�ǰ�����������壬 Ĭ��ȡ0
		MsgFrameConstructAndSend(0x0f,0,Control_MSG_TYPE,buf,1);
	}
		
	else if(CtrlType==AlarmPara_CTL)					//	3 //�������澯������ѯ/����
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
		
	else if(CtrlType==TimePara_CTL)					//	4 //������ʱ�������ѯ/����
	{

	}
	
	else if(CtrlType==ID_CTL)							  //	5 //������ ID ��ѯ/����
	{
	
	}
	
	else if(CtrlType==Reset_CLT)							//	6 //��������λ����
	{
		
	}
	else if(CtrlType==Timing_CTL)						//	7 //����������Уʱ
	{	
		GeneralParaQuerySet->CtrlType = Timing_CTL;
		GeneralParaQuerySet->RequestSetFlag = 0 ;  //���ֶ��ڵ�ǰ�����������壬 Ĭ��ȡ0
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
		crc = (pbuf[len-2]<<8)|pbuf[len-1];  //���ֽ���ǰ
		if(CRC16(pbuf,len-2)!= crc)  return ;//����-2byte crc
		pMessage = (struct MessageStructure_Def*)pbuf;
		//pL_ID = (uint8_t*)&SystemGlobalVar.SensorID;
		//pF_ID = (uint8_t*)&pMessage->SensorID;
		SensorID_BigEnd((uint8_t*)&SystemGlobalVar.SensorID,id_buf);
		for(i=0;i<6;i++)  //ID 6���ֽ�
		{
			if(id_buf[i]!=pbuf[i]) {APP_LOG("�Ǳ�������ID\r\n");return ;}
		}
		
		switch(pMessage->PacketType)
		{
			case MonitoringData_MSG_TYPE:			//	0	 //������ݱ���
				APP_LOG("�յ�������ݱ���\r\n");
				break;
			case MonitoringDataRes_MSG_TYPE:	//	1	 //���������Ӧ����
				if(pMessage->Data[0]==STATUS_SUCCEED)	APP_LOG("���������ӦSucceed\r\n");
				else 																	APP_LOG("���������ӦFail\r\n");
			  if((SystemGlobalVar.Retry&CMD_CONTROL)!=CMD_CONTROL)  //�ǿ��������·�������Ӱ��
				{
					SystemGlobalVar.Retry = 0;
					//SystemGlobalVar.Retry |= CMD_SEND_END;		//�յ���Ӧ��ȡ���ط�
					SystemSleepTime = 0;
				}
				break;
			case Alarm_MSG_TYPE:							//	2	 //�澯���ݱ���
				APP_LOG("�յ��澯���ݱ���\r\n");
				break;
			case AlarmRes_MSG_TYPE:						//	3	 //�澯������Ӧ����
				if(pMessage->Data[0]==STATUS_SUCCEED)	APP_LOG("�澯������ӦSucceed\r\n");
				else 																	APP_LOG("�澯������ӦFail\r\n");
			  if((SystemGlobalVar.Retry&CMD_CONTROL)!=CMD_CONTROL)  //�ǿ��������·�������Ӱ��
				{
					SystemGlobalVar.Retry = 0;
					//SystemGlobalVar.Retry |= CMD_SEND_END;		//�յ���Ӧ��ȡ���ط�
					SystemSleepTime = 0;
				}
				break;
			case Control_MSG_TYPE:						//	4	 //���Ʊ���
				SensorControlAnalysis(pbuf);    //���Ʊ��Ľ���
				break;
			case ControlRes_MSG_TYPE:					//	5	 //������Ӧ����
				break;
			case FragmentedResponse_MSG_TYPE:	//	6	 //��Ƭ����Ӧ����
				break;
			case Reserved_MSG_TYPE:						//	7	 //Ԥ��
				break;
			default:
				break;
		}
	}
}


void GetTowerTiltedVals(I1_TowerTilted_StructDef *pVals)  //������б����
{
	struct ADXL355_Typedef ADXL355;
	float Inclination_X_tan,Inclination_Y_tan;
	ADXL355_Data_Scan(&ADXL355);
	
	pVals->Angle_X=ADXL355.x_ang-SystemGlobalVar.TT_Offset.Angle_X;				// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	pVals->Angle_Y=ADXL355.y_ang-SystemGlobalVar.TT_Offset.Angle_Y;				// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩	
	
	
	Inclination_X_tan = (float)tan((double)(pVals->Angle_X*PI/180));
	Inclination_Y_tan = (float)tan((double)(pVals->Angle_Y*PI/180));
	pVals->Inclination_X = Inclination_X_tan*1000;	// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	pVals->Inclination_Y = Inclination_Y_tan*1000;	// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	
	pVals->Inclination=(float)sqrt(Inclination_X_tan*Inclination_X_tan + Inclination_Y_tan*Inclination_Y_tan)*1000;	// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
	
}

void SensorMonitoringDataUp(uint8_t flag)				 //������ݱ����ϴ�����۽ڵ�
{
	I1_TowerTilted_StructDef pVals;
	GetTowerTiltedVals(&pVals);
	TowerTilted.Inclination.Val_f = pVals.Inclination;				// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
	TowerTilted.Inclination_X.Val_f =	pVals.Inclination_X;		// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	TowerTilted.Inclination_Y.Val_f =	pVals.Inclination_Y;		// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	TowerTilted.Angle_X.Val_f =	pVals.Angle_X;								// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	TowerTilted.Angle_Y.Val_f =	pVals.Angle_Y;								// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	APP_PRINTF("\r\n��������б�����������б�\r\n");
//	APP_PRINTF("��һ����������б�ȡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.1f\r\n",TowerTilted.Inclination.SensorType,TowerTilted.Inclination.LengthFlag,TowerTilted.Inclination.Val_f);
//	APP_PRINTF("�ڶ���������˳����б�ȡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.1f\r\n",TowerTilted.Inclination_X.SensorType,TowerTilted.Inclination_X.LengthFlag,TowerTilted.Inclination_X.Val_f);
//	APP_PRINTF("������������������б�ȡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.1f\r\n",TowerTilted.Inclination_Y.SensorType,TowerTilted.Inclination_Y.LengthFlag,TowerTilted.Inclination_Y.Val_f);
//	APP_PRINTF("���ĸ�������˳����б�ǡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.2f\r\n",TowerTilted.Angle_X.SensorType,TowerTilted.Angle_X.LengthFlag,TowerTilted.Angle_X.Val_f);
//	APP_PRINTF("�����������������б�ǡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  Data:%.2f\r\n",TowerTilted.Angle_Y.SensorType,TowerTilted.Angle_Y.LengthFlag,TowerTilted.Angle_Y.Val_f);
	APP_PRINTF("����б�ȡ���%.1f\r\n",TowerTilted.Inclination.Val_f);
	APP_PRINTF("��˳����б�ȡ���%.1f\r\n",TowerTilted.Inclination_X.Val_f);
	APP_PRINTF("��������б�ȡ���%.1f\r\n",TowerTilted.Inclination_Y.Val_f);
	APP_PRINTF("��˳����б�ǡ���%.2f\r\n",TowerTilted.Angle_X.Val_f);
	APP_PRINTF("��������б�ǡ���%.2f\r\n",TowerTilted.Angle_Y.Val_f);
	if(flag)
		//���������ϴ���5������������Ƭ��������ݱ��ģ����������б��б���
		MsgFrameConstructAndSend(5,0,MonitoringData_MSG_TYPE,(uint8_t*)&TowerTilted,sizeof(TowerTilted_Def));
}

void SensorStatusUp(uint8_t flag)				 //������״̬���ϴ�����۽ڵ�
{
	SensorStatus.BatVoltage.BatVoltage_f = Read_Bat_Voltage();		//��ص�ѹ
	SensorStatus.BatCapacity.BatCapacity_u16 = (uint16_t)((SensorStatus.BatVoltage.BatVoltage_f-3.6)/0.6*100);	//���ʣ�����=(V-3.6)/(4.2-3.6)*100;
	if(SensorStatus.BatCapacity.BatCapacity_u16>100)			SensorStatus.BatCapacity.BatCapacity_u16 = 100;
	else if(SensorStatus.BatCapacity.BatCapacity_u16<1)	SensorStatus.BatCapacity.BatCapacity_u16 = 1;
	SensorStatus.Sensor.Status_u8 = 1;		//�������Լ�״̬
	SensorStatus.NetWork.Status_u8 = LoRa.Status;		//��������״̬
	SensorStatus.Power.Status_u8 = 1;			//��Դ״̬
	SensorStatus.SamplingPeriod.SamplingPeriod_u16 = SystemGlobalVar.SamplingPeriod;	//���ݲɼ����� 000 100010 2 �޷�������
	SensorStatus.RSSI.RSSI_f = 0 - (float)(256-LoRa_RSSI_Read());	//�ź�ǿ��
	SensorStatus.Temp.Val_f = ADXL355_Read_Temp();
	
//	APP_PRINTF("\r\n��������״̬�����б�\r\n");
//	APP_PRINTF("��һ�����������ʣ���������");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.BatCapacity.SensorType,SensorStatus.BatCapacity.LengthFlag,SensorStatus.BatCapacity.length,SensorStatus.BatCapacity.BatCapacity_u16);
//	APP_PRINTF("�ڶ�����������ص�ѹ����");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:NULL  Data:%.2f\r\n",SensorStatus.BatVoltage.SensorType,SensorStatus.BatVoltage.LengthFlag,SensorStatus.BatVoltage.BatVoltage_f);
//	APP_PRINTF("�������������������Լ�״̬����");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.Sensor.SensorType,SensorStatus.Sensor.LengthFlag,SensorStatus.Sensor.length,SensorStatus.Sensor.Status_u8);
//	APP_PRINTF("���ĸ���������������״̬����");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.NetWork.SensorType,SensorStatus.NetWork.LengthFlag,SensorStatus.NetWork.length,SensorStatus.NetWork.Status_u8);
//	APP_PRINTF("�������������Դ״̬����");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.Power.SensorType,SensorStatus.Power.LengthFlag,SensorStatus.Power.length,SensorStatus.Power.Status_u8);
//	APP_PRINTF("���������������ݲɼ����ڡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:%d  Data:%d\r\n",SensorStatus.SamplingPeriod.SensorType,SensorStatus.SamplingPeriod.LengthFlag,SensorStatus.SamplingPeriod.length,SensorStatus.SamplingPeriod.SamplingPeriod_u16);
//	APP_PRINTF("���߸��������ź�ǿ�ȡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:NULL  Data:%.2f\r\n",SensorStatus.RSSI.SensorType,SensorStatus.RSSI.LengthFlag,SensorStatus.RSSI.RSSI_f);
//	APP_PRINTF("�ڰ˸��������¶ȡ���");
//	APP_PRINTF("ParameterType:%x  LengthFlag:%d  length:NULL  Data:%.2f\r\n\r\n",SensorStatus.Temp.SensorType,SensorStatus.Temp.LengthFlag,SensorStatus.Temp.Val_f);
	APP_PRINTF("\r\n��������״̬�����б�\r\n");
	APP_PRINTF("�����ʣ���������%d\r\n",SensorStatus.BatCapacity.BatCapacity_u16);
	APP_PRINTF("����ص�ѹ����%.2f\r\n",SensorStatus.BatVoltage.BatVoltage_f);
	APP_PRINTF("���������Լ�״̬����%d\r\n",SensorStatus.Sensor.Status_u8);
	APP_PRINTF("��LoRa����״̬����%d\r\n",SensorStatus.NetWork.Status_u8);
	APP_PRINTF("����Դ״̬����%d\r\n",SensorStatus.Power.Status_u8);
	APP_PRINTF("�����ݲɼ�����(��)����%d\r\n",SensorStatus.SamplingPeriod.SamplingPeriod_u16);
	APP_PRINTF("���ź�ǿ�ȡ���%.2f\r\n",SensorStatus.RSSI.RSSI_f);
	APP_PRINTF("���¶ȡ���%.2f\r\n\r\n",SensorStatus.Temp.Val_f);
	if(flag)
		//״̬���ϴ���7������������Ƭ��������ݱ��ģ�״̬�����б��б���
		MsgFrameConstructAndSend(8,0,MonitoringData_MSG_TYPE,(uint8_t*)&SensorStatus,sizeof(Status_Def));
}

/*
*��ͨ�ò������á�
	����
		pbuf			:				//����ָ��
		ParaLen		:			  //��������
	���أ�
		��
*/
void SensorStatusSet(uint8_t *pbuf,uint8_t ParaLen)
{
	ParaLen = ParaLen;
	struct SamplingPeriod_Status_Def *pSamplingPeriod_Status;
	pSamplingPeriod_Status = (struct SamplingPeriod_Status_Def *)pbuf;
	if(pSamplingPeriod_Status->SensorType == SensorStatus.SamplingPeriod.SensorType)  //��������
	{
		SystemGlobalVar.SamplingPeriod = pSamplingPeriod_Status->SamplingPeriod_u16;	//���ݲɼ�����
		APP_LOG("�����������ã�%d\r\n\r\n",SystemGlobalVar.SamplingPeriod);
		//ͨ�ò����ϴ������8������������Ƭ��������Ӧ���ģ����������б��б���
		MsgFrameConstructAndSend(1,0,ControlRes_MSG_TYPE,pbuf,sizeof(struct SamplingPeriod_Status_Def));
		__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//���RTC WAKE UP�ı�־
		HAL_RTCEx_SetWakeUpTimer(&hrtc, SystemGlobalVar.SamplingPeriod-60, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);  //��ǰ1���ӻ���
		SystemConfigSave();  //��������
	}
}

//������ݱ����봫����״̬���ϴ�����۽ڵ�
void SensorMonitoringDataAndStatusUp(void)				 
{
	uint8_t i,index=0;
	uint8_t Buf[128],*p;
	SensorMonitoringDataUp(0);				 //������ݱ���
	SensorStatusUp(0);			 						//������״̬���ϴ�����۽ڵ�
	p = (uint8_t*)&TowerTilted;
	for(i=0;i<sizeof(TowerTilted_Def);i++) Buf[index++] = *p++;
	p = (uint8_t*)&SensorStatus;
	for(i=0;i<sizeof(Status_Def);i++) Buf[index++] = *p++;
	//���������ϴ���5+7������������Ƭ��������ݱ��ģ����������б��б���
	MsgFrameConstructAndSend(13,0,MonitoringData_MSG_TYPE,Buf,index);
}

/*
*������������ֵ��ѯ��
	����
		pbuf			:				//����ָ��
		ParaLen		:			  //��������
	���أ�
		��
*/
void SensorAlarmLimitDataRequest(uint8_t *pbuf,uint8_t ParaLen)				 //����������ֵ��ѯ
{
	uint8_t i,j;
	struct AlarmLimitPara4Byte_2f_Def *pAlarmTab;
	struct AlarmLimitPara4Byte_2f_Def	*pAlarmLimit; //������б����������ֵ
	pAlarmLimit = (struct AlarmLimitPara4Byte_2f_Def *)&SystemGlobalVar.AlarmLimit;
	pAlarmTab = (struct AlarmLimitPara4Byte_2f_Def *)pbuf;
	if(ParaLen==0xf)  //���в���
		ParaLen = 5;		//���5������
	for(i=0;i<ParaLen;i++)
	{
		for(j=0;j<5;j++)
		{
			if(pAlarmTab[i].SensorType == pAlarmLimit[j].SensorType)
			{
				pAlarmTab[i].AlarmUpperLimit = pAlarmLimit[j].AlarmUpperLimit;
				pAlarmTab[i].AlarmLowerLimit = pAlarmLimit[j].AlarmLowerLimit;
				APP_LOG("����������ֵ��ѯ��ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n\r\n",pAlarmLimit[j].SensorType,pAlarmLimit[j].LengthFlag,pAlarmLimit[j].AlarmUpperLimit,pAlarmLimit[j].AlarmLowerLimit);
				break;
			}	
		}
	}
	
	APP_LOG("������������ֵ�����б�������\r\n");
	APP_PRINTF("��һ����������б�ȡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.SensorType,SystemGlobalVar.AlarmLimit.Inclination.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
	APP_PRINTF("�ڶ���������˳����б�ȡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.SensorType,SystemGlobalVar.AlarmLimit.Inclination_X.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
	APP_PRINTF("������������������б�ȡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.SensorType,SystemGlobalVar.AlarmLimit.Inclination_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
	APP_PRINTF("���ĸ�������˳����б�ǡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.SensorType,SystemGlobalVar.AlarmLimit.Angle_X.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
	APP_PRINTF("�����������������б�ǡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.SensorType,SystemGlobalVar.AlarmLimit.Angle_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
	//����������ֵ�ϴ���5������������Ƭ���澯������Ӧ���ģ����������б��б���
	MsgFrameConstructAndSend(ParaLen,0,AlarmRes_MSG_TYPE,pbuf,ParaLen*sizeof(struct AlarmLimitPara4Byte_2f_Def));
}

/*
*������������ֵ���á�
	����
		pbuf			:				//����ָ��
		ParaLen		:			  //��������
	���أ�
		��
*/
void SensorAlarmLimitDataSet(uint8_t *pbuf,uint8_t ParaLen)				 //����������ֵ����
{
	uint8_t i,j;
	struct AlarmLimitPara4Byte_2f_Def *pAlarmTab;
	struct AlarmLimitPara4Byte_2f_Def	*pAlarmLimit; //������б����������ֵ
	pAlarmLimit = (struct AlarmLimitPara4Byte_2f_Def *)&SystemGlobalVar.AlarmLimit;
	pAlarmTab = (struct AlarmLimitPara4Byte_2f_Def *)pbuf;
	if(ParaLen==0xf)  //���в���
		ParaLen = 5;		//���5������
	for(i=0;i<ParaLen;i++)
	{
		for(j=0;j<5;j++)
		{
			if(pAlarmTab[i].SensorType == pAlarmLimit[j].SensorType)
			{
				pAlarmLimit[j].AlarmUpperLimit = pAlarmTab[i].AlarmUpperLimit;
				pAlarmLimit[j].AlarmLowerLimit = pAlarmTab[i].AlarmLowerLimit;
				APP_LOG("����������ֵ���ã�ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n\r\n",pAlarmLimit[j].SensorType,pAlarmLimit[j].LengthFlag,pAlarmLimit[j].AlarmUpperLimit,pAlarmLimit[j].AlarmLowerLimit);
				break;
			}	
		}
	}
	
	APP_LOG("������������ֵ�����б�������\r\n");
	APP_PRINTF("��һ����������б�ȡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.SensorType,SystemGlobalVar.AlarmLimit.Inclination.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
	APP_PRINTF("�ڶ���������˳����б�ȡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.SensorType,SystemGlobalVar.AlarmLimit.Inclination_X.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
	APP_PRINTF("������������������б�ȡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.SensorType,SystemGlobalVar.AlarmLimit.Inclination_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
	APP_PRINTF("���ĸ�������˳����б�ǡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.SensorType,SystemGlobalVar.AlarmLimit.Angle_X.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
	APP_PRINTF("�����������������б�ǡ���");
	APP_PRINTF("ParameterType:%x  LengthFlag:%d  ����ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.SensorType,SystemGlobalVar.AlarmLimit.Angle_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
	//����������ֵ�ϴ���5������������Ƭ���澯������Ӧ���ģ����������б��б���
	MsgFrameConstructAndSend(ParaLen,0,AlarmRes_MSG_TYPE,pbuf,ParaLen*sizeof(struct AlarmLimitPara4Byte_2f_Def));
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************/
void Shell_SensorMonitoringDataUp(void)				 //������ݱ����ϴ�����۽ڵ�
{
	I1_TowerTilted_StructDef pVals;
	GetTowerTiltedVals(&pVals);
	TowerTilted.Inclination.Val_f = pVals.Inclination;				// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
	TowerTilted.Inclination_X.Val_f =	pVals.Inclination_X;		// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	TowerTilted.Inclination_Y.Val_f =	pVals.Inclination_Y;		// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	TowerTilted.Angle_X.Val_f =	pVals.Angle_X;								// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	TowerTilted.Angle_Y.Val_f =	pVals.Angle_Y;								// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	printf("��������б�����������б�\r\n");
	printf("����б�ȡ�:%.1f\r\n",TowerTilted.Inclination.Val_f);
	printf("��˳����б�ȡ�:%.1f\r\n",TowerTilted.Inclination_X.Val_f);
	printf("��������б�ȡ�:%.1f\r\n",TowerTilted.Inclination_Y.Val_f);
	printf("��˳����б�ǡ�:%.2f\r\n",TowerTilted.Angle_X.Val_f);
	printf("��������б�ǡ�:%.2f\r\n\r\n",TowerTilted.Angle_Y.Val_f);
}

void Shell_SensorStatusUp(void)				 //������״̬���ϴ�����۽ڵ�
{
	SensorStatus.BatVoltage.BatVoltage_f = Read_Bat_Voltage();		//��ص�ѹ
	SensorStatus.BatCapacity.BatCapacity_u16 = (uint16_t)((SensorStatus.BatVoltage.BatVoltage_f-3.6)/0.6*100);	//���ʣ�����=(V-3.6)/(4.2-3.6)*100;
	if(SensorStatus.BatCapacity.BatCapacity_u16>100)			SensorStatus.BatCapacity.BatCapacity_u16 = 100;
	else if(SensorStatus.BatCapacity.BatCapacity_u16<1)	SensorStatus.BatCapacity.BatCapacity_u16 = 1;
	SensorStatus.Sensor.Status_u8 = 1;		//�������Լ�״̬
	SensorStatus.NetWork.Status_u8 = 1;		//��������״̬
	SensorStatus.Power.Status_u8 = 1;		//��Դ״̬
	SensorStatus.SamplingPeriod.SamplingPeriod_u16 = SystemGlobalVar.SamplingPeriod;	//���ݲɼ����� 000 100010 2 �޷�������
	SensorStatus.RSSI.RSSI_f = LoRa_RSSI_Read();		//�ź�ǿ��
	SensorStatus.Temp.Val_f = ADXL355_Read_Temp();
	
	printf("��������״̬�����б�\r\n");
	printf("�����ʣ�������:%d%%\r\n",SensorStatus.BatCapacity.BatCapacity_u16);
	printf("����ص�ѹ��:%.2f\r\n",SensorStatus.BatVoltage.BatVoltage_f);
	printf("���������Լ�״̬��:%d\r\n",SensorStatus.Sensor.Status_u8);
	printf("����������״̬��:%d\r\n",SensorStatus.NetWork.Status_u8);
	printf("����Դ״̬��:%d\r\n",SensorStatus.Power.Status_u8);
	printf("�����ݲɼ����ڡ�:%d\r\n",SensorStatus.SamplingPeriod.SamplingPeriod_u16);
	printf("���ź�ǿ�ȡ�:%.2f\r\n",SensorStatus.RSSI.RSSI_f);
	printf("���¶ȡ�:%.2f\r\n\r\n",SensorStatus.Temp.Val_f);
}

/*
*������������ֵ��ѯ��
	����
		pbuf			:				//����ָ��
		ParaLen		:			  //��������
	���أ�
		��
*/
void Shell_SensorAlarmLimitDataRequest(void)				 //����������ֵ��ѯ
{
	printf("������������ֵ�����б�������\r\n");
//	printf("1.����б�ȡ�   �� ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
//	printf("2.��˳����б�ȡ�������ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
//	printf("3.��������б�ȡ�������ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
//	printf("4.��˳����б�ǡ�������ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
//	printf("5.��������б�ǡ�������ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
	printf("1.��˳�ߡ�������б�ǡ�������ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
}

/*
*������������ֵ���á�
	����
		pbuf			:				//����ָ��
		ParaLen		:			  //��������
	���أ�
		��
*/
//void Shell_SensorAlarmLimitDataSet(uint8_t *pbuf,uint8_t ParaLen)				 //����������ֵ����
//{
//	uint8_t i,j;
//	struct AlarmLimitPara4Byte_2f_Def *pAlarmTab;
//	struct AlarmLimitPara4Byte_2f_Def	*pAlarmLimit; //������б����������ֵ
//	pAlarmLimit = (struct AlarmLimitPara4Byte_2f_Def *)&SystemGlobalVar.AlarmLimit;
//	pAlarmTab = (struct AlarmLimitPara4Byte_2f_Def *)pbuf;
//	if(ParaLen==0xf)  //���в���
//		ParaLen = 5;		//���5������
//	for(i=0;i<ParaLen;i++)
//	{
//		for(j=0;j<5;j++)
//		{
//			if(pAlarmTab[i].SensorType == pAlarmLimit[j].SensorType)
//			{
//				pAlarmLimit[j].AlarmUpperLimit = pAlarmTab[i].AlarmUpperLimit;
//				pAlarmLimit[j].AlarmLowerLimit = pAlarmTab[i].AlarmLowerLimit;
//				APP_LOG("����������ֵ���ã�ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n\r\n",pAlarmLimit[j].SensorType,pAlarmLimit[j].LengthFlag,pAlarmLimit[j].AlarmUpperLimit,pAlarmLimit[j].AlarmLowerLimit);
//				break;
//			}	
//		}
//	}
//	
//	printf("������������ֵ�����б�������\r\n");
//	printf("��һ����������б�ȡ���");
//	printf("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination.SensorType,SystemGlobalVar.AlarmLimit.Inclination.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination.AlarmLowerLimit);
//	printf("�ڶ���������˳����б�ȡ���");
//	printf("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_X.SensorType,SystemGlobalVar.AlarmLimit.Inclination_X.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_X.AlarmLowerLimit);
//	printf("������������������б�ȡ���");
//	printf("ParameterType:%x  LengthFlag:%d  ����ֵ:%.1f  ����ֵ:%.1f\r\n",SystemGlobalVar.AlarmLimit.Inclination_Y.SensorType,SystemGlobalVar.AlarmLimit.Inclination_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Inclination_Y.AlarmLowerLimit);
//	printf("���ĸ�������˳����б�ǡ���");
//	printf("ParameterType:%x  LengthFlag:%d  ����ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.SensorType,SystemGlobalVar.AlarmLimit.Angle_X.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
//	printf("�����������������б�ǡ���");
//	printf("ParameterType:%x  LengthFlag:%d  ����ֵ:%.2f  ����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_Y.SensorType,SystemGlobalVar.AlarmLimit.Angle_Y.LengthFlag,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmUpperLimit,SystemGlobalVar.AlarmLimit.Angle_Y.AlarmLowerLimit);
//	//����������ֵ�ϴ���5������������Ƭ���澯������Ӧ���ģ����������б��б���
//	MsgFrameConstructAndSend(ParaLen,0,AlarmRes_MSG_TYPE,pbuf,ParaLen*sizeof(struct AlarmLimitPara4Byte_2f_Def));
//}

