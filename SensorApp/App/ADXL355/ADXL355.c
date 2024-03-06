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
  函数功能：通过SPI 向 ADXL355 WRITE一个字节的数据
  adress：地址 
  data  ：数据
  写一个字节的数据    
*/
void SPI_ADXL355_WRITE(uint8_t adress, uint8_t data)
{

	unsigned char writeadress;                                          //unsigned char没有符号位，因此能表示0~255
	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);    //CS = LOW  片选信号拉低
	
	writeadress = adress<<1;                                            //寄存器的地址7位 左移1位 右边补0表示写数据  即 xxxxxxx0共8位
	SPI_ReadWriteByte(&XL355_hspi,writeadress);		                                //write adress写入地址
	SPI_ReadWriteByte(&XL355_hspi,data);                                            //data to be written写入数据
	
	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET);		//CS = High	 片选信号拉高                                                     //Wait Time
}

 
/*
  函数功能：通过SPI 从 ADXL355读取n个字节的数据
  adress：地址 
  读取bytes个字节的数据    
*/
 uint32_t SPI_ADXL355_READBYTE(uint8_t adress,uint8_t bytes)
{

	uint32_t result;
	unsigned char readadress;

  HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);  //CS = LOW
	readadress = adress<<1;                                           //寄存器的地址7位 左移1位 
	readadress |= 0x01;                                               //寄存器的地址7位 左移1位 右边与1或表示读数据  即 xxxxxxx1共8位
	result=SPI_ReadWriteByte(&XL355_hspi,readadress);                             //read adress
	result=SPI_ReadWriteByte(&XL355_hspi,0xff);                                 //Read 8bit data读取一个字节的数据 8位 00000000 00000000 00000000 xxxxxxxx
	if(bytes==1) 
	{
		HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
		return result;
	}
	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 16bit data读取2个字节的数据 16位00000000 00000000 xxxxxxxx xxxxxxxx
	if(bytes==2) 
	{
		HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
		return result;
	}
	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 24bit data读取3个字节的数据 24位00000000 xxxxxxxx xxxxxxxx xxxxxxxx

	//HAL_Delay(1);			                                          //CS to SCLK set up time
	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
	//HAL_Delay(1);			                                          //Wait Time

	return result;
}
 /*
  函数功能：通过SPI 从 ADXL355读取一个字节的数据
  adress：地址 
  读取一个字节的数据    
*/
// unsigned char SPI_ADXL355_READ1BYTE(uint8_t adress)
//{

//	uint32_t result;
//	unsigned char readadress;
//	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);    //CS = LOW  片选信号拉低
////	HAL_Delay(1);						                               //CS to SCLK set up time
//	readadress = adress<<1;                                            //寄存器的地址7位 左移1位 
//	readadress |= 0x01;                                                //寄存器的地址7位 左移1位 右边与1或表示读数据  即 xxxxxxx1共8位
//	result=SPI_ReadWriteByte(&XL355_hspi,readadress);                              //read adress读地址
//  result=SPI_ReadWriteByte(&XL355_hspi,0xff);                                    //Read 8bit data (Send Dummy data 0xff)
//	
////  HAL_Delay(1);			                                           //CS to SCLK set up time
//	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET);     //CS = High	
////	HAL_Delay(1);				                                       //Wait Time

//	return result;
//}

///*
//  函数功能：通过SPI 从 ADXL355读取3个字节的数据
//  adress：地址 
//  读取3个字节的数据    
//*/
// uint32_t SPI_ADXL355_READ3BYTE(uint8_t adress)
//{

//	uint32_t result;
//	unsigned char readadress;

//    HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_RESET);  //CS = LOW
////	HAL_Delay(1);					                                  //CS to SCLK set up time
//	readadress = adress<<1;                                           //寄存器的地址7位 左移1位 
//	readadress |= 0x01;                                               //寄存器的地址7位 左移1位 右边与1或表示读数据  即 xxxxxxx1共8位
//	result=SPI_ReadWriteByte(&XL355_hspi,readadress);                             //read adress
//  result=SPI_ReadWriteByte(&XL355_hspi,0xff);                                   //Read 8bit data读取一个字节的数据 8位 00000000 00000000 00000000 xxxxxxxx
//	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 16bit data读取2个字节的数据 16位00000000 00000000 xxxxxxxx xxxxxxxx
//	result=result<<8|SPI_ReadWriteByte(&XL355_hspi,0xff);                         //Read 24bit data读取3个字节的数据 24位00000000 xxxxxxxx xxxxxxxx xxxxxxxx

//	//HAL_Delay(1);			                                          //CS to SCLK set up time
//	HAL_GPIO_WritePin(XL355_SPI_CS_PORT,XL355_SPI_CS,GPIO_PIN_SET); 	  //CS = High	
//	//HAL_Delay(1);			                                          //Wait Time

//	return result;
//}

/*
  函数功能: ADXL355 传感器开始工作
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
  函数功能: 加速计进入待机模式.
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
  函数功能: 复位加速计.
**/
void ADXL355_Reset_Sensor(void)
{
  SPI_ADXL355_WRITE(XL355_RESET, 0x52);  //写入代码 0x52 即复位器件，类似于上电复位(POR)
	HAL_Delay(100);
}

/**
  函数功能: XYZ 轴失调调整寄存器读写.
在所有其他信号处理之后，将失调加到 x/y/z 轴数据上。数据采用二进制补码
格式。 OFFSET_X[15:0]的重要性与 XDATA[19:4]的重要性一致。
**/
void ADXL355_Offset_RW(uint8_t axle,int16_t *Val,uint8_t mode)
{
   int16_t ui16temp;
	 if(mode==1)  //读
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
	 else //if(mode==0)  //写
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
  函数功能: 活动阈值、活动计数寄存器寄存器读写.
活动检测阈值。加速度幅度必须大于 ACT_THRESH 才能触发活动计数器。
ACT_THRESH 是无符号幅度。 ACT_TRESH[15:0]的重要性与 XDATA、 YDATA
和 ZDATA [18:3]的重要性一致。检测活动所需的超过阈值的连续事件数
**/
void ADXL355_ACT_THRESH_RW(uint16_t *ACT_Val,uint8_t *ACT_Count,uint8_t mode)
{
   uint16_t ui16temp;
	 if(mode==1)  //读
	 {
			ui16temp = (uint8_t)SPI_ADXL355_READBYTE(XL355_ACT_THRESH_H,1);
			ui16temp	<<=	8;
			ui16temp |= (uint8_t)SPI_ADXL355_READBYTE(XL355_ACT_THRESH_L,1);
		 *ACT_Val = ui16temp;
		 *ACT_Count = (uint8_t)SPI_ADXL355_READBYTE(XL355_ACT_COUNT,1);
   }
	 else //if(mode==0)  //写
	 {
		 ui16temp = *ACT_Val;
		 SPI_ADXL355_WRITE(XL355_ACT_THRESH_H, ui16temp>>8);
		 SPI_ADXL355_WRITE(XL355_ACT_THRESH_L,ui16temp&0xff);
		 SPI_ADXL355_WRITE(XL355_ACT_COUNT,*ACT_Count);
   }
}

/**
  函数功能: 活动使能寄存器.
	axle_bit:
	bit[7:3]  保留 0x0 
	bit 2 ACT_Z  	1：Z轴数据是活动检测算法的组成部分   0：Z轴数据不是活动检测算法的组成部分
	bit 1 ACT_Y  	1：Y轴数据是活动检测算法的组成部分	 0：Y轴数据不是活动检测算法的组成部分
	bit 0 ACT_X   1：X轴数据是活动检测算法的组成部分	 0：X轴数据不是活动检测算法的组成部分
**/
void ADXL355_ACT_EN_W(uint8_t axle_bit)
{
		SPI_ADXL355_WRITE(XL355_ACT_EN, axle_bit&0x07);
}

//读取状态
struct ADXL355_STATUS_DEF ADXL355_Status_Read(void)
{
	uint8_t status;
	struct ADXL355_STATUS_DEF status_res;
	status = SPI_ADXL355_READBYTE(XL355_STATUS,1);
	status_res = *(struct ADXL355_STATUS_DEF*)&status;
	return status_res;
}


/*
  函数功能: 设置ADXL355活动阈值 
  配置相关寄存器  XL355_RANGE、XL355_SYNC等 
  判断传感器是否连接正常
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
		APP_LOG("XL355唤醒角度：%.2f\r\n",sin_val);
		//APP_LOG("sin%.2f：%.2f\r\n",sin_val,sin(sin_val*PI/180));
		ACT_Val = sin(sin_val*PI/180)*32051;  //32051 = 1000000/3.9/8(根据数据手册计算)
		//APP_LOG("XL355活动阈值：%d\r\n",ACT_Val);
		ADXL355_ACT_THRESH_RW(&ACT_Val,&ACT_Count,0);
		ADXL355_ACT_EN_W(0x03); 		//活动使能寄存器	
	}
	else
	{
		APP_LOG("XL355不启用倾角唤醒\r\n");
		ADXL355_ACT_EN_W(0x00); 		//不使能 活动使能寄存器	
	}
}

/*
  函数功能: ADXL355 初始化
  配置相关寄存器  XL355_RANGE、XL355_SYNC等 
  判断传感器是否连接正常
*/
void ADXL355_Init(void)
{
  int16_t temp_int16;
	ADXL355_Stop_Sensor();
	HAL_Delay(100);
	//SPI_ADXL355_WRITE(XL355_RANGE,0x81);  /* Set sensor range within RANGE register */ //设置为 1000 0001 高速模式  ±2g
	#if ADXL_RANGE == 2
		SPI_ADXL355_WRITE(XL355_RANGE, 0xC1);    /* I2C高速模式,NT1和INT2为高电平有效, 范围±2 g*/
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
	SPI_ADXL355_WRITE(XL355_FILTER,0x06); //滤波器设置寄存器,未使能高通滤波器,低通滤波62.5 Hz 和 15.625 Hz
	SPI_ADXL355_WRITE(XL355_INT_MAP,0x08);//INT1 上的活动中断使能
	
	uint8_t ui8test  = SPI_ADXL355_READBYTE(XL355_DEVID_AD,1);  /* 0xAD Read the ID register */ 
	uint8_t ui8test2 = SPI_ADXL355_READBYTE(XL355_DEVID_MST,1);  /* 0x1D Read the ID register */
	uint8_t ui8test3 = SPI_ADXL355_READBYTE(XL355_PARTID,1);     /* 0xED Read the ID register */
	uint8_t ui8test4 = SPI_ADXL355_READBYTE(XL355_REVID,1);      /* 0x03 Read the ID register */
  
	temp_int16 = SystemGlobalVar.ADXL355.X_Offset;
	ADXL355_Offset_RW(0,&temp_int16,0);	 //X轴失调调整寄存器写
	temp_int16 = SystemGlobalVar.ADXL355.Y_Offset;
	ADXL355_Offset_RW(1,&temp_int16,0);	 //Y轴失调调整寄存器写
	temp_int16 = SystemGlobalVar.ADXL355.Z_Offset;
	ADXL355_Offset_RW(2,&temp_int16,0);	 //Z轴失调调整寄存器写
  	
  ADXL355_ACT_Set();
	ADXL355_Start_Sensor();
	/* 验证ADXL355设备是否连接正常*/
		if(SPI_ADXL355_READBYTE(XL355_PARTID,1)==0xED)	
	  {
		  APP_LOG("ADXL355 Init OK!\r\n");
//		  APP_PRINTF("XL355_DEVID_AD:0x%x  XL355_DEVID_MST:%x\r \n",ui8test,ui8test2);
//		  APP_PRINTF("XL355_PARTID:0x%x  XL355_REVID:0x%x\r \n",ui8test3,ui8test4);
	  }
	  else
	  {
		  APP_LOG("ADXL355 Error！\r\n");
	  }	
		
}

/*
  函数功能： ADXL355加速度数据转换
*/
int32_t ADXL355_Acceleration_Data_Conversion (uint32_t ui32SensorData)
{
   int32_t volatile i32Conversion = 0;

	//加速度数据为20位 读取的3个字节（24位）加速度数据其中最低4位无效[3:0]     00000000 xxxxxxxx xxxxxxxx xxxxxxxx
   ui32SensorData = (ui32SensorData  >> 4);                  //右移4位
   ui32SensorData = (ui32SensorData & 0x000FFFFF);           //得到32位真实的数据 00000000 0000xxxx xxxxxxxx xxxxxxxx
   if((ui32SensorData & 0x00080000)  == 0x00080000)          //       如果数据是  00000000 00001xxx xxxxxxxx xxxxxxxx
	 {
         i32Conversion = (ui32SensorData | 0xFFF00000);      //      则数据转换成 11111111 11111xxx xxxxxxxx xxxxxxxx
				//i32Conversion = 0xFFFFFFFF-i32Conversion+1;
	 }
   else
	 {
          i32Conversion = ui32SensorData;                   //      否则数据转换成 00000000 0000xxxx xxxxxxxx xxxxxxxx
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
			statu = ADXL355_Status_Read(); //读取状态
			if(statu.DATA_RDY)
			{
				ui32SensorX=SPI_ADXL355_READBYTE(XL355_XDATA3,3); //XL355_XDATA3为X轴3数据寄存器的首地址  ui32SensorX为3个字节的数据  00000000 xxxxxxxx xxxxxxxx xxxxxxxx
				ui32SensorY=SPI_ADXL355_READBYTE(XL355_YDATA3,3); //XL355_YDATA3为Y轴3数据寄存器的首地址
				ui32SensorZ=SPI_ADXL355_READBYTE(XL355_ZDATA3,3); //XL355_ZDATA3为Z轴3数据寄存器的首地址
				i32SensorX = ADXL355_Acceleration_Data_Conversion(ui32SensorX);     //将AD采集到的数据进行转换
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
					APP_LOG("X、Y、Z轴加速度值读取错误\r\n");
					return 0;
				}
			}
	}while(1);
}

/*
  函数功能： ADXL355数据扫描
  Reads the accelerometer data.
  发送ADXL355采集的数据   
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

//	APP_LOG("X轴加速度值：%.2f \r\n",pADXL355->ax);
//	APP_LOG("Y轴加速度值：%.2f \r\n",pADXL355->ay);
//	APP_LOG("Z轴加速度值：%.2f \r\n\r\n",pADXL355->az);
//	APP_LOG("X轴倾斜角：%.2f \r\n",pADXL355->x_ang-SystemGlobalVar.TT_Offset.Angle_X);
//	APP_LOG("Y轴倾斜角：%.2f \r\n",pADXL355->y_ang-SystemGlobalVar.TT_Offset.Angle_Y);
	//APP_LOG("Z轴倾斜角：%.2f \r\n\r\n",pADXL355->z_ang);
}

float ADXL355_Read_Temp(void)
{
	uint16_t time=0;
	float ADXL355_Temp_f;
	struct ADXL355_STATUS_DEF statu;
	do{
			statu = ADXL355_Status_Read(); //读取状态
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
					APP_LOG("温度值读取错误\r\n");
					return 0;
				}
			}
	}while(1);
}

/*
  函数功能： 读取ADXL355加速度数据
  Reads the accelerometer data.
*/
void Shell_ADXL355_ACC_Read(struct ADXL355_Typedef *pADXL355)
{
		uint8_t time=0;
		pADXL355->Status = ADXL355_Status_Read(); //读取状态
		while(0==pADXL355->Status.DATA_RDY)
		{
			HAL_Delay(1);
			time++;
			if(time==100)
			{
				return;
			}
			pADXL355->Status = ADXL355_Status_Read(); //读取状态
		}
		//else if(pADXL355->Status.DATA_RDY)
		{
			ui32SensorX=SPI_ADXL355_READBYTE(XL355_XDATA3,3); //XL355_XDATA3为X轴3数据寄存器的首地址  ui32SensorX为3个字节的数据  00000000 xxxxxxxx xxxxxxxx xxxxxxxx
			ui32SensorY=SPI_ADXL355_READBYTE(XL355_YDATA3,3); //XL355_YDATA3为Y轴3数据寄存器的首地址
			ui32SensorZ=SPI_ADXL355_READBYTE(XL355_ZDATA3,3); //XL355_ZDATA3为Z轴3数据寄存器的首地址
			ui32SensorT = SPI_ADXL355_READBYTE(XL355_TEMP2, 2);
			i32SensorX = ADXL355_Acceleration_Data_Conversion(ui32SensorX);     //将AD采集到的数据进行转换
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

