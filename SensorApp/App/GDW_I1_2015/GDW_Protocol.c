#include "GDW_Protocol.h"
#include "RTU_CRC.h"
#include "stdlib.h"

Frame_StructDef *pGDW_Frame;
unsigned char Frame_No=0;
//unsigned char CMD_ID[17];
#if (USE_Meteorological)
	Meteorological_StructDef Meteorological;     //������
#endif
#if (USE_TowerTilted)
	TowerTilted_StructDef TowerTilted;   //������б����
#endif
NetworkAdapterQuerySetting_StructDef  *pNetworkAdapterQuerySetting;     //������������ѯ/�������ݱ���

//�������ݱ�����λ��to���װ�ã�
void ControlDataItem(unsigned char Packet_Type,unsigned char *pdata)
{
	switch(Packet_Type)
	{
		case 0xA1:								//״̬���װ��������������ѯ/����
			pNetworkAdapterQuerySetting = (NetworkAdapterQuerySetting_StructDef*)pdata;
			//�������ã�����Ӧ����ϵͳ
			break;
		case 0xA2:								//�ϼ��豸����״̬���װ����ʷ����
			
			break;
		case 0xA3:								//״̬���װ�ò������ڲ�ѯ/����
			break;
		case 0xA4:								//״̬���װ��ָ����λ������Ϣ��ѯ/����
			break;
		case 0xA5:								//״̬���װ�� ID ��ѯ/����
			break;
		case 0xA6:								//״̬���װ�ø�λ
			break;
		case 0xA7:								//״̬���װ��ģ�Ͳ���������Ϣ��ѯ/����
			break;
		case 0xA8:								//�������ݱ�Ԥ���ֶ�
		case 0xAF:	
			break;
		default:break;
	}
}
/*************************************************************************************
���� ������һ�������Ƿ����һ֡���ݱ���
����
		pdata		������ָ��
		len			�����ݳ���
����ֵ��֡����ָ�룬��ָ�벻Ϊ��ʱ��frameָ������ݱ�����Ч
**************************************************************************************/
Frame_StructDef *DataAnalysis(unsigned char *pdata,unsigned short len)
{
	unsigned char *p=NULL;
	unsigned short i,f_len;
	unsigned short  crc;
	for(i=0;i<len-27;i++)   //һ֡�������ٰ���28����
	{
		if(pdata[i]==0xA5&&pdata[i+1]==0x5A)  //֡ͬ��ͷ
		{
			p=&pdata[i];
			break;
		}
	}
	if(p==NULL) return NULL;
	f_len = (unsigned short)(p[3]<<8) | p[2];
	if(p[f_len+26]!=0x96)  return NULL;  										//֡β����0x96����Ϊ����֡
        crc = RTU_CRC(&p[2],f_len+22);   												//CRCУ��
	if(crc !=((p[f_len+26]<<25)|p[f_len+24])) return NULL; 	//CRCУ�����
	return (Frame_StructDef*)p;      												//֡������Ч
}

/*************************************************************************************
���� ����֡���ݽ��н���
����
		frame					��ָ����Ч֡����ָ��
**************************************************************************************/
void FrameAnalysis(Frame_StructDef *frame)
{
	//�Ƚ� pGDW_Frame->CMD_ID
	switch(frame->Frame_Type)  //֡���͡��ο����� C8-1 ��غ���
	{
		case 0x01:			// ������ݱ������װ��to��λ����
			break;
		case 0x02:			//������Ӧ������λ��to���װ�ã�
			if(frame->Payload[0]==0x00)   //���ݷ���״̬�� ��0xFF �ɹ�  ��0x00 ʧ��
			{
					//���ݷ��͸�ʧ�ܣ���Ҫ�ط�
			}
			break;
		case 0x03:			//�������ݱ�����λ��to���װ�ã�
			ControlDataItem(frame->Packet_Type,frame->Payload);
			break;
		case 0x04:			//������Ӧ�������װ��to��λ����
			break;
		case 0x05:			//ͼ�����ݱ������װ��to��λ����
			break;
		case 0x06:			//ͼ��������Ӧ���� ��λ��to���װ�ã�����δʹ�ã�
			break;
		case 0x07:			//ͼ����Ʊ�����λ��to���װ�ã�
			break;
		case 0x08:			//ͼ�������Ӧ�������װ��to��λ����
			break;
		case 0x09:			//����״̬�������װ��to��λ����
			break;
		case 0x0A:			//����״̬��Ӧ������λ��to���װ�ã�
			break;
		default:break;
	}
}

//�������
Frame_StructDef *DataPackingIntoFrames(unsigned char Frame_Type,unsigned char Packet_Type,unsigned char *pdata,unsigned short len)
{
	unsigned char *p;
        unsigned short i,crc;
	Frame_StructDef *pframe;
	pframe = (Frame_StructDef*)malloc(sizeof(Frame_StructDef)+len+3*sizeof(unsigned char));//�䳤�����ڴ�: ��������(�䳤) + У��λ(2λ) + ����β(1λ) ���1420
	pframe->Sync = 0x5AA5;   				//����ͷ
	pframe->Packet_Length = len;   	//���ĳ���
	for(i=0;i<17;i++)										//17 ״̬���װ�� ID��17 λ���룩
	{
		//pframe->CMD_ID[i] = CMD_ID[i];
	}
	pframe->Frame_Type = Frame_Type;			//֡����
	pframe->Packet_Type = Packet_Type;		// ��������	
	Frame_No++;
	pframe->Frame_No = Frame_No;					//֡���к�
	p = pdata;     													//������
	for(i=0;i<pframe->Packet_Length;i++) //���Ʊ�������
	{
		pframe->Payload[i] = *p++;
	}
	p = (unsigned char*)&pframe->Packet_Length;  //����crc
        crc = RTU_CRC(p,pframe->Packet_Length+22);
        pframe->Payload[pframe->Packet_Length] = crc&0xff;
        pframe->Payload[pframe->Packet_Length+1] = crc>>8;
	pframe->Payload[pframe->Packet_Length+2] = 0x96;  //����β��0x96
	return pframe;  //
	//free(pframe);   //�ͷ��ڴ�
}


//������ݱ�
//void MonitoringDatagram(unsigned char Frame_Type,unsigned char Packet_Type)
//{
//	unsigned char *p, *p2;
//	unsigned short i,crc;
//	
//	switch(Packet_Type)
//	{
//		case 0x01:				//���󻷾������ݱ�
//			#if (USE_Meteorological)
//				DataPacking(Frame_Type,Packet_Type,(unsigned char*)&Meteorological,sizeof(Meteorological_StructDef));
//			#endif
//			break;
//		case 0x02:				//������б���ݱ�
//			#if (USE_TowerTilted)
//				DataPacking(Frame_Type,Packet_Type,(unsigned char*)&TowerTilted,sizeof(TowerTilted_StructDef));   //������б����
//			#endif
//			break;
//		case 0x03:				//������΢�������������ݱ�
//			break;
//		case 0x04:				//������΢���񶯲����ź����ݱ�
//			break;
//		case 0x05:				//���߻������ݱ�
//			break;
//		case 0x06:				//�����¶����ݱ�
//			break;
//		case 0x07:				//���������������������ݱ�
//			break;
//		case 0x08:				//���߷�ƫ���ݱ�
//			break;
//		case 0x09:				//�������趯���������ݱ�
//			break;
//		case 0x0A:				//�������趯�켣���ݱ�
//			break;
//		case 0x0B:				//�ֳ��ۻ�����ݱ�
//			break;
//		case 0x0C:				//���������ݱ�Ԥ���ֶ�
//		case 0x0D:
//		case 0x0E:
//		case 0x0F:
//			break;
//		default:break;
//	}
//	
//}

////�����װ��to��λ����
//void CMD_to_CMA(unsigned char Frame_Type,unsigned char Packet_Type)
//{
//	switch(Frame_Type) //֡���͡��ο����� C8-1 ��غ���
//	{
//		case 0x01:				// ������ݱ������װ��to��λ����
//			switch(Packet_Type)
//			{
//				case 0x01:
//					break;
//			}
//			break;
//		case 0x04:				//������Ӧ�������װ��to��λ����
//			break;
//		case 0x05:				//ͼ�����ݱ������װ��to��λ����
//			break;
//		case 0x08:				//ͼ�������Ӧ�������װ��to��λ����
//			break;
//		case 0x09:				//����״̬�������װ��to��λ����
//			break;
//		default:break;
//	}
//}

//�����װ��to��λ����
unsigned char CMD_to_CMA(unsigned char Frame_Type,unsigned char Packet_Type,unsigned char *pdata,unsigned short len)
{
	Frame_StructDef *frame;
//	unsigned short frame_len;
	unsigned char *p;
	frame = DataPackingIntoFrames(Frame_Type,Packet_Type,pdata,len);
	p = (unsigned char*)frame;
	//SendData(p,frame_len);   //��������	
	return 0;
}

void GDW_Protocol_test(unsigned short Packet_Length)
{

}

