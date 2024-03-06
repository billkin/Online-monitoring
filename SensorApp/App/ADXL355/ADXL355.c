#include "ADXL355.h"
#include <string.h>
#include <math.h>
#include "SystemGlobalVar.h"

/****************************** Global Data ***********************************/
int32_t volatile i32SensorX;
int32_t volatile i32SensorY;
int32_t volatile i32SensorZ;
int32_t volatile i32SensorT;
uint32_t volatile ui32SensorX;
uint32_t volatile ui32SensorY;
uint32_t volatile ui32SensorZ;
uint32_t volatile ui32SensorT;


//volatile uint32_t ui32timer_counter = 0;

//struct ADXL355_Typedef	ADXL355;


/*
  �������ܣ�ͨ��SPI �� ADXL355 WRITEһ���ֽڵ�����
  adress����ַ 
  data  ������
  дһ���ֽڵ�����    
*/
void SPI_ADXL355_WRITE(uint8_t adress, uint8_t data)
{

	unsigned char writeadress;                                          //unsigned charû�з���λ������ܱ�ʾ0~255
	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);    //CS = LOW  Ƭѡ�ź�����
	
	writeadress = adress<<1;                                            //�Ĵ����ĵ�ַ7λ ����1λ �ұ߲�0��ʾд����  �� xxxxxxx0��8λ
	SPI_ReadWriteByte(&XL355_hspi,writeadress);		                                //write adressд���ַ
	SPI_ReadWriteByte(&XL355_hspi,data);                                            //data to be writtenд������
	
	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET);		//CS = High	 Ƭѡ�ź�����                                                     //Wait Time
}

 
/*
  �������ܣ�ͨ��SPI �� ADXL355��ȡn���ֽڵ�����
  adress����ַ 
  ��ȡbytes���ֽڵ�����    
*/
 uint32_t SPI_ADXL355_READBYTE(uint8_t adress,uint8_t bytes)
{

	uint32_t result;
	unsigned char readadress;

  HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);  //CS = LOW
	readadress = adress<<1;                                           //�Ĵ����ĵ�ַ7λ ����1λ 
	readadress |= 0x01;                                               //�Ĵ����ĵ�ַ7λ ����1λ �ұ���1���ʾ������  �� xxxxxxx1��8λ
	result=SPI_ReadWriteByte(&XL355_hspi,readadress);                             //read adress
	result=SPI_ReadWriteByte(&XL355_hspi,0xff);                                 //Read 8bit data��ȡһ���ֽڵ����� 8λ 00000000 00000000 00000000 xxxxxxxx
	if(bytes==1) 
	{
		HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
		return result;
	}
	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 16bit data��ȡ2���ֽڵ����� 16λ00000000 00000000 xxxxxxxx xxxxxxxx
	if(bytes==2) 
	{
		HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
		return result;
	}
	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 24bit data��ȡ3���ֽڵ����� 24λ00000000 xxxxxxxx xxxxxxxx xxxxxxxx

	//HAL_Delay(1);			                                          //CS to SCLK set up time
	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
	//HAL_Delay(1);			                                          //Wait Time

	return result;
}
 /*
  �������ܣ�ͨ��SPI �� ADXL355��ȡһ���ֽڵ�����
  adress����ַ 
  ��ȡһ���ֽڵ�����    
*/
// unsigned char SPI_ADXL355_READ1BYTE(uint8_t adress)
//{

//	uint32_t result;
//	unsigned char readadress;
//	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);    //CS = LOW  Ƭѡ�ź�����
////	HAL_Delay(1);						                               //CS to SCLK set up time
//	readadress = adress<<1;                                            //�Ĵ����ĵ�ַ7λ ����1λ 
//	readadress |= 0x01;                                                //�Ĵ����ĵ�ַ7λ ����1λ �ұ���1���ʾ������  �� xxxxxxx1��8λ
//	result=SPI_ReadWriteByte(&XL355_hspi,readadress);                              //read adress����ַ
//  result=SPI_ReadWriteByte(&XL355_hspi,0xff);                                    //Read 8bit data (Send Dummy data 0xff)
//	
////  HAL_Delay(1);			                                           //CS to SCLK set up time
//	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET);     //CS = High	
////	HAL_Delay(1);				                                       //Wait Time

//	return result;
//}

///*
//  �������ܣ�ͨ��SPI �� ADXL355��ȡ3���ֽڵ�����
//  adress����ַ 
//  ��ȡ3���ֽڵ�����    
//*/
// uint32_t SPI_ADXL355_READ3BYTE(uint8_t adress)
//{

//	uint32_t result;
//	unsigned char readadress;

//    HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);  //CS = LOW
////	HAL_Delay(1);					                                  //CS to SCLK set up time
//	readadress = adress<<1;                                           //�Ĵ����ĵ�ַ7λ ����1λ 
//	readadress |= 0x01;                                               //�Ĵ����ĵ�ַ7λ ����1λ �ұ���1���ʾ������  �� xxxxxxx1��8λ
//	result=SPI_ReadWriteByte(&XL355_hspi,readadress);                             //read adress
//  result=SPI_ReadWriteByte(&XL355_hspi,0xff);                                   //Read 8bit data��ȡһ���ֽڵ����� 8λ 00000000 00000000 00000000 xxxxxxxx
//	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 16bit data��ȡ2���ֽڵ����� 16λ00000000 00000000 xxxxxxxx xxxxxxxx
//	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 24bit data��ȡ3���ֽڵ����� 24λ00000000 xxxxxxxx xxxxxxxx xxxxxxxx

//	//HAL_Delay(1);			                                          //CS to SCLK set up time
//	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
//	//HAL_Delay(1);			                                          //Wait Time

//	return result;
//}

/*
  ��������: ADXL355 ��������ʼ����
*/
void ADXL355_Start_Sensor(void)
{
   uint8_t ui8temp;
   ui8temp = (uint8_t)SPI_ADXL355_READBYTE(XL355_POWER_CTL,1);       /* Read POWER_CTL register, before modifying it */
   ui8temp = ui8temp & 0xFE;                                        /* Set measurement bit in POWER_CTL register */
   SPI_ADXL355_WRITE(XL355_POWER_CTL, ui8temp);                     /* Write the new value to POWER_CTL register */
	HAL_Delay(20);
}

/**
  ��������: ���ټƽ������ģʽ.
**/
void ADXL355_Stop_Sensor(void)
{
   uint8_t ui8temp;
   ui8temp = (uint8_t)SPI_ADXL355_READBYTE(XL355_POWER_CTL,1);     /* Read POWER_CTL register, before modifying it */
   ui8temp = ui8temp | 0x01;                                      /* Clear measurement bit in POWER_CTL register */
   SPI_ADXL355_WRITE(XL355_POWER_CTL, ui8temp);                   /* Write the new value to POWER_CTL register */
	 HAL_Delay(20);
}

/**
  ��������: ��λ���ټ�.
**/
void ADXL355_Reset_Sensor(void)
{
  SPI_ADXL355_WRITE(XL355_RESET, 0x52);  //д����� 0x52 ����λ�������������ϵ縴λ(POR)
	HAL_Delay(100);
}

/**
  ��������: XYZ ��ʧ�������Ĵ�����д.
�����������źŴ���֮�󣬽�ʧ���ӵ� x/y/z �������ϡ����ݲ��ö����Ʋ���
��ʽ�� OFFSET_X[15:0]����Ҫ���� XDATA[19:4]����Ҫ��һ�¡�
**/
void ADXL355_Offset_RW(uint8_t axle,int16_t *Val,uint8_t mode)
{
   int16_t ui16temp;
	 if(mode==1)  //��
	 {
		 switch(axle)
		 {
			 case 0: 
					ui16temp = (uint8_t)SPI_ADXL355_READBYTE(XL355_OFFSET_X_H,1);
					ui16temp	<<=	8;
					ui16temp |= (uint8_t)SPI_ADXL355_READBYTE(XL355_OFFSET_X_L,1);
					break;
			 case 1: 
					ui16temp = (uint8_t)SPI_ADXL355_READBYTE(XL355_OFFSET_Y_H,1);
					ui16temp	<<=	8;
					ui16temp |= (uint8_t)SPI_ADXL355_READBYTE(XL355_OFFSET_Y_L,1);
					break;
			 case 2: 
					ui16temp = (uint8_t)SPI_ADXL355_READBYTE(XL355_OFFSET_Z_H,1);
					ui16temp	<<=	8;
					ui16temp |= (uint8_t)SPI_ADXL355_READBYTE(XL355_OFFSET_Z_L,1);
					break;
			 default: break;
		 }
		 *Val = ui16temp;
   }
	 else //if(mode==0)  //д
	 {
		 ui16temp = *Val;
		 switch(axle)
		 {
			 case 0: 
					SPI_ADXL355_WRITE(XL355_OFFSET_X_H, ui16temp>>8);
					SPI_ADXL355_WRITE(XL355_OFFSET_X_L,ui16temp&0xff);
					break;
			 case 1: 
					SPI_ADXL355_WRITE(XL355_OFFSET_Y_H, ui16temp>>8);
					SPI_ADXL355_WRITE(XL355_OFFSET_Y_L,ui16temp&0xff);
					break;
			 case 2: 
					SPI_ADXL355_WRITE(XL355_OFFSET_Z_H, ui16temp>>8);
					SPI_ADXL355_WRITE(XL355_OFFSET_Z_L,ui16temp&0xff);
					break;
			 default: break;
		 }
   }
}

/**
  ��������: ���ֵ��������Ĵ����Ĵ�����д.
������ֵ�����ٶȷ��ȱ������ ACT_THRESH ���ܴ������������
ACT_THRESH ���޷��ŷ��ȡ� ACT_TRESH[15:0]����Ҫ���� XDATA�� YDATA
�� ZDATA [18:3]����Ҫ��һ�¡��������ĳ�����ֵ�������¼���
**/
void ADXL355_ACT_THRESH_RW(uint16_t *ACT_Val,uint8_t *ACT_Count,uint8_t mode)
{
   uint16_t ui16temp;
	 if(mode==1)  //��
	 {
			ui16temp = (uint8_t)SPI_ADXL355_READBYTE(XL355_ACT_THRESH_H,1);
			ui16temp	<<=	8;
			ui16temp |= (uint8_t)SPI_ADXL355_READBYTE(XL355_ACT_THRESH_L,1);
		 *ACT_Val = ui16temp;
		 *ACT_Count = (uint8_t)SPI_ADXL355_READBYTE(XL355_ACT_COUNT,1);
   }
	 else //if(mode==0)  //д
	 {
		 ui16temp = *ACT_Val;
		 SPI_ADXL355_WRITE(XL355_ACT_THRESH_H, ui16temp>>8);
		 SPI_ADXL355_WRITE(XL355_ACT_THRESH_L,ui16temp&0xff);
		 SPI_ADXL355_WRITE(XL355_ACT_COUNT,*ACT_Count);
   }
}

/**
  ��������: �ʹ�ܼĴ���.
	axle_bit:
	bit[7:3]  ���� 0x0 
	bit 2 ACT_Z  	1��Z�������ǻ����㷨����ɲ���   0��Z�����ݲ��ǻ����㷨����ɲ���
	bit 1 ACT_Y  	1��Y�������ǻ����㷨����ɲ���	 0��Y�����ݲ��ǻ����㷨����ɲ���
	bit 0 ACT_X   1��X�������ǻ����㷨����ɲ���	 0��X�����ݲ��ǻ����㷨����ɲ���
**/
void ADXL355_ACT_EN_W(uint8_t axle_bit)
{
		SPI_ADXL355_WRITE(XL355_ACT_EN, axle_bit&0x07);
}

//��ȡ״̬
struct ADXL355_STATUS_DEF ADXL355_Status_Read(void)
{
	uint8_t status;
	struct ADXL355_STATUS_DEF status_res;
	status = SPI_ADXL355_READBYTE(XL355_STATUS,1);
	status_res = *(struct ADXL355_STATUS_DEF*)&status;
	return status_res;
}


/*
  ��������: ����ADXL355���ֵ 
  ������ؼĴ���  XL355_RANGE��XL355_SYNC�� 
  �жϴ������Ƿ���������
*/
void ADXL355_ACT_Set(void)
{
	uint16_t ACT_Val=16000;
	uint8_t ACT_Count=3;
	float sin_val; 
//	if(fabs(SystemGlobalVar.TT_Offset.Angle_X)>fabs(SystemGlobalVar.TT_Offset.Angle_Y))
//		Off_val = fabs(SystemGlobalVar.TT_Offset.Angle_X);
//	else
//		Off_val = fabs(SystemGlobalVar.TT_Offset.Angle_Y);
//	if(fabs(SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit)<fabs(SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit))
//	{
//		sin_val = fabs(SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit)-Off_val;
//		if(sin_val<Off_val)  sin_val = fabs(SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit);
//	}
//	else
//	{
//		sin_val = fabs(SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit)-Off_val;
//		if(sin_val<Off_val)  sin_val = fabs(SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
//	}
	sin_val = fabs(SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit);
	if(sin_val>0)
	{
		APP_LOG("XL355���ѽǶȣ�%.2f\r\n",sin_val);
		//APP_LOG("sin%.2f��%.2f\r\n",sin_val,sin(sin_val*PI/180));
		ACT_Val = sin(sin_val*PI/180)*32051;  //32051 = 1000000/3.9/8(���������ֲ����)
		//APP_LOG("XL355���ֵ��%d\r\n",ACT_Val);
		ADXL355_ACT_THRESH_RW(&ACT_Val,&ACT_Count,0);
		ADXL355_ACT_EN_W(0x03); 		//�ʹ�ܼĴ���	
	}
	else
	{
		APP_LOG("XL355��������ǻ���\r\n");
		ADXL355_ACT_EN_W(0x00); 		//��ʹ�� �ʹ�ܼĴ���	
	}
}

/*
  ��������: ADXL355 ��ʼ��
  ������ؼĴ���  XL355_RANGE��XL355_SYNC�� 
  �жϴ������Ƿ���������
*/
void ADXL355_Init(void)
{
  int16_t temp_int16;
	ADXL355_Stop_Sensor();
	HAL_Delay(100);
	//SPI_ADXL355_WRITE(XL355_RANGE,0x81);  /* Set sensor range within RANGE register */ //����Ϊ 1000 0001 ����ģʽ  ��2g
	#if ADXL_RANGE == 2
		SPI_ADXL355_WRITE(XL355_RANGE, 0xC1);    /* I2C����ģʽ,NT1��INT2Ϊ�ߵ�ƽ��Ч, ��Χ��2 g*/
		#define ADXL355_SCALE	256000.0f
	#endif

	#if ADXL_RANGE == 4
	 SPI_ADXL355_WRITE(XL355_RANGE, 0x82);          /* Set sensor range within RANGE register */
	 #define ADXL355_SCALE	 128000.0f
	#endif

	#if ADXL_RANGE == 8
	 SPI_ADXL355_WRITE(XL355_RANGE, 0x83);          /* Set sensor range within RANGE register */
	 #define ADXL355_SCALE 64000.0f
	#endif
	SPI_ADXL355_WRITE(XL355_SYNC,0x03); 	//    
	SPI_ADXL355_WRITE(XL355_FILTER,0x06); //�˲������üĴ���,δʹ�ܸ�ͨ�˲���,��ͨ�˲�62.5 Hz �� 15.625 Hz
	SPI_ADXL355_WRITE(XL355_INT_MAP,0x08);//INT1 �ϵĻ�ж�ʹ��
	
	uint8_t ui8test  = SPI_ADXL355_READBYTE(XL355_DEVID_AD,1);  /* 0xAD Read the ID register */ 
	uint8_t ui8test2 = SPI_ADXL355_READBYTE(XL355_DEVID_MST,1);  /* 0x1D Read the ID register */
	uint8_t ui8test3 = SPI_ADXL355_READBYTE(XL355_PARTID,1);     /* 0xED Read the ID register */
	uint8_t ui8test4 = SPI_ADXL355_READBYTE(XL355_REVID,1);      /* 0x03 Read the ID register */
  
	temp_int16 = SystemGlobalVar.ADXL355.X_Offset;
	ADXL355_Offset_RW(0,&temp_int16,0);	 //X��ʧ�������Ĵ���д
	temp_int16 = SystemGlobalVar.ADXL355.Y_Offset;
	ADXL355_Offset_RW(1,&temp_int16,0);	 //Y��ʧ�������Ĵ���д
	temp_int16 = SystemGlobalVar.ADXL355.Z_Offset;
	ADXL355_Offset_RW(2,&temp_int16,0);	 //Z��ʧ�������Ĵ���д
  	
  ADXL355_ACT_Set();
	ADXL355_Start_Sensor();
	/* ��֤ADXL355�豸�Ƿ���������*/
		if(SPI_ADXL355_READBYTE(XL355_PARTID,1)==0xED)	
	  {
		  APP_LOG("ADXL355 Init OK!\r\n");
//		  APP_PRINTF("XL355_DEVID_AD:0x%x  XL355_DEVID_MST:%x\r \n",ui8test,ui8test2);
//		  APP_PRINTF("XL355_PARTID:0x%x  XL355_REVID:0x%x\r \n",ui8test3,ui8test4);
	  }
	  else
	  {
		  APP_LOG("ADXL355 Error��\r\n");
	  }	
		
}

/*
  �������ܣ� ADXL355���ٶ�����ת��
*/
int32_t ADXL355_Acceleration_Data_Conversion (uint32_t ui32SensorData)
{
   int32_t volatile i32Conversion = 0;

	//���ٶ�����Ϊ20λ ��ȡ��3���ֽڣ�24λ�����ٶ������������4λ��Ч[3:0]     00000000 xxxxxxxx xxxxxxxx xxxxxxxx
   ui32SensorData = (ui32SensorData  >> 4);                  //����4λ
   ui32SensorData = (ui32SensorData & 0x000FFFFF);           //�õ�32λ��ʵ������ 00000000 0000xxxx xxxxxxxx xxxxxxxx
   if((ui32SensorData & 0x00080000)  == 0x00080000)          //       ���������  00000000 00001xxx xxxxxxxx xxxxxxxx
	 {
         i32Conversion = (ui32SensorData | 0xFFF00000);      //      ������ת���� 11111111 11111xxx xxxxxxxx xxxxxxxx
				//i32Conversion = 0xFFFFFFFF-i32Conversion+1;
	 }
   else
	 {
          i32Conversion = ui32SensorData;                   //      ��������ת���� 00000000 0000xxxx xxxxxxxx xxxxxxxx
   }
   return i32Conversion;
}

void ADXL355_delay(uint16_t time)
{
	while(time--){;}
}


int8_t ADXL355_Data_Read()
{
	uint16_t time=0;
	struct ADXL355_STATUS_DEF statu;
	do{
			statu = ADXL355_Status_Read(); //��ȡ״̬
			if(statu.DATA_RDY)
			{
				ui32SensorX=SPI_ADXL355_READBYTE(XL355_XDATA3,3); //XL355_XDATA3ΪX��3���ݼĴ������׵�ַ  ui32SensorXΪ3���ֽڵ�����  00000000 xxxxxxxx xxxxxxxx xxxxxxxx
				ui32SensorY=SPI_ADXL355_READBYTE(XL355_YDATA3,3); //XL355_YDATA3ΪY��3���ݼĴ������׵�ַ
				ui32SensorZ=SPI_ADXL355_READBYTE(XL355_ZDATA3,3); //XL355_ZDATA3ΪZ��3���ݼĴ������׵�ַ
				i32SensorX = ADXL355_Acceleration_Data_Conversion(ui32SensorX);     //��AD�ɼ��������ݽ���ת��
				i32SensorY = ADXL355_Acceleration_Data_Conversion(ui32SensorY);
				i32SensorZ = ADXL355_Acceleration_Data_Conversion(ui32SensorZ);
				return 1;
			}
			else
			{
				ADXL355_delay(100);
				time++;
				if(time==850)
				{
					APP_LOG("X��Y��Z����ٶ�ֵ��ȡ����\r\n");
					return 0;
				}
			}
	}while(1);
}

/*
  �������ܣ� ADXL355����ɨ��
  Reads the accelerometer data.
  ����ADXL355�ɼ�������   
*/
void ADXL355_Data_Pro(struct ADXL355_Typedef *pADXL355)
{
	if(ADXL355_Data_Read())
	{
		pADXL355->ax=(double)i32SensorX/ADXL355_SCALE;
		pADXL355->ay=(double)i32SensorY/ADXL355_SCALE;
		pADXL355->az=(double)i32SensorZ/ADXL355_SCALE;

		pADXL355->x_ang = atan(pADXL355->ax/sqrt(pADXL355->ay * pADXL355->ay + pADXL355->az * pADXL355->az)) * pi_ang ;
		pADXL355->y_ang = atan(pADXL355->ay/sqrt(pADXL355->ax * pADXL355->ax + pADXL355->az * pADXL355->az)) * pi_ang ;
		//ADXL355.z_ang = atan(ADXL355.az/sqrt(ADXL355.ax * ADXL355.ax + ADXL355.ay * ADXL355.ay)) * pi_ang ;
	}
}

#define DATA_NUM	20

void ADXL355_Data_Scan(struct ADXL355_Typedef *pADXL355)
{
		// t1 = HAL_GetTick();//
	uint16_t i;
	float	x_buf[DATA_NUM];
	float y_buf[DATA_NUM];
	float x_min,x_max,y_min,y_max;
	struct ADXL355_Typedef ADXL355_temp;
	ADXL355_Data_Pro(pADXL355);
	x_min=x_max=pADXL355->x_ang;
	y_min=y_max=pADXL355->y_ang;
	for(i=0;i<DATA_NUM-1;i++)
	{
		ADXL355_Data_Pro(&ADXL355_temp);
		x_buf[i] = ADXL355_temp.x_ang;
		y_buf[i] = ADXL355_temp.y_ang;
		if(x_min>x_buf[i]) x_min=x_buf[i];
		if(x_max<x_buf[i]) x_max=x_buf[i];
		if(y_min>y_buf[i]) y_min=y_buf[i];
		if(y_max<y_buf[i]) y_max=y_buf[i];
		pADXL355->x_ang += x_buf[i];
		pADXL355->y_ang += y_buf[i];
	}
	pADXL355->x_ang = (pADXL355->x_ang-x_min-x_max)/(DATA_NUM-2);
	pADXL355->y_ang = (pADXL355->y_ang-x_min-y_max)/(DATA_NUM-2);
	pADXL355->Scale = ADXL355_SCALE;

//	APP_LOG("X����ٶ�ֵ��%.2f \r\n",pADXL355->ax);
//	APP_LOG("Y����ٶ�ֵ��%.2f \r\n",pADXL355->ay);
//	APP_LOG("Z����ٶ�ֵ��%.2f \r\n\r\n",pADXL355->az);
//	APP_LOG("X����б�ǣ�%.2f \r\n",pADXL355->x_ang-SystemGlobalVar.TT_Offset.Angle_X);
//	APP_LOG("Y����б�ǣ�%.2f \r\n",pADXL355->y_ang-SystemGlobalVar.TT_Offset.Angle_Y);
	//APP_LOG("Z����б�ǣ�%.2f \r\n\r\n",pADXL355->z_ang);
}

float ADXL355_Read_Temp(void)
{
	uint16_t time=0;
	float ADXL355_Temp_f;
	struct ADXL355_STATUS_DEF statu;
	do{
			statu = ADXL355_Status_Read(); //��ȡ״̬
			if(statu.DATA_RDY)
			{
				ui32SensorT = SPI_ADXL355_READBYTE(XL355_TEMP2, 2);
				ADXL355_Temp_f = ((((float)ui32SensorT - ADXL355_TEMP_BIAS)) / ADXL355_TEMP_SLOPE) + 25.0;
				return ADXL355_Temp_f;
			}
			else
			{
				ADXL355_delay(100);
				time++;
				if(time==850)
				{
					APP_LOG("�¶�ֵ��ȡ����\r\n");
					return 0;
				}
			}
	}while(1);
}

/*
  �������ܣ� ��ȡADXL355���ٶ�����
  Reads the accelerometer data.
*/
void Shell_ADXL355_ACC_Read(struct ADXL355_Typedef *pADXL355)
{
		uint8_t time=0;
		pADXL355->Status = ADXL355_Status_Read(); //��ȡ״̬
		while(0==pADXL355->Status.DATA_RDY)
		{
			HAL_Delay(1);
			time++;
			if(time==100)
			{
				return;
			}
			pADXL355->Status = ADXL355_Status_Read(); //��ȡ״̬
		}
		//else if(pADXL355->Status.DATA_RDY)
		{
			ui32SensorX=SPI_ADXL355_READBYTE(XL355_XDATA3,3); //XL355_XDATA3ΪX��3���ݼĴ������׵�ַ  ui32SensorXΪ3���ֽڵ�����  00000000 xxxxxxxx xxxxxxxx xxxxxxxx
			ui32SensorY=SPI_ADXL355_READBYTE(XL355_YDATA3,3); //XL355_YDATA3ΪY��3���ݼĴ������׵�ַ
			ui32SensorZ=SPI_ADXL355_READBYTE(XL355_ZDATA3,3); //XL355_ZDATA3ΪZ��3���ݼĴ������׵�ַ
			ui32SensorT = SPI_ADXL355_READBYTE(XL355_TEMP2, 2);
			i32SensorX = ADXL355_Acceleration_Data_Conversion(ui32SensorX);     //��AD�ɼ��������ݽ���ת��
			i32SensorY = ADXL355_Acceleration_Data_Conversion(ui32SensorY);
			i32SensorZ = ADXL355_Acceleration_Data_Conversion(ui32SensorZ);		
		  
			pADXL355->Temp = ((((float)ui32SensorT - ADXL355_TEMP_BIAS)) / ADXL355_TEMP_SLOPE) + 25.0;
			pADXL355->Scale = ADXL355_SCALE;
			
			pADXL355->ax=(double)i32SensorX;
			pADXL355->ay=(double)i32SensorY;
			pADXL355->az=(double)i32SensorZ;
			double ax,ay,az;
			ax=(double)i32SensorX/ADXL355_SCALE;
			ay=(double)i32SensorY/ADXL355_SCALE;
			az=(double)i32SensorZ/ADXL355_SCALE;
			pADXL355->x_ang = atan(ax/sqrt(ay * ay + az * az)) * pi_ang ;
			pADXL355->y_ang = atan(ay/sqrt(ax * ax + az * az)) * pi_ang ;
			pADXL355->z_ang = atan(az/sqrt(ax * ax + ay * ay)) * pi_ang ;
		}
}

