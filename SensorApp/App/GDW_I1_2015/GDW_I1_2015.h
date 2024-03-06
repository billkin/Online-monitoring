#ifndef _GDW_I1_2015_H_
#define _GDW_I1_2015_H_

typedef struct
{
	unsigned short Sync;							// 2 ����ͷ�� 5AA5
	unsigned short Packet_Length;			// 2 ���ĳ���
	unsigned char CMD_ID[17];					// 17 ״̬���װ�� ID��17 λ���룩
	unsigned char Frame_Type;					// 1 ֡���͡��ο����� C8-1 ��غ���
	unsigned char Packet_Type;				// 1 �������͡��ο����� C8-2 ��غ���
	unsigned char Frame_No;						// 1 ֡���кţ��޷���������
	unsigned char Payload[];				//��������(�䳤) + У��λ(2λ) + ����β(0x96) ���1420
}Frame_StructDef;



typedef struct
{
	unsigned char Component_ID[17];							// 17 ������豸 ID��17 λ���룩
	unsigned int Time_Stamp;										// 4 �ɼ�ʱ��
	float Average_WindSpeed_10min;							// 4 10 ����ƽ�����٣�����������ȷ��С����� 1 λ����λ��m/s��
	unsigned short Average_WindDirection_10min;	// 2 10 ����ƽ�������޷�����������λ���㣩
	float Max_WindSpeed;												// 4 �����٣�����������ȷ��С����� 1 λ����λ�� m/s��
	float Extreme_WindSpeed;										// 4 ������٣�����������ȷ��С����� 1 λ����λ�� m/s��
	float Standard_WindSpeed;										// 4 ��׼���٣����ö���������ת������׼״̬�ķ��٣�����������ȷ��С����� 1 λ����λ�� m/s��
	float Air_Temperature;											// 4 ���£�����������ȷ��С����� 1 λ����λ���棩
	unsigned short Humidity;										// 2 ʪ�ȣ��޷�����������λ�� %RH��
	float Air_Pressure;													// 4 ��ѹ������������ȷ��С����� 1 λ����λ�� hPa��
	float Precipitation;												// 4 ������������������ȷ��С����� 1 λ����λ�� mm��
	float Precipitation_Intensity;							// 4 ��ˮǿ�ȣ�����������ȷ��С����� 1 λ����λ�� mm/min��
	unsigned short Radiation_Intensity;					// 2 �����ǿ�ȣ��޷�����������λ�� W/m2��
//	unsigned short CRC16;												// 2 У��λ
//	unsigned char End;													// 1 ����β�� 0x9
}Meteorological_StructDef;     //������


typedef struct
{
//	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
//	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	float Inclination;										// 4 ��б�ȣ�����������ȷ��С����� 1 λ����λ�� mm/m��
	float Inclination_X;										// 4 ˳����б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	float Inclination_Y;									// 4 ������б�ȣ�����������ȷ��С����� 1 λ����λ��mm/m��
	float Angle_X;												// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	float Angle_Y;												// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
}I1_TowerTilted_StructDef;   //������б����

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned char Unit_Sum;								// 1 �ɼ���Ԫ�������޷��������� ȡֵ��Χ�� ���� 0��
	unsigned char Unit_No;								// 1 �ɼ���Ԫ��ţ��޷��������� ȡֵ��Χ�� ���ڵ��� 0��
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	unsigned short Strain_Amplitude;			// 2 ����Ӧ���ֵ���޷�����������λ�� �̦� ��
	float Bending_Amplitude;							// 4 �������������������ȷ��С����� 3 λ����λ��mm��
	float Vibration_Frequency;						// 4 ΢����Ƶ�ʣ�����������ȷ��С����� 2 λ����λ�� Hz��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}BreezeVibrationCharacQuant_StructDef;//΢�������������ݱ���

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned char Unit_Sum;								// 1 �ɼ���Ԫ�������޷��������� ȡֵ��Χ�� ���� 0��
	unsigned char Unit_No;								// 1 �ɼ���Ԫ��ţ��޷��������� ȡֵ��Χ�� ���� 0��
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	unsigned char SamplePack_Sum;					// 1 ���ݲ���������޷��������� ȡֵ��Χ�� ���� 0��
	unsigned char SamplePack_No;					// 1 ���ݱ������޷��������� ȡֵ��Χ�� ���� 0��
	unsigned short Strain_Data[];					// 2�� N��N Ϊ���������� ΢�����źţ��޷�����������λ�� �̦� ��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}BreezeVibrationWaveformSignal_StructDef;     //΢���񶯲����ź����ݱ���

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	float Conductor_Sag;									// 4 ���߻���������������ȷ��С����� 3 λ����λ�� m��
	float Toground_Distance;							// 4 ���߶Եؾ��루����������ȷ��С����� 3 λ����λ��m��
	float Angle;													// 4 �߼г��ڴ�����������ˮƽ�߼нǣ�����������ȷ��С����� 2 λ����λ���㣩
	unsigned char Measure_Flag;						// 1 ��������ʶ�� ��0x00 ֱ�ӷ� ��0x01 ��ӷ�
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}ConductorSag_StructDef;     //���߻������ݱ���

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned char Unit_Sum;								// 1 �ɼ���Ԫ�������޷��������� ȡֵ��Χ�� ���� 0��
	unsigned char Unit_No;								// 1 �ɼ���Ԫ��ţ��޷��������� ȡֵ��Χ�� ���� 0��
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	float Line_Temperature;								// 4 ���£�����������ȷ��С����� 1 λ����λ���棩
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}ConductorTemperature_StructDef;     //�����¶����ݱ���


typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	float Equal_IceThickness;							// 4 ��ֵ������ȣ�����������ȷ��С����� 1 λ����λ��mm��
	float Tension;												//4 �ۺ������غɣ�����������ȷ��С����� 1 λ����λ��N��
	float Tension_Difference;							//4 ���������������������ȷ��С����� 1 λ����λ��N��
	unsigned char T_Sensor_Num;						// 1 ��װ�ģ�����������������
	struct
	{
		float Original_Tension;							// 4 �� 1 ����������ԭʼ����ֵ������������ȷ��С����� 1 λ����λ�� N��
		float Windage_Yaw_Angle;						//4 �� 1 ���������ľ�Ե�Ӵ���ƫ�ǣ�����������ȷ��С����� 2 λ����λ���㣩
		float Deflection_Angle;							//4 �� 1 ���������ľ�Ե�Ӵ�ƫб�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	}Sensor[];    												//N��������
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}IcingUnevenTensionDiff_StructDef;     //���������������������ݱ���

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	float Windage_Yaw_Angle;							// 4 ��ƫ�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	float Deflection_Angle;								// 4 ƫб�ǣ�����������ȷ��С����� 2 λ����λ���㣩
	float Least_Clearance;								// 4 ��С������϶������������ȷ��С����� 3 λ����λ�� m��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}MonsoonData_StructDef;     //��ƫ���ݱ���

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned char Unit_Sum;								// 1 �ɼ���Ԫ�������޷��������� ȡֵ��Χ�� ����0��
	unsigned char Unit_No;								// 1 �ɼ���Ԫ��ţ��޷��������� ȡֵ��Χ�� ���ڵ��� 0��
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	float U_Gallop_Amplitude;							// 4 �趯��ֵ������������ȷ��С����� 3 λ����λ�� m��
	float U_Vertical_Amplitude;						// 4 ��ֱ�趯��ֵ������������ȷ��С����� 3 λ����λ�� m��
	float U_Horizontal_Amplitude;					// 4 ˮƽ�趯��ֵ������������ȷ��С����� 3 λ����λ�� m��
	float U_AngleToVertical;							// 4 �趯��Բ��б�ǣ�����������ȷ��С����� 2λ����λ���㣩
	float U_Gallop_Frequency;							// 4 �趯Ƶ�ʣ�����������ȷ��С����� 2 λ����λ�� Hz��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}CharacQuantOfConduDanc_StructDef;     //�����趯���������ݱ���

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned char Unit_Sum;								// 1 �ɼ���Ԫ�������޷��������� ȡֵ��Χ�� ����0��
	unsigned char Unit_No;								// 1 �ɼ���Ԫ��ţ��޷��������� ȡֵ��Χ�� ����0��
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	unsigned char SamplePack_Sum;					// 1 ���ݲ���������޷��������� ȡֵ��Χ�� ����0��
	unsigned char SamplePack_No;					// 1 ���ݱ������޷��������� ȡֵ��Χ�� ���� 0��
	struct
	{
		float X; 				//�������λ�����꣨4Byte����λ������Ϊ����������ȷ��С����� 3 λ����λ�� m��
		float Y; 				//�������λ�����꣨4Byte��
		float Z; 				//�������λ�����꣨4Byte��
	}Displacement[];			// 12�� N��N Ϊ����������
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}TraverseTrack_StructDef;     //�����趯�켣���ݱ���

typedef struct
{
	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	float ESDD;														// 4 ��ֵ�����ܶȣ������ܣ�����������ȷ��С����� 3λ����λ�� mg/cm2��
	float NSDD;														// 4 �������ܶȣ������ܣ�����������ȷ��С����� 3 λ����λ�� mg/cm2��
	float Daily_Max_Temperature;					// 4 ������¶ȣ�����������ȷ��С����� 1 λ����λ�� �棩
	float Daily_Min_Temperature;					// 4 ������¶ȣ�����������ȷ��С����� 1 λ����λ�� �棩
	unsigned short Daily_Max_Humidity;		// 2 �����ʪ�ȣ��޷�����������λ�� ��RH��
	unsigned short Daily_Min_Humidity;		// 2 ����Сʪ�ȣ��޷�����������λ�� ��RH��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}SitePollutionSeverity_StructDef;     //�ֳ��ۻ�����ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;				// 1�����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ  ��0x01 ����������Ϣ
	unsigned char Request_Flag;						// 1��ʶλ���ֽ�λ�Ե�������ζ�Ӧ 9-11 �ĸ�ѡ����������õĲ����
																				// �� 1 ��ʾѡ���� 0 ��ʾδѡ���磺0x01 ��ʾ IP ��ѡ��
	unsigned char IP[4];									// 4 ״̬���װ�� IP ��ַ
	unsigned char Subnet_mask[4];					// 4 ��������
	unsigned char Gateway[4];							// 4 ����
	unsigned char PhoneNumber[20];				// 20 �ֻ�����
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}NetworkAdapterQuerySetting_StructDef;     //������������ѯ/�������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;   		// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ	 ��0x01 ����������Ϣ
	unsigned char Command_Status;					// 1 ���ݷ���״̬�� ��0xFF �ɹ�  ��0x00 ʧ��
	unsigned char Request_Flag;						// 1��ʶλ���ֽ�λ�Ե�������ζ�Ӧ 9-11 �ĸ�ѡ����������õĲ����
																				//�� 1 ��ʾѡ���� 0��ʾδѡ���磺 0x01 ��ʾ IP ��Ч
	unsigned char IP[4];									// 4 ״̬���װ�� IP ��ַ
	unsigned char Subnet_mask[4];					// 4 ��������
	unsigned char Gateway[4];							// 4 ����
	unsigned char PhoneNumber[20];				// 20 �ֻ�����
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}NetworkAdapterQuerySetting_Res_StructDef;     //������������ѯ/�������ݱ�����Ӧ

typedef struct
{
	unsigned char Request_Type;						// 1 �����������͡��ο����� C8-2 ��غ���
	unsigned int Data_Start_Time;					// 4 ������ʷ���ݵĿ�ʼʱ�䣨�����룩
	unsigned int Data_End_Time;						// 4 ������ʷ���ݵĽ���ʱ�䣨�����룩
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}UpperLevelDeviceRequestsData_StructDef;    //�ϼ��豸����״̬���װ����ʷ����

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ����0x01 ����������Ϣ
	unsigned char Request_Flag;						// 1 ��ʶλ
	unsigned char Request_Type;						// 1 �������������͡��ο����� C8-2 ���
	unsigned short Main_Time;							// 2 �ɼ�ʱ�����ڣ��޷�����������λ�� ���ӣ�
	unsigned char Heartbeat_Time;					// 1 �����������ڣ��޷�����������λ�� ���ӣ�
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}QuerySetSamplingPeriod_StructDef;     //�������ڲ�ѯ/�������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ����0x01 ����������Ϣ
	unsigned char Command_Status;					// 1	���ݷ���״̬�� ��0xFF �ɹ�  ��0x00 ʧ��
	unsigned char Request_Flag;						// 1 ��ʶλ
	unsigned char Request_Type;						// 1 ��������������
	unsigned short Main_Time;							// 2 �ɼ�ʱ�����ڣ��޷�����������λ�� ���ӣ�
	unsigned char Heartbeat_Time;					// 1 �����������ڣ��޷�����������λ�� ���ӣ�
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}QuerySetSamplingPeriod_Res_StructDef;     //�������ڲ�ѯ/�������ݱ�����Ӧ

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ��λ����ַ/�˿���Ϣ ��0x01 ������λ����ַ/�˿���Ϣ
	unsigned char Request_Flag;						// 1 ��ʶλ
	unsigned char IP_Address[4];					// 4 ��λ�� IP ��ַ
	unsigned short Port;									// 2 ��λ���˿ں�
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}PointsToInformationQuerySettings_StructDef;     //ָ����Ϣ��ѯ/�������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ��λ����ַ/�˿���Ϣ ��0x01 ������λ����ַ/�˿���Ϣ
	unsigned char Command_Status;					// 1���ݷ���״̬�� ��0xFF �ɹ�  ��0x00 ʧ��
	unsigned char Request_Flag;						// 1 ��ʶλ
	unsigned char IP_Address[4];					// 4 ��λ�� IP ��ַ
	unsigned short Port;									// 2 ��λ���˿ں�
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}PointsToInformationQuerySettings_Res_StructDef;     //ָ����Ϣ��ѯ/�������ݱ�����Ӧ

typedef struct
{
	unsigned char Request_Set_Flag;				// 1�����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ ��0x01 ����������Ϣ
	unsigned char Request_Flag;						// 1 ��ʶλ
	unsigned char Component_ID[17];				// 17 ������豸�����õ� ID��17 λ���룩
	unsigned char Original_ID[17];				// 17 ԭʼ ID���� 7 λ���ұ���� 10 λ�������ڲ�װ�ñ����ɣ��ڳ���ʱ�̻����������� ID ʱУ��
	unsigned char NEW_CMD_ID[17];					// 17 ״̬���װ�������õ� ID��17 λ���룩
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}ID_QuerySetting_StructDef;     //ID ��ѯ���������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ  ��0x01 ����������Ϣ
	unsigned char Command_Status;					// 1 ���ݷ���״̬�� ��0xFF �ɹ� ��0x00 ʧ��
	unsigned char Request_Flag;						// 1 ��ʶλ
	unsigned char Component_ID[17];				// 17 ������豸��ǰ�� ID��17 λ���룩
	unsigned char Original_ID[17];				// 17 ԭʼ ID
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}ID_QuerySetting_Res_StructDef;     //ID ��ѯ���������ݱ�����Ӧ

typedef struct
{
	unsigned char Reset_Mode;							// 1 װ�ø�λ�� ��0x00 ���渴λ������������0x01 ��λ������ģʽ��
																				// �ڵ���ģʽ�£�״̬���װ�ñ���ͨ��״̬�������ߣ����η��͸��ֱ��ģ�
																				//ͬʱ���ո����������ģʽ������ 10 ���ӡ�
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}ResetDevice_StructDef;     //װ�ø�λ���ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;				// 1�����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ����0x01 ����������Ϣ
	unsigned char Config_Total;						// 1 ���ò�������
	struct
	{
		unsigned char Name[6];				// 6 ��һ�����ò��������ƣ��ַ����� ȡֵ��Χ�� Сд��ĸ�����֣�
		unsigned char DataType;				// 1 ��һ�����ò������������ͣ��޷�������������0x00 �޷�����������0x01 �з���������3 0x02 �����ȸ�����
		unsigned int 	Data;						// 4 ��һ�����ò�����ֵ
	}Config[];
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}QuerySetConfigurationInformation_StructDef;     //������Ϣ��ѯ/�������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;			// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ����0x01 ����������Ϣ
	unsigned char Command_Status;				// 1 ���ݷ���״̬�� ��0xFF �ɹ� ��0x00 ʧ��
	unsigned char Config_Total;					// 1 ���ò�������
	struct
	{
		unsigned char Name[6];			// 6 ��n�����ò��������ƣ��ַ����� ȡֵ��Χ�� Сд��ĸ�����֣�
		unsigned char DataType;			// 1 ��n�����ò������������ͣ��޷�������������0x00 �޷�����������0x01 �з���������3 0x02 �����ȸ�����
		unsigned int  Data;			    // 4 ��n�����ò�����ֵ
	}Config[];
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}QuerySetConfigurationInformation_Res_StructDef;     //������Ϣ��ѯ/�������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;		// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ ��0x01 ����������Ϣ
	unsigned char Request_Flag;				// 1 ��ʶλ���ֽ�λ�Ե�������ζ�Ӧ 9-11 �ĸ�ѡ����������õĲ����
																		// �� 1 ��ʾѡ���� 0 ��ʾδѡ���磺 0x01 ��ʾ IP ��ѡ��
	unsigned char Color_Select;				// 1 ɫ��ѡ�� 0 Ϊ�ڰף� 1 Ϊ��ɫ
	unsigned char Resolution;					// 1 �Զ���ͼ��ֱ��ʣ��������¼��飺
																		// ��320 X 240 Ϊ 1��
																		// ��640 X 480 Ϊ 2��
																		// ��704 X 576 Ϊ 3��
																		// ��720 X 480 Ϊ 4�� �����壩
																		// ��1280 X 720 Ϊ 5����720P��
																		// ��1920 X 1080 Ϊ 6����1080P��
																		// ��1280P ���߸���Ϊ 7��
	unsigned char Luminance;					//  1 ���ȣ��޷��������� ȡֵ��Χ�� 1��100��
	unsigned char Contrast;						// 1 �Աȶȣ��޷��������� ȡֵ��Χ�� 1��100��
	unsigned char Saturation;					// 1 ���Ͷȣ��޷��������� ȡֵ��Χ�� 1��100��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}QuerySetImageAcquisitionParameters_StructDef;     //ͼ��ɼ�������ѯ/�������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;		// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ ��0x01 ����������Ϣ
	unsigned char Command_Status;			// 1 ���ݷ���״̬�� ��0xFF �ɹ� ��0x00 ʧ��
	unsigned char Request_Flag;				// 1 ��ʶλ
	unsigned char Color_Select;				// 1 ɫ��ѡ�� 0 Ϊ�ڰף� 1 Ϊ��ɫ
	unsigned char Resolution;					// 1 �Զ���ͼ��ֱ��ʣ��������¼��飺
																		// ��320 X 240 Ϊ 1��
																		// ��640 X 480 Ϊ 2��
																		// ��704 X 576 Ϊ 3��
	unsigned char Luminance;					// 1 ���ȣ��޷��������� ȡֵ��Χ�� 1��100��
	unsigned char Contrast;						// 1 �Աȶȣ��޷��������� ȡֵ��Χ�� 1��100��
	unsigned char Saturation;					// 1 ���Ͷȣ��޷��������� ȡֵ��Χ�� 1��100��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}QuerySetImageAcquisitionParameters_Res_StructDef;     //ͼ��ɼ�������ѯ/�������ݱ�����Ӧ

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ ��0x01 ����������Ϣ
	unsigned char Channel_No;							// 1 ͨ���ţ� ����ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char Group;									// 1 ���������������õ�ʱ������Ŀ�������� 72 ��
	struct
	{
		unsigned char Hour;									// 1 ʱ 1
		unsigned char Minute;								// 1 �� 1
		unsigned char Presetting_No;				// 1 Ԥ��λ�� 1������̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
	}GroupSet[];
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}PhotoScheduleSetting_StructDef;     //����ʱ����������ݱ���

typedef struct
{
	unsigned char Request_Set_Flag;				// 1 �����������ͱ�ʶ�� ��0x00 ��ѯ������Ϣ ��0x01 ����������Ϣ
	unsigned char Channel_No;							// 1 ͨ���ţ� ����ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char	Command_Status;					// 1 ���ݷ���״̬�� ��0xFF �ɹ� ��0x00 ʧ��
	unsigned char Group;									// 1 ���������������õ�ʱ������Ŀ�������� 72 ��
	struct
	{
		unsigned char Hour;									// 1 ʱ 1
		unsigned char Minute;								// 1 �� 1
		unsigned char Presetting_No;				// 1 Ԥ��λ�� 1������̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
	}GroupSet[];
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}PhotoScheduleSetting_Res_StructDef;     //����ʱ����������ݱ�����Ӧ

typedef struct
{
	unsigned char Channel_No;					// 1 ͨ���š���ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char Presetting_No;			// 1 Ԥ��λ�š�����̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}ManuallyRequestphoto_StructDef;     //�ֶ�����������Ƭ���ݱ���


typedef struct
{
	unsigned char Channel_No;				// 1 ͨ���š���ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char Presetting_No;		// 1 Ԥ��λ�š�����̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
	unsigned char Packet_High;			// 1 ������λ���޷���������
	unsigned char Packet_Low;				// 1 ������λ���޷���������
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}DeviceRequestedPhotoUpload_StructDef;     //װ������������Ƭ���ݱ���

typedef struct
{
	unsigned char Channel_No;				// 1 ͨ���š���ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char Presetting_No;		// 1 Ԥ��λ�š�����̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
	unsigned short Packet_No;				// 2 �ܰ������޷��������� ȡֵ��Χ�� ���ڵ��� 0��
	unsigned short Subpacket_No;		// 2 �Ӱ����ţ��޷��������� ȡֵ��Χ�� ���ڵ��� 1��
	unsigned char Sample[];			// N �������������һ������СΪ 512��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}RemoteImageData_StructDef;     //Զ��ͼ�����ݱ���

typedef struct
{
	unsigned char Channel_No;			// 1 ͨ���š���ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char Presetting_No;	// 1 Ԥ��λ�š�����̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
	unsigned int Time_Stamp;			// 4 ʱ��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}RemoteImageData_SendEndMark_StructDef;     //Զ��ͼ���������ͽ�����Ǳ���

typedef struct
{
	unsigned char Channel_No;								// 1 ͨ���š���ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char Presetting_No;						// 1 Ԥ��λ�š�����̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
	unsigned short ComplementPack_Sum;			// 2 ����������δ�յ����ܰ������޷���������
	unsigned short ComplementPack_No[];		// 2�� N ������������ 
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}RemoteImagePatchDataDelivery_StructDef;     //Զ��ͼ�񲹰������·�����

typedef struct
{
	unsigned char Channel_No;							// 1 ͨ���š���ʾ�ɼ�װ���ϵ��������š��磺 һ��װ������ 2 ��������� ��ֱ���Ϊ 1�� 2
	unsigned char Presetting_No;					// 1 Ԥ��λ�š�����̨���������õ�Ԥ��λ�ţ�������̨�������Ԥ��λ��Ϊ 255
	unsigned char Action;									// 1 ����ָ���������� C8-3 ����ָ���
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}RemoteCameraAdjustment_StructDef;     //�����Զ�̵��ڱ���

typedef struct
{
	unsigned int Clocktime_Stamp;					// 4 ��ǰʱ��
	float Battery_Voltage;								// 4 ��Դ��ѹ������������ȷ��С����� 1 λ����λ�� V��
	float Operation_Temperature;					// 4 �����¶ȣ�����������ȷ��С����� 1 λ����λ���棩
	float Battery_Capacity;								// 4 ���ʣ�����������������ȷ��С����� 1 λ����λ�� Ah��
	unsigned char FloatingCharge;					// 1 ����״̬�� ��0x00 ���  ��0x01 �ŵ�
	unsigned int Total_Working_Time;			// 4 ������ʱ�䣨�޷�����������λ�� Сʱ��
	unsigned int Working_Time;						// 4 ������������ʱ�䣨�޷�����������λ�� Сʱ��
	unsigned char Connection_State;				// 1 ����״̬�� ��0x00 �����д������������� ��0x01 ��һ�����߶�������������쳣
	unsigned int Send_Flow;								// 4 ���·����������޷�����������λ���ֽڣ�
	unsigned int Receive_Flow;						// 4 ���½����������޷�����������λ���ֽڣ�
	unsigned int Protocol_Version;				// 4 ͨ��Э��汾�ţ���С��λ���ĸ����֣�ÿ���ֽڴ���һ�Σ������� �汾�� 1.2.4.10 �� 4 ���ֽڱ�ʾΪ�� 01 02 04 0A
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}HeartbeatPacket_StructDef;     //�������ݱ���

typedef struct
{
	unsigned char Command_Status;					// 1 ���ݷ���״̬�� ��0xFF �ɹ� ��0x00 ʧ��
	unsigned char Mode;										// 1 ����ģʽ�� ��0x00 �л�������ģʽ ��0x01 �л�������ģʽ
	unsigned int Clocktime_Stamp;					// 4 ��λ����ǰʱ�䣨�����룬��ֵΪ 0 ʱ����ʾװ��ʱ������λ��ʱ��һ�£���ֵ�� 0 ʱ��װ����Ҫ���ݸ�ʱ�����Уʱ��
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}HeartbeatPacket_Res_StructDef;     //�������ݱ�����Ӧ

typedef struct
{
	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��
	unsigned char Fault_Desc[512];				// N��N<512Byte�� ����װ�ù���������Ϣ��UTF-8 ���룩
//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
}DeviceFaultInformation_StructDef;     //װ�ù�����Ϣ���ݱ���


typedef struct
{
	unsigned char File_Name[20];		// 20 �ļ������ԡ�\0����β ASCII�ַ�����ռ��20Byte,���� 20�ֽڵĲ���\0��
	unsigned int File_Size;					// 4 �ļ�����(���ֽ���ǰ)
  unsigned short Packet_No;				// 2 �ܰ���(���ֽ���ǰ)
	unsigned short Packet_Size;			// 2 ������(���ֽ���ǰ)
}RemoteUpgradeStart_StructDef;    //Զ��������ʼ���� ,Ӧ���ĸ�ʽһ����ԭ�ⲻ������Ӧ


typedef struct
{
	unsigned short File_Packet;			// 2 �ܰ���
	unsigned short Packet_No;				// 2 �����(���Ŵ� 1��ʼ����)
  unsigned short Packet_Len;			// 2 �����ݳ���
	unsigned char  File_Content[];	// Packet_Len �ļ�����
}RemoteUpgradeData_StructDef;     //Զ���������ݱ���

typedef struct
{
	unsigned char File_Name[20];		// 20 �ļ������ԡ�\0����β ASCII�ַ�����ռ��20Byte,���� 20�ֽڵĲ���\0��
	unsigned int File_Size;					// 4 �ļ�����(���ֽ���ǰ)
}RemoteUpgradeEnd_StructDef;      //Զ���������ݱ��·��������

typedef struct
{
	unsigned short Upload_Result;				// 2  �������(���ֽ���)
	unsigned char File_Name[20];				// 20 �ļ������ԡ�\0����β ASCII�ַ�����ռ��20Byte
	unsigned short ComplementPack_Sum;	// 2 ����������δ�յ����ܰ���(���ֽ���)
	unsigned int ComplementPack_No;			// 4��N ������������(���ֽ���)
}ComplementPack_StructDef;        		//����������ݱ���


typedef struct
{
	unsigned char Ctrl_Cmd;							 // 1   0:���渴λ 1:��λ������ģʽ 2:��λ��Ӧ�ó���ģʽ
}SoftwareUpgradeCtrlCmd_StructDef;     //���������������

typedef struct
{
	unsigned char Ctrl_Result;							// 1  0:�ɹ� ����:ʧ��
}SoftwareUpgradeCtrlCmd_Res_StructDef;     //�����������������Ӧ����

//��ȡ�汾�ű����ޱ�������
typedef struct
{
	unsigned char version[20];					// 20 ��ǰ�汾�Ų�����\0������
	unsigned char mode;									// 1 ģʽ
	unsigned char State[20];						// 20 �쳣״̬
}ObtainVersionNumber_Res_StructDef;   //��ȡ�汾����Ӧ����


//typedef struct
//{
//	unsigned char Component_ID[17];				// 17 ������豸 ID��17 λ���룩
//	unsigned int Time_Stamp;							// 4 �ɼ�ʱ��

//	unsigned short CRC16;									// 2 У��λ
//	unsigned char End;										// 1 ����β�� 0x9
//}_StructDef;     //����

//	if(frame[0]==0xA5&&frame[1]==0x5A)
//	{
//		Packet_Length= (frame[3]<<8) + frame[2];
//		pGDW_Frame = (Frame_StructDef*)malloc(sizeof(Frame_StructDef)+(Packet_Length+3)*sizeof(unsigned char));//�䳤�����ڴ�: ��������(�䳤) + У��λ(2λ) + ����β(1λ) ���1420
//		p=(unsigned char*)pGDW_Frame;
//		for(i=0;i<frame_length;i++) p[i]=frame[i];
//		
//	}

//#define USE_Meteorological 			1		//������
//#define USE_TowerTilted					1   //������б����

//#if (USE_Meteorological)
//	extern Meteorological_StructDef Meteorological;     //������
//#endif

//#if (USE_TowerTilted)
//	extern TowerTilted_StructDef TowerTilted;   				//������б����
//#endif

//extern NetworkAdapterQuerySetting_StructDef  *pNetworkAdapterQuerySetting;     //������������ѯ/�������ݱ���


//unsigned short RTU_CRC(unsigned char * puchMsg,unsigned short usDataLen);  //CRC У���㷨
//void GDW_Protocol_test(unsigned short Packet_Length);
//void FrameAnalysis(Frame_StructDef *frame);	
//unsigned char CMD_to_CMA(unsigned char Frame_Type,unsigned char Packet_Type,unsigned char *pdata,unsigned short len);

#endif

