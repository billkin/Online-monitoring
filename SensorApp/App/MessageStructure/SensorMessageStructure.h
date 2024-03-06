#ifndef __SENSOR_MESSAGE_H__
#define __SENSOR_MESSAGE_H__

#include "main.h"
#include "GDW_I1_2015.h"

////��������ͨ�ŵ�ַ�����ֽڵ�ַ��0x00~0xFD ���ֽڵ�ַ��0x00~0xFF 
//#define MESSAGE_FRAME_TYPE	0	//ҵ���ŵ���Ϣ֡
//#define REQ_FRAME_TYPE	 		1	//�����ŵ�����֡
//#define RSP_FRAME_TYPE 			2	//�����ŵ�Ӧ��֡
//#define RSP_END_FRAME_TYPE 	3	//�����ŵ�Ӧ����֡
//#define BURST_FRAME_TYPE 		4	//ͻ������֡
//#define ACK_FRAME_TYPE 			5	//�����ŵ�ȷ��֡
////#define RFU ��������

//�ݷ���״̬�� ��0xFF�ɹ� ��0x00ʧ�ܡ�
#define STATUS_SUCCEED	0xFF
#define STATUS_FAIL			0x00

//�����������ͱ�ʶ RequestSetFlag 1���� 0�� ��ѯ�� 1�� ����
#define REQUEST_FLAG	0
#define SET_FLAG			1

#define CMD_CONTROL						0x80	    //��������
#define CMD_CONTROL_WAIT			0x40	    //�ȴ���������
#define CMD_TIMING_END				0x20	    //ʱ��ͬ�����
#define CMD_RETRY							0x10	    //ָ��/�����ط�

#define MonitoringData_MSG_TYPE			0	 //������ݱ���
#define MonitoringDataRes_MSG_TYPE	1	 //���������Ӧ����
#define Alarm_MSG_TYPE							2	 //�澯���ݱ���
#define AlarmRes_MSG_TYPE						3	 //�澯������Ӧ����
#define Control_MSG_TYPE						4	 //���Ʊ���
#define ControlRes_MSG_TYPE					5	 //������Ӧ����
#define FragmentedResponse_MSG_TYPE	6	 //��Ƭ����Ӧ����
#define Reserved_MSG_TYPE						7	 //Ԥ��

#define GeneralParaQuerySet_CTL 1		//������ͨ�ò�����ѯ/����
#define MonitorDataQuery_CTL		2 	//������������ݲ�ѯ����
#define AlarmPara_CTL						3 	//�������澯������ѯ/����
#define TimePara_CTL						4 	//������ʱ�������ѯ/����
#define ID_CTL									5 	//������ ID ��ѯ/����
#define Reset_CLT								6 	//��������λ����
#define Timing_CTL							7 	//����������Уʱ
//8-99 //Э����չԤ���ֶ�
//100-127 //�����Զ���Ԥ���ֶ�

#pragma pack (1) /*ָ����1�ֽڶ���*/

//ͨ��״̬�������ͱ�
struct Para4Byte_f_Def           //����ͷ��
{
	uint16_t LengthFlag:	2		;	//bit0~1	���ݳ����ֶ�ָʾλ
	uint16_t SensorType:	14	;	//bit2~15	�������� 000 11110
	float Val_f;								//4 ������
};
struct Para4Byte_u32_Def           //����ͷ��
{
	uint16_t LengthFlag:	2		;	//bit0~1	���ݳ����ֶ�ָʾλ
	uint16_t SensorType:	14	;	//bit2~15	�������� 000 11110
	uint32_t Val_u32;						//4 ������
};

//���ʣ�����
struct BatCapacity_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 ���ݳ����ֶ�ָʾλ, 1
	uint16_t SensorType:	14	;	//bit2~15	29 �������� 29
	uint8_t	length;							//2 ���ݳ���,�����ݳ����ֶ�ָʾλ������ ���ֶεĳ����������ݳ����ֶα�ʾλ�� x���Էֱ� NULL�� 1�� 2��3���ֽڡ�
	uint16_t BatCapacity_u16;		//���ݲɼ�����  2 �޷�������
};
//��ص�ѹ
struct BatVoltage_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 ���ݳ����ֶ�ָʾλ, 0
	uint16_t SensorType:	14	;	//bit2~15	30 �������� 30
	float BatVoltage_f;					//��ص�ѹ  4 ������
};

//�������Լ�״̬
struct Sensor_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 ���ݳ����ֶ�ָʾλ, 1
	uint16_t SensorType:	14	;	//bit2~15	31 �������� 31
	uint8_t	length;							//1 ���ݳ���,�����ݳ����ֶ�ָʾλ������ ���ֶεĳ����������ݳ����ֶα�ʾλ�� x���Էֱ� NULL�� 1�� 2��3���ֽڡ�
	uint8_t Status_u8;		//�������Լ�״̬ 1�� ������ 2���쳣��
};

//��������״̬
struct NetWork_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 ���ݳ����ֶ�ָʾλ,1
	uint16_t SensorType:	14	;	//bit2~15	32 ��������
	uint8_t	length;							//1 ���ݳ���,�����ݳ����ֶ�ָʾλ������ ���ֶεĳ����������ݳ����ֶα�ʾλ�� x���Էֱ� NULL�� 1�� 2��3���ֽڡ�
	uint8_t Status_u8;					//��������״̬ 1�� ������ 2���쳣��
};

//��Դ״̬
struct Power_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 ���ݳ����ֶ�ָʾλ,1
	uint16_t SensorType:	14	;	//bit2~15	33 �������� 33
	uint8_t	length;							//1 ���ݳ���,�����ݳ����ֶ�ָʾλ������ ���ֶεĳ����������ݳ����ֶα�ʾλ�� x���Էֱ� NULL�� 1�� 2��3���ֽڡ�
	uint8_t Status_u8;					//��Դ״̬ 1�� ������ 2���쳣��
};

//���ݲɼ����� 000 100010 2 �޷�������
struct SamplingPeriod_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1	1 ���ݳ����ֶ�ָʾλ,1
	uint16_t SensorType:	14	;	//bit2~15	34 ��������
	uint8_t	length;							//2 ���ݳ���,�����ݳ����ֶ�ָʾλ������ ���ֶεĳ����������ݳ����ֶα�ʾλ�� x���Էֱ� NULL�� 1�� 2��3���ֽڡ�
	uint16_t SamplingPeriod_u16;//���ݲɼ�����  2 �޷�������
};

//�ź�ǿ��
struct RSSI_Status_Def
{
	uint16_t LengthFlag:	2		;	//bit0~1 	���ݳ����ֶ�ָʾλ,0
	uint16_t SensorType:	14	;	//bit2~15	35 ��������
	float RSSI_f;					//�ź�ǿ�� 4 ������
};

/**
**״̬����
**/
typedef struct
{
	//���ʣ�����
	struct BatCapacity_Status_Def BatCapacity;
	//��ص�ѹ
	struct BatVoltage_Status_Def	BatVoltage;
	//�������Լ�״̬
	struct Sensor_Status_Def			Sensor;
	//��������״̬
	struct NetWork_Status_Def			NetWork;
	//��Դ״̬
	struct Power_Status_Def				Power;
	//���ݲɼ����� 000 100010 2 �޷�������
	struct SamplingPeriod_Status_Def	SamplingPeriod;
	//�ź�ǿ��
	struct RSSI_Status_Def			RSSI;
	//�¶�
	struct Para4Byte_f_Def  Temp;
}Status_Def;   										//״̬����


/**
**���ҵ��״̬�������ͱ�
**/
//������б
typedef struct
{
	struct Para4Byte_f_Def Inclination;										// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
	struct Para4Byte_f_Def Inclination_X;									// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	struct Para4Byte_f_Def Inclination_Y;									// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	struct Para4Byte_f_Def Angle_X;												// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	struct Para4Byte_f_Def Angle_Y;												// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
}TowerTilted_Def;   										//������б����


struct AlarmLimitPara4Byte_2f_Def      //����ͷ��
{
	uint16_t LengthFlag:	2		;	//bit0~1 	���ݳ����ֶ�ָʾλ
	uint16_t SensorType:	14	;	//bit2~15	�������� 000 11110
	float AlarmUpperLimit;			//4 ������
	float AlarmLowerLimit;
};

typedef struct
{
	struct AlarmLimitPara4Byte_2f_Def Inclination;			// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
	struct AlarmLimitPara4Byte_2f_Def Inclination_X;		// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	struct AlarmLimitPara4Byte_2f_Def Inclination_Y;		// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	struct AlarmLimitPara4Byte_2f_Def Angle_X;					// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	struct AlarmLimitPara4Byte_2f_Def Angle_Y;					// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
}TT_AlarmLimit_Def;   										//������б����������ֵ


//������ͨ�ò�����ѯ/���ñ�����Ӧ��ʽ�淶
struct GeneralParaQuerySetRes_Def
{
	uint8_t RequestSetFlag:	1;					//bit0	�����������ͱ�ʶ,0�� ��ѯ�� 1�� ����
	uint8_t CtrlType:	7;								//bit1~7	���Ʊ�������
	uint8_t Data[];											//��Ӧ����
};

//������ʱ���ѯ/���ñ��ĸ�ʽ
struct TimeQuerySetRes_Def
{
	uint8_t RequestSetFlag:	1;					//bit0	�����������ͱ�ʶ,0�� ��ѯ�� 1�� ����
	uint8_t CtrlType:	7;								//bit1~7	���Ʊ�������
	uint32_t Timestamp;									//ʱ���
	uint16_t SamplingPeriod;						//Ϊ�����ʱͬ�����Ѷ���ӵ��ֶ�/*��������*/����׼Э��û������ֶΣ�
};

struct ManufacturerID_Def      	//�����ն� ID
{
	uint16_t 	ManufacturerID;			//���̴���
	uint32_t	SerialNumber :	21;	//0~20bit  ��Ʒ���к�
	uint32_t	VersionNumber:	6	;	//21~26bit �汾��
	uint32_t	VersionLetter:	5	;	//27~31bit �汾��ǩ
};

//������ ID ��ѯ/���ñ��ĸ�ʽ
struct ID_QuerySetRes_Def
{
	uint8_t RequestSetFlag:	1;								//bit0    �����������ͱ�ʶ,0�� ��ѯ�� 1�� ����
	uint8_t CtrlType:	7;											//bit 1~7 ���Ʊ�������
	struct ManufacturerID_Def NewSensorID;		//�����ն� ID									
};

struct MessageStructure_Def
{
	struct ManufacturerID_Def SensorID;		//�����ն�ID
	uint8_t	PacketType:	3;								//bit0~2 	��������
	uint8_t	FragInd		:	1;								//bit3   	��Ƭָʾ
	uint8_t	DataLen		:	4;								//bit4~7	��������
	uint8_t Data[];												//��������+crc16
};

struct FragInd_PDU_Def
{
	/*****��Ƭͷ*********************************************/
	uint8_t SSEQ		:	6;	//bit0~5 	ҵ�����ݵ�Ԫ���,ȡֵ��1��ʼ�� ����ȡֵ��
	uint8_t FLAG		:	2;	//bit6~7	��Ƭ����
	uint8_t PSEQ		:	7;	//bit0~6 	Э�����ݵ�Ԫ���
	uint8_t Priority:	1;	//bit7 		Э�����ݵ�Ԫ���ȼ�,0��ʾ�ͣ� 1��ʾ��
	/*****��Ƭ��������*********************************************/
	uint16_t SIZE;				//�������ݳ���,�������ݣ���Ƭ�� �ĳ��ȣ��ֽ����� ��
	uint8_t Data[];				//��Ƭ���PDU��������
};

#pragma pack ()

extern const TT_AlarmLimit_Def AlarmLimit_Init; //������б����������ֵ

void GetTowerTiltedVals(I1_TowerTilted_StructDef *pVals);  //��ȡ������бֵ
void SensorMonitoringDataUp(uint8_t flag);				 //������ݱ����ϴ�����۽ڵ�
void SensorStatusUp(uint8_t flag);								 //������״̬���ϴ�����۽ڵ�
void SensorStatusSet(uint8_t *pbuf,uint8_t ParaLen);
void SensorMonitoringDataAndStatusUp(void);					//������ݱ����봫����״̬���ϴ�����۽ڵ�

void SensorAlarmLimitDataRequest(uint8_t *pbuf,uint8_t ParaLen);		//����������ֵ��ѯ
void SensorAlarmLimitDataSet(uint8_t *pbuf,uint8_t ParaLen);				 //����������ֵ����

void SensorMessagePro(uint8_t *pbuf,uint16_t len);

void SensorControl(uint8_t CtrlType,uint8_t RequestSetFlag,uint8_t *paralist,uint16_t len); //���Ʊ���

/*********************************************************************************************/
/**     �ն˴�ӡ����                                                                         **/
/*********************************************************************************************/
void Shell_SensorMonitoringDataUp(void);				 //������ݱ����ϴ�����۽ڵ�
void Shell_SensorStatusUp(void);				 					//������״̬���ϴ�����۽ڵ�
void Shell_SensorAlarmLimitDataRequest(void);			//����������ֵ��ѯ

#endif /* __SENSOR_MESSAGE_H__ */


