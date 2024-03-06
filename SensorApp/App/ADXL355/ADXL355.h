#ifndef __ADXL355_H__
#define __ADXL355_H__

#include "main.h"
#include "spi.h"

#define pi_ang 	57.295779513			//180/��
#define PI      3.1415926

#define XL355_hspi	hspi2
#define XL355_SPI_CS 			ADXL355_CS_Pin
#define XL355_SPI_CS_PORT 	ADXL355_CS_GPIO_Port

////IO��������											   

#define XL355_DEVID_AD			0x00	//R	ADI ID �Ĵ���,�˼Ĵ������� ADI ��˾ ID 0xAD
#define XL355_DEVID_MST			0x01	//R ADI MEMS ID �Ĵ���,�˼Ĵ������� ADI ��˾ MEMS ID 0x1D	
#define XL355_PARTID				0x02	//R	���� ID �Ĵ���,�˼Ĵ����������� ID 0xED���˽��� 355��
#define XL355_REVID					0x03	//R	��Ʒ�汾 ID �Ĵ���,�˼Ĵ���������Ʒ�汾 ID���� 0x00 ��ʼ�����ÿ���޶������� 1
#define XL355_STATUS				0x04	//R	״̬�Ĵ���,�üĴ���������ӳ ADXL355 ����״����λ��
#define XL355_FIFO_ENTRIES	0x05	//R	FIFO ��Ŀ�Ĵ����üĴ���,��ʾ FIFO �������д��ڵ���Ч�����������������������ķ�Χ�� 0 �� 96
#define XL355_TEMP2					0x06	//R	�¶����ݼĴ���,TEMP2 ���� 12 λֵ�� 4 �������Чλ
#define XL355_TEMP1					0x07	//R	TEMP1 ������ 8 �������Чλ
#define XL355_XDATA3				0x08	//R	X�����ݼĴ���,�������Ĵ�������X����ٶ����ݡ�����Ϊ����룬���ö����Ʋ����ʽ��
#define XL355_XDATA2				0x09
#define XL355_XDATA1				0x0A
#define XL355_YDATA3				0x0B	//R	Y�����ݼĴ���,�������Ĵ�������Y����ٶ����ݡ�����Ϊ����룬���ö����Ʋ����ʽ��
#define XL355_YDATA2				0x0C
#define XL355_YDATA1				0x0D
#define XL355_ZDATA3				0x0E	//R	Z�����ݼĴ���,�������Ĵ�������Z����ٶ����ݡ�����Ϊ����룬���ö����Ʋ����ʽ��
#define XL355_ZDATA2				0x0F
#define XL355_ZDATA1				0x10
#define XL355_FIFO_DATA			0x11	//R	FIFO���ʼĴ���
#define XL355_OFFSET_X_H		0x1E	//R/W	X��ƫ��,X��ʧ�������Ĵ���
#define XL355_OFFSET_X_L		0x1F	
#define XL355_OFFSET_Y_H		0x20	//R/W	Y��ƫ��,Y��ʧ�������Ĵ���
#define XL355_OFFSET_Y_L		0x21
#define XL355_OFFSET_Z_H		0x22	//R/W	Z��ƫ��,Z��ʧ�������Ĵ���
#define XL355_OFFSET_Z_L		0x23
#define XL355_ACT_EN				0x24	//R/W	�ʹ�ܼĴ���
#define XL355_ACT_THRESH_H	0x25	//R/W	���ֵ�Ĵ���,ACT_TRESH[15:0]
#define XL355_ACT_THRESH_L	0x26
#define XL355_ACT_COUNT			0x27	//R/W	������Ĵ���,�������ĳ�����ֵ�������¼���
#define XL355_FILTER				0x28	//R/W	�˲������üĴ���,ָ���ڲ���ͨ�͵�ͨ�˲����Ĳ���
#define XL355_FIFO_SAMPLES	0x29	//R/W	FIFO �����Ĵ���,ʹ�� FIFO_SAMPLES ֵָ�� FIFO �д洢�����������üĴ�����Ĭ��ֵΪ 0x60���Ա��ⴥ�� FIFO ˮӡ�ж�
#define XL355_INT_MAP				0x2A	//R/W	�ж�����(INTx)����ӳ��Ĵ���
#define XL355_SYNC					0x2B	//R/W	����ͬ��,ʹ�øüĴ��������ⲿ��ʱ��������
#define XL355_RANGE					0x2C	//R/W	I2C �ٶȡ��жϼ��Ժͷ�Χ�Ĵ���
#define XL355_POWER_CTL			0x2D	//R/W	��Դ���ƼĴ���,ʡ�繦�ܿ���
#define XL355_SELF_TEST			0x2E	//R/W	�Լ�Ĵ���
#define XL355_RESET					0x2F	//W	��λ�Ĵ���,д����� 0x52 ����λ�������������ϵ縴λ(POR)

/* Temperature parameters */
#define ADXL355_TEMP_BIAS       (float)1852.0      /* Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_SLOPE      (float)-9.05       /* Accelerometer temperature change from datasheet (LSB/degC) */

#pragma pack (1) /*ָ����1�ֽڶ���*/

struct ADXL355_STATUS_DEF
{
	uint8_t DATA_RDY : 	1;	//0 DATA_RDY ������������ x �ᡢ y ��� z ����������ҿ��Զ�ȡ����� 0x0 R
	uint8_t FIFO_FULL: 	1;	//1 FIFO_FULL �ﵽ FIFO ˮӡ�� 0x0 R
	uint8_t FIFO_OVR : 	1;	//2 FIFO_OVR FIFO �������ɵ����ݶ�ʧ�� 0x0 R
	uint8_t ACT 		 : 	1;	//3 � ��⵽ THRESH_ACT �� COUNT_ACT �Ĵ���������Ļ�� 0x0 R
	uint8_t NVM_BUSY : 	1;	//4 NVM_BUSY NVM ������æ��ˢ�¡���̻������Լ�(BIST)�� 0x0 R
	uint8_t Reserve  : 	3	;	//[7:5] ���� ������ 0x0 R
};

struct ADXL355_Typedef
{
	float	x_ang;
	float y_ang;
	float z_ang;
	float Temp;
	double	ax;
	double  ay;
	double	az;
	float Scale;
	struct ADXL355_STATUS_DEF Status;
};

//��������λֵ��ƫ��ֵ
struct ADXL355_XYZ_Offset_Def
{
	int16_t	X_Offset;
	int16_t Y_Offset;
	int16_t	Z_Offset;
//	int16_t X_acc_Offset;
//	int16_t Y_acc_Offset;
//	int16_t Z_acc_Offset;
};

#pragma pack ()

/**************************** Configuration parameters **********************/
/* Temperature parameters */
#define ADXL355_TEMP_BIAS       (float)1852.0      /* ���ٶȼ��¶�ƫ��(��ADC������)��25�� Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_SLOPE      (float)-9.05       /*���ټ��¶ȱ仯���ݱ�(LSB/degC) Accelerometer temperature change from datasheet (LSB/degC) */

/* Accelerometer parameters */
#define ADXL_RANGE     2     /* ADXL362 sensitivity: 2, 4, 8 [g] */

#define ACT_VALUE          50     /* Activity threshold value */

#define INACT_VALUE        50     /* Inactivity threshold value */

#define ACT_TIMER          100    /* Activity timer value in ms */

#define INACT_TIMER        10     /* Inactivity timer value in seconds */

/****************************** Global Data ***********************************/

//extern volatile int32_t i32SensorX;
//extern volatile int32_t i32SensorY;
//extern volatile int32_t i32SensorZ;

//extern volatile uint32_t ui32SensorX;
//extern volatile uint32_t ui32SensorY;
//extern volatile uint32_t ui32SensorZ;
//extern volatile uint32_t ui32SensorT;

//extern volatile uint32_t ui32timer_counter;
 
 
/*************************** Functions prototypes *****************************/

void ADXL355_Init(void);
void ADXL355_Start_Sensor(void);
void ADXL355_Stop_Sensor(void);
void ADXL355_Data_Scan(struct ADXL355_Typedef *pADXL355);
int32_t ADXL355_Acceleration_Data_Conversion (uint32_t ui32SensorData);
void ADXL355_Offset_RW(uint8_t axle,int16_t *Val,uint8_t mode);							//XYZ ��ʧ�������Ĵ�����д.
void ADXL355_ACT_THRESH_RW(uint16_t *ACT_Val,uint8_t *ACT_Count,uint8_t mode);//���ֵ,������Ĵ����Ĵ�����д.
void ADXL355_ACT_EN_W(uint8_t axle_bit); //�ʹ�ܼĴ���
struct ADXL355_STATUS_DEF ADXL355_Status_Read(void); //��ȡ״̬
float ADXL355_Read_Temp(void);       //��ȡ�¶�
	
void Shell_ADXL355_ACC_Read(struct ADXL355_Typedef *pADXL355);
void ADXL355_ACT_Set(void); //
void ADXL355_Reset_Sensor(void);
#endif /* ADXL355_H_ */


