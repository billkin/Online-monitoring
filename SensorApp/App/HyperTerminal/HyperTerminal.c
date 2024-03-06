/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "HyperTerminal.h"
//#include "flash_if.h"
//#include "ymodem.h"
#include "stdio.h"
//#include "timer.h"
#include "SystemGlobalVar.h"
#include "timestamp.h"
#include <string.h>
#include "ADXL355.h"
#include "stmflash.h"
#include "E28_2G4T12S_LoRa.h"

uint8_t Main_Menu(void);								//主菜单
void ChangePassword(void);							//更改密码
void SensorDeviceConfig(void);   			//传感器终端配置
void CycleSamplingConfig(void);				//主采样周期配置
void SystemTimeConfig(void);						//系统时间配置
void NetworkModelConfig(void);					//网络模式配置
void NetworkParameterConfig(void);			//网络参数配置
void USART_BaudRateConfig(void);				//串口波特率配置
void SystemsParameterDisplay(void);     //系统参数显示
void SystemFunctiongConfig(void);				//系统功能配置
void SystemFactoryResetConfig(void);		//恢复出厂设置
void ClearingHistoricalDataConfig(void);//清空历史数据
//void TensionCorrectionConfig(void);			//拉力修正值
//void SimScardConfig(void);							//SIM卡号
void InclinationConfig(void);						//杆塔倾斜配置
void LoRaConfig(void);										//LoRa配置

void TiltSensorsConfig(void);		//5.倾角传感器设置

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
uint8_t HyperTerminal_GetKey(uint16_t timeout,uint8_t res)
{
  uint8_t key = 0;
	if(timeout==0)   //不超时等待
	{
		/* Waiting for user input */
		while (1)
		{
			if ( __HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_RXNE) )
			{
				key = (uint8_t)Shell_huart.Instance->RDR;
//				while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕
//				Shell_huart.Instance->TDR = key;
				return key;
			}
		}	
	}
	else		//超时等待
	{
		while (timeout--)
		{
			if ( __HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_RXNE) )
			{
				key = (uint8_t)Shell_huart.Instance->RDR;
//				while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕
//				Shell_huart.Instance->TDR = key;
				return key;
			}
		}
	}
  return key;
}
/**
  * @brief  WaitInput
* @param  pbuff:接收区,len：接收长度, mode：bit 0: 0 接收全部字符， 1 接收数字字符 
																			mode：bit 7: 0 正常显示，     1 *号显示
  * @retval 0：正确；1：回车  2：错误返回
  */
uint8_t WaitInput(uint8_t *pbuff,uint8_t len,uint8_t mode)
{
	uint8_t key,i=0;
	memset(pbuff, 0, len);
//	printf("请输入：");
	while(1)
	{
		key = HyperTerminal_GetKey(0,0);  //不超时等待用户输入
		if(mode&0x0f)
		{
			if(((key>='0') && (key<='9'))|| (key=='-')|| (key=='.'))  //
			{
        while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕
        if(mode&0x80)  Shell_huart.Instance->TDR = '*';
        else Shell_huart.Instance->TDR = key;
				pbuff[i++]=key; if(key=='-'&& i>1) return 2;
				if(i==len)   //最多输入len个字符密码
				{
					return 0;
				}
			}
			else if(key=='\b')   //删除键
			{
				if(i>0)        //已经有字符输入
				{
					i--;
					pbuff[i]= 0 ;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕
          Shell_huart.Instance->TDR = key;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕   
          Shell_huart.Instance->TDR = ' ';
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕
          Shell_huart.Instance->TDR = key;
				}
			}
			else if(key==0x0D)   //回车
			{
					return 1;
			}
			else
				return 2;
		}
		else
		{
			if((key>=0x20) && (key<=0x7e))  //
			{
				while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0){;}	//循环发送,直到发送完毕
        if(mode&0x80)  Shell_huart.Instance->TDR = '*';
        else Shell_huart.Instance->TDR = key;
        pbuff[i++]=key;
				if(i==len)   //最多输入len个字符密码
				{	
					return 0;
				}
			}
			else if(key=='\b')   //删除键
			{
				if(i>0)        //已经有字符输入
				{
					i--;
					pbuff[i]= 0 ;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕
          Shell_huart.Instance->TDR = key;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕   
          Shell_huart.Instance->TDR = ' ';
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//循环发送,直到发送完毕
          Shell_huart.Instance->TDR = key;
				}
			}
			else if(key==0x0D)   //回车
			{
					return 1;
			}
			else
				return 2;
		}
	}
}

/**
  * @brief  WaitInputNum
* @param  Num:接收到的数据，size:允许输入的最大位数
* @retval 0:没有数据输入，	1:有数据输入
  */
uint8_t WaitInputNum(float *Num,uint8_t size)
{
	uint8_t res,i=0;
	float temp=0;
	uint8_t pbuf[8];
	if(size>7)	size=7;
	memset(pbuf, 0, 8);
	res = WaitInput(pbuf,size,1);
	if(res == 0||res == 1)
	{
		res = 0;
		if(pbuf[0] == '-') i=1;
		while(pbuf[i]!=0)
		{
			if(pbuf[i]=='.')
			{
				if(res==1) return 0; //2个小数点，出错处理
				res=1;
			}
			else if(res==0)
			{
				temp *= 10;
				temp += (pbuf[i]-'0');
			}
			else
			{
				temp += (float)(pbuf[i]-'0')/10;
			}
			i++;
		}
		if(pbuf[0] == '-') temp = 0 - temp;
		*Num = temp;
		return 1;
	}
	return 0;
}

/**
  * @brief  WaitInputPassWrod
  * @param  允许输入密码次数
  * @retval 0：密码正确；1：密码错误
  */
uint8_t WaitInputPassWrod(uint8_t times)
{
	uint8_t i,res = 0;
	char password[8];
	while(times--)
	{
		printf("\r\n请输入密码：");
    res = WaitInput((uint8_t*)password,4,0x80);
		if((res==0) || (res==1))
		{
			if(strcmp(password,"kyne")==0) return 0;  //超级密码
			else 
			{
				res = 0;
				for(i=0;i<4;i++)
					if(password[i]!=SystemGlobalVar.PassWord[i]) {res=1;break;}
				if(res==0) return 0;
			}
		}
    else
      printf("\r\n密码错误");
    printf("\r\n\r\n");
	}
	return 1;
}
/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
unsigned char HyperTerminal_Menu(unsigned short timeout)
{
  uint8_t key = 0;
	//uint8_t i=0;
  printf("\r\n\n");
	printf("************************系统交互界面****************************\r\n");
	printf("*               深圳量云能源网络科技有限公司                   *\r\n");
	printf("*                      在线监测系统                            *\r\n");
	printf("* 名称:系统配置工具                                            *\r\n");
//	printf("* 编写:                                                        *\r\n");
//	printf("* 邮箱:                                                        *\r\n");
	printf("* 版本:Sensor app V1.0                                         *\r\n");
	printf("* 日期:2022.01                                                 *\r\n");
	printf("****************************************************************\r\n\r\n");
	printf("3秒后程序跳过,按s键进入交互...\r\n\n");
	/* Receive key */
	uint32_t RunTime = HAL_GetTick();
	while (1)
	{
		if(HAL_GetTick()-RunTime>=timeout)
			return 1; 	//超时进入系统
		key = HyperTerminal_GetKey(10,0);
    if (key == 's'||key == 'S')          //进入配置界面
    {
			if(WaitInputPassWrod(3))  return 1;  //输入3次密码，不正确就退出
      else
        break;
    }
    else if (key > 0) 
		{
			//printf("\r\n跳过配置界面\r\n");
			return 1;   //有键盘输入
		}
	}
	while(1)
	{
		if(Main_Menu()==0) break;   //没有修改配置则进入系统
		HAL_Delay(1);
	}
	return 0;
}


///**
//  * @brief  主菜单
//  * @param  None
//  * @retval None
//  */
uint8_t Main_Menu(void)
{
	uint8_t key = 0;
	uint8_t res=0xff;
	printf("\r\n\r\n************************主菜单****************************\r\n");
	printf("1.更改密码\r\n");
	printf("2.传感器终端配置\r\n");
	printf("3.采样周期设置\r\n");
	printf("4.LoRa模块配置\r\n");
	printf("5.倾角传感器零点设置\r\n");
	printf("6.系统时间,硬件重启时间\r\n");
	printf("7.实时测量显示\r\n");
	printf("8.系统参数显示\r\n");
	printf("b.恢复出厂配置\r\n");
	printf("0.保存退出\r\n\r\n");
   
  printf("\r\n请输入：");
	key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
  printf("\r\n\n");
	switch(key)
	{
		case '0':
			res = 0;			//
			printf("保存修改?(y/n):");
			key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
		  if(key=='y'||key=='Y') 
			{
					printf("\r\n\r\n保存修改...\r\n");	
					SystemConfigSave();//保存配置
				  HAL_Delay(100);
				  HAL_NVIC_SystemReset();  //重启系统
			}
			break;
		case '1':       ChangePassword();	      //更改密码
			res = 1;
			break;
		case '2':  			SensorDeviceConfig();   //传感器终端配置
			res = 2;
			break;
		case '3':				CycleSamplingConfig();   //主采样周期配置
			break;
		case '4':				LoRaConfig();   //LoRa模块配置
			break;	
		case '5':				TiltSensorsConfig();				//5.倾角传感器零点设置
			break;
		case '6':				SystemTimeConfig();				//系统时间配置
			break;
		case '7':				//实时测量
			Shell_SensorStatusUp();				 				//传感器状态量
			Shell_SensorMonitoringDataUp();				 //监测数据
			break;
//		case '5':				NetworkModelConfig();		  //网络模式配置
//			break;
//		case '6':				NetworkParameterConfig();			//网络参数配置		
//			break;
//		case '7':				USART_BaudRateConfig();				//串口波特率配置	
//			break;
//		
//			break;
		case '8':				SystemsParameterDisplay();       //系统参数显示
			break;
//		case 'a':				SystemFunctiongConfig();				//系统功能配置	
//			break;
		case 'B':				
		case 'b':				SystemFactoryResetConfig();		  //恢复出厂设置		
			break;
//		case 'c':				ClearingHistoricalDataConfig();//清空历史数据		
//			break;
//		case 'd':				TensionCorrectionConfig();			//拉力修正值		
//			break;
//		case 'e':				SimScardConfig();							//SIM卡号
//			break;
//		case 'f':				
//			break;
//		case 'g':				
//			break;
//		case 'h':				InclinationConfig();						//杆塔倾斜配置			
//			break;
//		case 'o':				
//			break;
		default:break;
	}
	return res;
}

/**
  * @brief  更改密码
  * @param  None
  * @retval None
  */
void ChangePassword()
{
	uint8_t res,i;
	char password[4];
	printf("\r\n\r\n========================================================\r\n");
	printf("1.更改密码\r\n");
	printf("========================================================\r\n");
	printf("\r\n请输入（4个任意字符，Esc键退出）：");
	res = WaitInput((uint8_t*)password,4,0x80);
	if(res==0)
	{
			for(i=0;i<4;i++)
			{
				if(password[i]<0x20 || password[i]>0x7F)
				{
					res = 1;  break;
				}
			}	
			if(res==0)
			{
				for(i=0;i<4;i++)
					SystemGlobalVar.PassWord[i]=password[i];
			}
			else
			{
				printf("\r\n密码设置失败！\r\n");
				HAL_Delay(1000);
			}
	}	
}

///**
//  * @brief  传感器终端配置
//  * @param  None
//  * @retval None
//  */
void SensorDeviceConfig()
{
	uint8_t key;
	uint8_t pbuff[8];
	uint8_t err=0;
	float temp;
	do
	{
		printf("\r\n\r\n========================================================\r\n");
		printf("2.传感器终端配置\r\n");
		printf("========================================================\r\n");
		printf("1.厂商代码:%d\r\n",SystemGlobalVar.SensorID.ManufacturerID);
		printf("2.版本标签:%d\r\n",SystemGlobalVar.SensorID.VersionLetter);
		printf("3.版本号:%d\r\n",SystemGlobalVar.SensorID.VersionNumber);
		//printf("4.产品序列号:%d\r\n",SystemGlobalVar.SensorID.SerialNumber);
		NumToSerialNumberString(SystemGlobalVar.SensorID.SerialNumber,pbuff,7);
		pbuff[7] = 0;
		printf("4.产品序列号:%s\r\n",pbuff);
		printf("\r\n请输入：");
		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("1.厂商代码(0~65535):");
				if(WaitInputNum(&temp,5))
				{
//					if(((pbuff[0]>='0'&&pbuff[0]<='9')||(pbuff[0]>='A'&&pbuff[0]<='Z')||(pbuff[0]>='a'&&pbuff[0]<='z'))&&((pbuff[1]>='0'&&pbuff[1]<='9')||(pbuff[1]>='A'&&pbuff[1]<='Z')||(pbuff[1]>='a'&&pbuff[1]<='z')))
//						SystemGlobalVar.SensorID.ManufacturerID = (pbuff[0]<<8)|pbuff[1];
					if(temp>=0 &&temp<65536)SystemGlobalVar.SensorID.ManufacturerID=temp;
					else
					{
						printf("\r\n输入错误！\r\n");
						HAL_Delay(1000);
					}
				}
				else
				{
					printf("\r\n输入错误！\r\n");
					HAL_Delay(1000);
				}
			break;
			case '2':
				printf("2.版本标签(1~26):");
				if(WaitInputNum(&temp,2))
				{
					if(temp>0 &&temp<32)
						SystemGlobalVar.SensorID.VersionLetter = temp;
					else
						printf("\r\n输入错误\r\n");
				}		
			break;
			case '3':
				printf("3.版本号(0~63):");
				if(WaitInputNum(&temp,2))
				{
					if(temp>=0 && temp<64)
						SystemGlobalVar.SensorID.VersionNumber = temp;	
					else
						printf("\r\n输入错误\r\n");
				}
			break;
			case '4':
				printf("4.产品序列号(0000000~2097151):");
				if(WaitInputNum(&temp,7))
				{
					if(temp<2097152)
						SystemGlobalVar.SensorID.SerialNumber = temp;
					else
						printf("\r\n输入错误\r\n");
				}
			break;
			default:err=1;break;
		}
	}while(err==0);
}


//主采样周期配置
void CycleSamplingConfig()
{
	uint8_t key;
	uint8_t err=0;
	float temp;
	do
	{
		printf("\r\n\r\n========================================================\r\n");
		printf("3.采样周期\r\n");
		printf("========================================================\r\n");
		printf("1.杆塔倾斜(分钟)：%d\r\n",SystemGlobalVar.SamplingPeriod/60);
		printf("其它任意键返回\r\n");
		
		printf("\r\n请输入：");
		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("\r\n1.杆塔倾斜采样周期(1~240分钟/整数):"); 
				if(WaitInputNum(&temp,3))
				{
					if(temp>0 && temp<=240)
						SystemGlobalVar.SamplingPeriod = temp*60;	
					else
						printf("\r\n输入错误");
				}
				break;
			default:err=1;
				break;	
		}
	}while(err==0);
	
}

///**
//  * @brief  传感器终端配置
//  * @param  None
//  * @retval None
//  */
void SensorAlarmLimitConfig()
{
	uint8_t key;
	//uint8_t pbuff[4];
	uint8_t err=0;
	float temp;
	do
	{
		printf("\r\n\r\n========================================================\r\n");
		printf("报警值设置\r\n");
		printf("========================================================\r\n");
		printf("1.报警值(±):%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit);
		//printf("2.下限值:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
		printf("其他键返回\r\n");
		
		printf("\r\n请输入：");
		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("1.报警值(0~60°,0表示不报警):");
				if(WaitInputNum(&temp,2))
				{
					if(temp<90)
						SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit = temp;
					else
						printf("\r\n输入错误\r\n");
				}	
			break;
//			case '2':
//				printf("2.下限值(0~60°，0表示不报警):-");
//				if(WaitInputNum(&temp,2))
//				{
//					if(temp<90)
//						SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit = (0-(float)temp);
//					else
//						printf("\r\n输入错误\r\n");
//				}		
//			break;
			default:err=1;break;	
		}
	}while(err==0);
}

//杆塔倾斜复位值（绝对角度值）
//TiltSensorsConfig()中调用
void TiltAbsoluteSet(float Angle_x,float Angle_y)
{
	uint8_t key;
	uint8_t err=0;
	float temp;
	do
	{
		printf("\r\n\r\n【杆塔倾斜修正（绝对角度值-60~+60°)】\r\n");			
		printf("1.顺线倾斜绝对角度值：%.2f \r\n",Angle_x-SystemGlobalVar.TT_Offset.Angle_X);					// 4 顺线倾斜角偏移值（浮点数，精确到小数点后 2 位，单位：°）
		printf("2.横向倾斜绝对角度值：%.2f \r\n",Angle_y-SystemGlobalVar.TT_Offset.Angle_Y);			// 4 横向倾斜角偏移值
		printf("其他返回\r\n\r\n");
		printf("请输入\r\n");
		key = HyperTerminal_GetKey(0,1);  					//不超时等待用户输入
    printf("\r\n\r\n");
		switch(key)
		{
				case '1':
					printf("顺线倾斜绝对角度值:");
					if(WaitInputNum(&temp,5))
					{
						if(temp >= -60 && temp <= 60)
						{
							SystemGlobalVar.TT_Offset.Angle_X = Angle_x-temp;
						}
						else
							printf("角度超出范围，请重新配置");
					}
					break;
				case '2':
					printf("横向倾斜绝对角度值:");
					if(WaitInputNum(&temp,5))
					{
						if(temp >= -60 && temp <= 60)
						{
							SystemGlobalVar.TT_Offset.Angle_Y = Angle_y-temp;
						}
						else
							printf("角度超出范围，请重新配置");
					}
					break;
			default:	err=1;	break;	
		}
	}while(err==0);
}

//5.倾角传感器设置
void TiltSensorsConfig()
{
	uint8_t key;
	uint8_t err=0;
//	int16_t ax_off,ay_off,az_off;
	//struct ADXL355_Typedef ADXL355_ACC;
	I1_TowerTilted_StructDef pVals;
	do
	{
		printf("\r\n\r\n========================================================\r\n");
		printf("5.倾角传感器零点设置\r\n");
		printf("========================================================\r\n");
		//Shell_ADXL355_ACC_Read(&ADXL355_ACC);   //读取传感器加速度值、角度值
		GetTowerTiltedVals(&pVals);
		printf("【传感器倾角值（相对零点）】\r\n");
//		printf("倾斜度偏移值：%.1f \r\n",SystemGlobalVar.TT_Offset.Inclination);				// 4 倾斜度偏移值（浮点数，精确到小数点后 1 位，单位： mm/m）
//		printf("顺线倾斜度偏移值：%.1f \r\n",SystemGlobalVar.TT_Offset.Inclination_X);	// 4 顺线倾斜度偏移值（浮点数，精确到小数点后 1 位，单位：mm/m）
//		printf("横向倾斜度偏移值：%.1f \r\n",SystemGlobalVar.TT_Offset.Inclination_Y);	// 4 横向倾斜度偏移值（浮点数，精确到小数点后 1 位，单位：mm/m）
		printf("1.顺线倾斜角：%.2f°\r\n",pVals.Angle_X);					// 4 顺线倾斜角偏移值（浮点数，精确到小数点后 2 位，单位：°）
		printf("2.横向倾斜角：%.2f°\r\n\r\n",pVals.Angle_Y);			// 4 横向倾斜角偏移值
		
//		printf("【实时测量值】\r\n");
////		printf("1.X轴加速度值：%.2fg \r\n",ADXL355_ACC.ax/ADXL355_ACC.Scale);
////		printf("2.Y轴加速度值：%.2fg \r\n",ADXL355_ACC.ay/ADXL355_ACC.Scale);
////		printf("3.Z轴加速度值：%.2fg \r\n",ADXL355_ACC.az/ADXL355_ACC.Scale);
//		printf("1.顺线倾斜角：%.2f \r\n",ADXL355_ACC.x_ang);				// 4 顺线倾斜角偏移值（浮点数，精确到小数点后 2 位，单位：°）
//		printf("2.横向倾斜角：%.2f \r\n\r\n",ADXL355_ACC.y_ang);		// 4 横向倾斜角偏移值
		
//		ADXL355_Offset_RW(0,&ax_off,1);	 //X轴失调调整寄存器读.
//		ADXL355_Offset_RW(1,&ay_off,1);	 //Y轴失调调整寄存器读.
//		ADXL355_Offset_RW(2,&az_off,1);	 //Z轴失调调整寄存器读.
//		
//		printf("【加速度修正值】\r\n");
//		printf("1.X轴加速度修正值：%d \r\n",ax_off);
//		printf("2.Y轴加速度修正值：%d \r\n",ay_off);
//		printf("3.Z轴加速度修正值：%d \r\n\r\n",az_off);
		
		Shell_SensorAlarmLimitDataRequest();				 //报警上下限值查询
		
		
		printf("\r\n\r\n修改请输入：\r\n");
		printf("1:传感器相对零点设置\r\n");
		printf("2:顺线、横向倾斜角报警上下限值设置\r\n");
//		printf("3:X轴加速度修正值(修正前应将传感器X轴正向向上垂直于水平面,暂不支持修改)\r\n");
//		printf("4:Y轴加速度修正值(修正前应将传感器Y轴正向向上垂直于水平面,暂不支持修改)\r\n");
//		printf("5:Z轴加速度修正值(修正前应将传感器Z轴正向向上垂直于水平面,传感器水平放置时加速度值接近1g,偏差过大可以修正)\r\n");
		printf("a.传感器复位(初始化)\r\n");
		printf("请输入\r\n");
		key = HyperTerminal_GetKey(0,1);  					//不超时等待用户输入
    printf("\r\n\r\n");
		switch(key)
		{
				case '1':
				{
					#if 0   //绝对零点设置
					TiltAbsoluteSet(ADXL355_ACC.x_ang,ADXL355_ACC.y_ang);
					#else		//相对零点设置
					SystemGlobalVar.TT_Offset.Angle_X = 0;
					SystemGlobalVar.TT_Offset.Angle_Y = 0;
					GetTowerTiltedVals(&pVals);
					SystemGlobalVar.TT_Offset.Angle_X =	pVals.Angle_X;							// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
					SystemGlobalVar.TT_Offset.Angle_Y =	pVals.Angle_Y;							// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
//					Shell_ADXL355_ACC_Read(&ADXL355_ACC);   //读取传感器加速度值、角度值
//					SystemGlobalVar.TT_Offset.Angle_X =	ADXL355_ACC.x_ang;							// 4 顺线倾斜角（浮点数，精确到小数点后 2 位，单位：°）
//					SystemGlobalVar.TT_Offset.Angle_Y =	ADXL355_ACC.y_ang;							// 4 横向倾斜角（浮点数，精确到小数点后 2 位，单位：°）
					#endif
				}
				break;
			case '2':
				{
					SensorAlarmLimitConfig();
				}				
				break;
			#if 0
			case '3':
				printf("暂不支持修改\r\n");
//				ax_off = 0;
//				ADXL355_Offset_RW(0,&ax_off,0);	 //X轴失调调整寄存器写
//				HAL_Delay(100);
//				Shell_ADXL355_ACC_Read(&ADXL355_ACC);	  
//				ax_off = (((ADXL355_ACC.ax - ADXL355_ACC.Scale)/16));
//				SystemGlobalVar.ADXL355.X_Offset = ax_off;
//				ADXL355_Offset_RW(0,&ax_off,0);	 //Z轴失调调整寄存器写..
				break;
			case '4':
				printf("暂不支持修改\r\n");
//				ay_off = 0;
//				ADXL355_Offset_RW(1,&ay_off,0);	 //X轴失调调整寄存器写
//				HAL_Delay(100);
//				Shell_ADXL355_ACC_Read(&ADXL355_ACC);	  
//				ay_off = (((ADXL355_ACC.ay - ADXL355_ACC.Scale)/16));
//				SystemGlobalVar.ADXL355.Y_Offset = ay_off;
//				ADXL355_Offset_RW(1,&ay_off,0);	 //Z轴失调调整寄存器写..
			  break;
			case '5':
				az_off = 0;
				ADXL355_Offset_RW(2,&az_off,0);	 //X轴失调调整寄存器写
				HAL_Delay(100);
				Shell_ADXL355_ACC_Read(&ADXL355_ACC);
//				if(ADXL355_ACC.az>ADXL355_ACC.Scale) 	  
				az_off = (((ADXL355_ACC.az - ADXL355_ACC.Scale)/16));
				SystemGlobalVar.ADXL355.Z_Offset = az_off;
				ADXL355_Offset_RW(2,&az_off,0);	 //Z轴失调调整寄存器写.
			  break;
			#endif
			case 'A':
			case 'a':
				{
					SystemGlobalVar.ADXL355.X_Offset = 0;
					SystemGlobalVar.ADXL355.Y_Offset = 0;
					SystemGlobalVar.ADXL355.Z_Offset = 0;
					ADXL355_Reset_Sensor();
					
					ADXL355_Start_Sensor();
					SystemConfigSave();      //保存配置
				}				
				break;
			default:	err=1;	break;	
		}
	}while(err==0);
}


void LoRaConfig(void)
{
	uint8_t key;
	uint8_t err=0;
	float temp;
	struct E28_2G4T_CONFIG_Typedef LoRaCfg;
	do
	{
		printf("\r\n");
		
		LoRa_ReadConfigPara(&LoRaCfg);
		HAL_Delay(50);
		LoRa_ReadConfigPara(&LoRaCfg);
		SystemGlobalVar.LoRa_Config = LoRaCfg;
		printf("\r\n\r\n========================================================\r\n");
		printf("4.LoRa模块配置\r\n");
		printf("========================================================\r\n");
		printf("1.模块地址:%d\r\n",SystemGlobalVar.LoRa_Config.Addr);
		
		if(SystemGlobalVar.LoRa_Config.SerialInfMode==0)
			printf("2.串口通信模式：8N1(默认)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==1)
			printf("2.串口通信模式：801\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==2)
			printf("2.串口通信模式：8E1\r\n");
		
		if(SystemGlobalVar.LoRa_Config.SerialInfBaud==0)
			printf("3.串口波特率：1200\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==1)
			printf("3.串口波特率：4800\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==2)
			printf("3.串口波特率：9600(默认)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==3)
			printf("3.串口波特率：19200\r\n");
		else //if(SystemGlobalVar.LoRa_Config.SerialInfBaud==4)
			printf("3.串口波特率出错，请重新配置\r\n");
		
		if(SystemGlobalVar.LoRa_Config.AirSpeed==0)
			printf("4.空中速率自适应（连续传输）\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==1)
			printf("4.空中速率:1K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==2)
			printf("4.空中速率:5K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==3)
			printf("4.空中速率:10K(默认)\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==4)
			printf("4.空中速率:50K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==5)
			printf("4.空中速率:100K\r\n");
//		else if(SystemGlobalVar.LoRa_Config.AirSpeed==6)
//			printf("4.空中速率:1M\r\n");
//		else if(SystemGlobalVar.LoRa_Config.AirSpeed==7)
//			printf("4.空中速率:2M\r\n");
		
		printf("5.通信信道:%d\r\n",SystemGlobalVar.LoRa_Config.Channel);
		printf("6.发射功率:");
		key = SystemGlobalVar.LoRa_Config.Option & 0x03;
		if(key==0)
			printf("12dBm(默认)\r\n");
		else if(key==1)
			printf("10dBm\r\n");
		else if(key==2)
			printf("7dBm\r\n");
		else if(key==3)
			printf("4dBm\r\n");
		
		printf("\r\n请输入：");
		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("1.模块地址（信道范围0~65535）:");
			  if(WaitInputNum(&temp,5))
				{
					if(temp<65536)
					{
						LoRaCfg.Addr = temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
					else
						printf("地址超出范围，请重新配置");
				}
			break;
			case '2':
				printf("2.串口通信模式,请输入如下选项\r\n\r\n");
				printf("  0. 8N1模式(默认)\r\n");
				printf("  1. 8O1模式\r\n");
				printf("  2. 8E1模式\r\n");
				printf("  其他退出\r\n\r\n");
				printf("请输入：");
				if(WaitInputNum(&temp,1))
				{
					if(temp<3)
					{
						LoRaCfg.SerialInfMode = temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
				} 
			break;
			case '3':
				printf("3.串口波特率，请输入如下选项\r\n\r\n");
				printf("  0. 波特率为1200\r\n");
				printf("  1. 波特率为4800\r\n");
				printf("  2. 波特率为9600(默认)\r\n");
				printf("  3. 波特率为19200\r\n");
//				printf("    4.波特率为57600\r\n");
//				printf("    5.波特率为115200\r\n");
				printf("  其他退出\r\n\r\n");
				printf("请输入：");
				if(WaitInputNum(&temp,1))
				{
					if(temp<4)
					{
						LoRaCfg.SerialInfBaud = temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
				}	
			break;
			case '4':
				printf("4.空中速率:请输入如下选项\r\n\r\n");
				printf("  0.空中速率自适应（连续传输）\r\n");
				printf("  1.空中速率:1K\r\n");
				printf("  2.空中速率:5K\r\n");
				printf("  3.空中速率:10K(默认)\r\n");
				printf("  4.空中速率:50K\r\n");
				printf("  5.空中速率:100K\r\n");
				printf("  其他退出\r\n\r\n");
				printf("请输入：");
				if(WaitInputNum(&temp,1))
				{
					if(temp<=5)
					{
						LoRaCfg.AirSpeed = temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
				}	
			break;
			case '5':
				printf("5.通信信道（信道范围0~127）:");
				if(WaitInputNum(&temp,3))
				{
					//printf("\r\ntemp=%d\r\n",temp);
					if(temp<128)
					{
						LoRaCfg.Channel = temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
					else
						printf("数值错误，请重新配置！\r\n");
				}
				break;
			case '6':
				printf("6.发射功率，请输入如下选项\r\n\r\n");
				printf("  0.12dBm(默认)\r\n");
				printf("  1.10dBm\r\n");
				printf("  2.7dBm\r\n");
				printf("  3.4dBm\r\n");
				if(WaitInputNum(&temp,1))
				{
					//printf("\r\ntemp=%d\r\n",temp);
					if(temp<=3)
					{
						LoRaCfg.Option &= 0xfc;
						LoRaCfg.Option |= (uint16_t)temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
				}
			break;
			default:	err=1;	break;	
		}
	}while(err==0);
}

//返回0，表示数据接收成功，返回1表示数据接收失败
uint8_t Inputdata(unsigned short *pdata,uint8_t len)
{	
	uint8_t i;
	uint8_t pbuff[8];
	uint8_t res,err=1;
	unsigned short temp;
	res = WaitInput(pbuff,len,1);
	if(res==0||res==1)
	{
		temp =0;
		for(i=0;i<len;i++)
		{
			if(pbuff[i]>='0'&& pbuff[i]<='9')
			{
				temp *=10;
				temp += (pbuff[i]-'0');
				*pdata = temp;
				err=0;
				continue;
			}
			break;
		}
	}
	return err;
}

//系统时间配置
void SystemTimeConfig()
{
	#if STM_RTC_USING
	uint8_t key;
	uint8_t err=0,flag;
	float temp;
	unsigned short year,mon,day,hour,min,sec;
	do
	{
		SystemGlobalVar.BJ_Time = RTC_Get_Time();
		printf("\r\n\r\n========================================================\r\n");
		printf("%d年%d月%d日 ",SystemGlobalVar.BJ_Time.sDate.Year+1970,SystemGlobalVar.BJ_Time.sDate.Month,SystemGlobalVar.BJ_Time.sDate.Date);
		printf(" %d时%d分%d秒\r\n",SystemGlobalVar.BJ_Time.sTime.Hours,SystemGlobalVar.BJ_Time.sTime.Minutes,SystemGlobalVar.BJ_Time.sTime.Seconds);
		printf("系统重启时间（天）：%d\r\n\r\n",SystemGlobalVar.ResetTimeStamps/86400);
		
		printf("修改请输入\r\n");
		printf("  1.修改系统时间：\r\n");
		printf("  2.修改系统重启时间：\r\n");
		printf("  其他按键返回\r\n");
		
		printf("\r\n请输入：");
		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				flag=0;
				printf("\r\n年:"); 
				if(WaitInputNum(&temp,4))
				{
					if(temp>=2022 && temp<=2099)	
					{
						year = temp;
						printf("\r\n月:");
						if(WaitInputNum(&temp,2))
						{
							if(temp>=1 && temp<=12)
							{
								mon = temp;
								printf("\r\n日:");
								if(WaitInputNum(&temp,2))
								{
									if(temp>=1 && temp<=31)
									{
										day = temp;
										printf("\r\n时:");
										if(WaitInputNum(&temp,2))
										{
											if(temp<=23)
											{
												hour = temp;
												printf("\r\n分:");
												if(WaitInputNum(&temp,2))
												{
													if(temp<=59)
													{
														min = temp;
														printf("\r\n秒:");
														if(WaitInputNum(&temp,2))
														{
															if(temp<=59)
															{
																sec = temp;
																printf("\r\n");
															}else	flag=1;
														}else	flag=1;
													}else	flag=1;
												}else	flag=1;
											}else	flag=1;
										}else	flag=1;
									}else	flag=1;
								}else	flag=1;
							}else	flag=1;
						}else	flag=1;
					}else	flag=1;	
				}else	flag=1;
				
				if(flag==0)
				{
					SystemGlobalVar.BJ_Time.sDate.Year =year-1970;
					SystemGlobalVar.BJ_Time.sDate.Month = mon;
					SystemGlobalVar.BJ_Time.sDate.Date = day;
					SystemGlobalVar.BJ_Time.sTime.Hours = hour;
					SystemGlobalVar.BJ_Time.sTime.Minutes = min;
					SystemGlobalVar.BJ_Time.sTime.Seconds = sec;
					RTC_Set_Time(SystemGlobalVar.BJ_Time);
				}
				else
					printf("\r\n输入错误，请重新配置");
				break;
			case '2':
				printf("2.系统重启时间（1~30天）:");
			  if(WaitInputNum(&temp,2))
				{
					if(temp>=1 && temp<=30)
					{
						SystemGlobalVar.ResetTimeStamps = temp*86400;
					}
					else
						printf("\r\n输入错误，请重新配置");
				}
				break;
			default:	err=1;	break;	
		}
	}while(err==0);
	#endif
}



////网络模式配置
//void NetworkModelConfig()
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("5.网络模式\r\n");
//		printf("(与服务器的通讯方式)\r\n");
//		printf("========================================================\r\n");
//		printf("当前模式：");
//		if(SystemGlobalVar.NetworkMode)
//			printf("网口模式\r\n");
//		else
//	  printf("DTU模式\r\n\n");
//    printf("修改请选择\r\n");
//    printf("1.DTU模式\r\n");
//    printf("2.网口模式\r\n");
//    
//		printf("\r\n请输入：");
//		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '0':err=1;break;
//			case '1':
//				SystemGlobalVar.NetworkMode = 0; //DTU模式
//				err=0;
//				break;
//			case '2':
//				SystemGlobalVar.NetworkMode = 1; //网口模式
//				err=0;
//			  break;
//			default:err=1;
//				break;	
//		}
//	}while(err==0);
//}

////网络参数配置
//void NetworkParameterConfig()
//{
//	uint8_t key,i,index;
//	uint8_t err=0;
//	uint8_t pbuff[16];
//  u16 temp;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("6.网络参数\r\n");
//		printf("========================================================\r\n");
//		printf("本地IP:%d.%d.%d.%d:%d\r\n",SystemGlobalVar.NetInf.CMD_IP[0],SystemGlobalVar.NetInf.CMD_IP[1],SystemGlobalVar.NetInf.CMD_IP[2],SystemGlobalVar.NetInf.CMD_IP[3],SystemGlobalVar.NetInf.CMD_Port);
//		printf("远端IP:%d.%d.%d.%d:%d\r\n",SystemGlobalVar.NetInf.CMA_IP[0],SystemGlobalVar.NetInf.CMA_IP[1],SystemGlobalVar.NetInf.CMA_IP[2],SystemGlobalVar.NetInf.CMA_IP[3],SystemGlobalVar.NetInf.CMA_Port);
//		printf("子网掩码:%d.%d.%d.%d\r\n",SystemGlobalVar.NetInf.Subnet_mask[0],SystemGlobalVar.NetInf.Subnet_mask[1],SystemGlobalVar.NetInf.Subnet_mask[2],SystemGlobalVar.NetInf.Subnet_mask[3]);
//		printf("网关:%d.%d.%d.%d\r\n",SystemGlobalVar.NetInf.Gateway[0],SystemGlobalVar.NetInf.Gateway[1],SystemGlobalVar.NetInf.Gateway[2],SystemGlobalVar.NetInf.Gateway[3]);
//		printf("Mac地址:%d.%d.%d.%d\r\n",SystemGlobalVar.NetInf.MAC[0],SystemGlobalVar.NetInf.MAC[1],SystemGlobalVar.NetInf.MAC[2],SystemGlobalVar.NetInf.MAC[3]);
//		printf("通讯协议:");
//		if(SystemGlobalVar.NetInf.Modbus)  //通讯协议  0: udp , 1:tcp
//				printf("TCP\r\n\n");
//		else
//				printf("UDP\r\n\n");
//		printf("1.修改本地IP\r\n");
//		printf("2.修改远端IP\r\n");
//		printf("3.修改本地端口\r\n");
//		printf("4.修改远端端口\r\n");
//		printf("5.修改子网掩码\r\n");
//		printf("6.修改默认网关\r\n");
//		printf("7.修改MAC地址\r\n");
//		printf("8.修改通讯协议\r\n");
//		
//		printf("\r\n请输入：");
//		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '0':err=1;break;
//			case '1':
//				index = 0;
//				temp = 0;
//        printf("本地IP:");
//				if(WaitInput(pbuff,16,0)==1)  //收到回车键
//				{
//					for(i=0;i<15;i++)
//					{
//						if(pbuff[i]>='0'&&pbuff[i]<='9')
//						{
//							temp *=10;
//							temp += (pbuff[i]-'0');
//						}
//						else if(pbuff[i]=='.'||pbuff[i]==0)
//						{
//							SystemGlobalVar.NetInf.CMD_IP[index++] = temp;
//              if(index==4) break;
//							temp = 0;
//						}
//					}
//					err=0;
//				}
//				else err=1;
//				break;
//			case '2':
//        printf("远端IP:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //收到回车键
//				{
//					for(i=0;i<15;i++)
//					{
//						if(pbuff[i]>='0'&&pbuff[i]<='9')
//						{
//							temp *=10;
//							temp += (pbuff[i]-'0');
//						}
//						else if(pbuff[i]=='.'||pbuff[i]==0)
//						{
//							SystemGlobalVar.NetInf.CMA_IP[index++] = temp;
//              if(index==4) break;
//							temp = 0;
//						}
//					}
//					err=0;
//				}
//				else err=1;
//				break;
//				case '3':
//          printf("本地端口:");
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //收到回车键
//				{
//					for(i=0;i<15;i++)
//					{
//						if(pbuff[i]>='0'&&pbuff[i]<='9')
//						{
//							temp *=10;
//							temp += (pbuff[i]-'0');
//							SystemGlobalVar.NetInf.CMD_Port = temp;
//						}
//            else if(pbuff[i]==0) break;    
//					}
//					err=0;
//				}
//				else err=1;
//				break;
//				case '4':
//          printf("远端端口:");
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //收到回车键
//				{
//					for(i=0;i<15;i++)
//					{
//						if(pbuff[i]>='0'&&pbuff[i]<='9')
//						{
//							temp *=10;
//							temp += (pbuff[i]-'0');
//							SystemGlobalVar.NetInf.CMA_Port = temp;
//						}
//            else if(pbuff[i]==0) break;
//					}
//					err=0;
//				}
//				else err=1;
//				break;
//				case '5':
//          printf("子网掩码:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //收到回车键
//				{
//					for(i=0;i<15;i++)
//					{
//						if(pbuff[i]>='0'&&pbuff[i]<='9')
//						{
//							temp *=10;
//							temp += (pbuff[i]-'0');
//						}
//						else if(pbuff[i]=='.'||pbuff[i]==0)
//						{
//							SystemGlobalVar.NetInf.Subnet_mask[index++] = temp;
//              if(index==4) break;
//							temp = 0;
//						}
//					}
//					err=0;
//				}
//				else err=1;
//				break;
//				case '6':
//        printf("默认网关:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //收到回车键
//				{
//					for(i=0;i<15;i++)
//					{
//						if(pbuff[i]>='0'&&pbuff[i]<='9')
//						{
//							temp *=10;
//							temp += (pbuff[i]-'0');
//						}
//						else if(pbuff[i]=='.'||pbuff[i]==0)
//						{
//							SystemGlobalVar.NetInf.Gateway[index++] = temp;
//              if(index==4) break;
//							temp = 0;
//						}
//					}
//					err=0;
//				}
//				else err=1;
//				break;
//				case '7':
//        printf("MAC地址:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //收到回车键
//				{
//					for(i=0;i<15;i++)
//					{
//						if(pbuff[i]>='0'&&pbuff[i]<='9')
//						{
//							temp *=10;
//							temp += (pbuff[i]-'0');
//						}
//						else if(pbuff[i]=='.'||pbuff[i]==0)
//						{
//							SystemGlobalVar.NetInf.MAC[index++] = temp;
//              if(index==4) break;
//							temp = 0;
//						}
//					}
//					err=0;
//				}
//				else err=1;
//				break;
//				case '8':
//          printf("通讯协议:\r\n");
//          printf("1.TCP模式\r\n");
//          printf("2.UDP模式\r\n");
//          printf("\r\n请输入：");
//					key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//          printf("\r\n\r\n");
//				  switch(key)
//					{
//						case '0':err=1;break;
//						case '1':
//							SystemGlobalVar.NetInf.Modbus = 1; //TCP模式
//							err=0;
//							break;
//						case '2':
//							SystemGlobalVar.NetInf.Modbus = 0; //UDP模式
//							err=0;
//							break;
//						default:err=1;
//							break;	
//					}
//					break;
//			default:err=1;
//				break;	
//		}
//	}while(err==0);
//}

////串口波特率配置
//void USART_BaudRateConfig()
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("7.串口波特率\r\n");
//		printf("（小无线默认115200,其余默认为9600）\r\n");
//		printf("========================================================\r\n");
//		printf("1.串口1(小无线)(Closed...Default=115200)%d\r\n",SystemGlobalVar.UsartConfig.UART1_BaudRate);
//		printf("2.串口2(调试DTU)：%d\r\n",SystemGlobalVar.UsartConfig.UART2_BaudRate);
//		printf("3.串口3(服务器通讯口)：%d\r\n",SystemGlobalVar.UsartConfig.UART3_BaudRate);
//		printf("4.串口6(传感器通讯口)：%d\r\n",SystemGlobalVar.UsartConfig.UART6_BaudRate);
//		
//		printf("\r\n修改请输入：");
//		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//		switch(key)
//		{
//			case '1':
//				printf("串口1(小无线)为默认波特率，不可修改,请修改其它串口波特率\r\n\r\n");
//				err=0;
//				break;
//			case '2':
//        printf("\r\n");
//        printf("2.串口2(调试DTU)\r\n");
//				printf("1.9600\r\n");
//			  printf("2.14400\r\n");
//				printf("3.19200\r\n");
//				printf("4.38400\r\n");
//				printf("5.115200\r\n");
//				printf("6.128000\r\n");
//			  printf("\r\n请输入：");
//			  err=0;
//			  key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//			  switch(key)
//				{
//					case '1': SystemGlobalVar.UsartConfig.UART2_BaudRate = 9600;break;
//					case '2': SystemGlobalVar.UsartConfig.UART2_BaudRate = 14400;break;
//					case '3': SystemGlobalVar.UsartConfig.UART2_BaudRate = 19200;break;
//					case '4': SystemGlobalVar.UsartConfig.UART2_BaudRate = 38400;break;
//					case '5': SystemGlobalVar.UsartConfig.UART2_BaudRate = 115200;break;
//					case '6': SystemGlobalVar.UsartConfig.UART2_BaudRate = 128000;break;
//					default:err=1;break;
//				}
//        break;
//				case '3':
//        printf("\r\n");
//        printf("3.串口3(服务器通讯口)\r\n");
//				printf("1.9600\r\n");
//			  printf("2.14400\r\n");
//				printf("3.19200\r\n");
//				printf("4.38400\r\n");
//				printf("5.115200\r\n");
//				printf("6.128000\r\n");
//			  printf("请输入：");
//			  err=0;
//			  key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//			  switch(key)
//				{
//					case '1': SystemGlobalVar.UsartConfig.UART3_BaudRate = 9600;break;
//					case '2': SystemGlobalVar.UsartConfig.UART3_BaudRate = 14400;break;
//					case '3': SystemGlobalVar.UsartConfig.UART3_BaudRate = 19200;break;
//					case '4': SystemGlobalVar.UsartConfig.UART3_BaudRate = 38400;break;
//					case '5': SystemGlobalVar.UsartConfig.UART3_BaudRate = 115200;break;
//					case '6': SystemGlobalVar.UsartConfig.UART3_BaudRate = 128000;break;
//					default:err=1;break;
//				}
//        break;
//				case '4':
//        printf("\r\n");
//        printf("4.串口6(传感器通讯口)\r\n");
//				printf("1.9600\r\n");
//			  printf("2.14400\r\n");
//				printf("3.19200\r\n");
//				printf("4.38400\r\n");
//				printf("5.115200\r\n");
//				printf("6.128000\r\n");
//			  printf("请输入：");
//			  err=0;
//			  key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//			  switch(key)
//				{
//					case '1': SystemGlobalVar.UsartConfig.UART6_BaudRate = 9600;break;
//					case '2': SystemGlobalVar.UsartConfig.UART6_BaudRate = 14400;break;
//					case '3': SystemGlobalVar.UsartConfig.UART6_BaudRate = 19200;break;
//					case '4': SystemGlobalVar.UsartConfig.UART6_BaudRate = 38400;break;
//					case '5': SystemGlobalVar.UsartConfig.UART6_BaudRate = 115200;break;
//					case '6': SystemGlobalVar.UsartConfig.UART6_BaudRate = 128000;break;
//					default:err=1;break;
//				}
//        break;
//			default:err=1; break;	
//		}
//	}while(err==0);
//}

//系统参数显示
void SystemsParameterDisplay()
{
//	unsigned short d,h,m,i; 
	uint8_t key;
	uint8_t pbuff[8];
	uint8_t res = 1;
	do
	{
		SystemGlobalVar.BJ_Time = RTC_Get_Time();
		printf("\r\n\r\n========================================================\r\n");
		printf("8.系统参数显示\r\n");
		printf("========================================================\r\n");

		printf("【时间参数】\r\n");
		printf("  1.当前时间：%d年%d月%d日 ",SystemGlobalVar.BJ_Time.sDate.Year+2000,SystemGlobalVar.BJ_Time.sDate.Month,SystemGlobalVar.BJ_Time.sDate.Date);
		printf(" %d时%d分%d秒\r\n",SystemGlobalVar.BJ_Time.sTime.Hours,SystemGlobalVar.BJ_Time.sTime.Minutes,SystemGlobalVar.BJ_Time.sTime.Seconds);
		printf("  2.系统重启时间（天）：%d\r\n",SystemGlobalVar.ResetTimeStamps/86400);
		printf("  3.采样周期(分钟)：%d\r\n\r\n",SystemGlobalVar.SamplingPeriod/60);

		printf("【终端ID配置】\r\n");
		printf("  1.厂商代码:%c%c\r\n",SystemGlobalVar.SensorID.ManufacturerID>>8,SystemGlobalVar.SensorID.ManufacturerID&0xff);
		printf("  2.版本标签:%d\r\n",SystemGlobalVar.SensorID.VersionLetter);
		printf("  3.版本号:%d\r\n",SystemGlobalVar.SensorID.VersionNumber);
		NumToSerialNumberString(SystemGlobalVar.SensorID.SerialNumber,pbuff,7);
		pbuff[7] = 0;
		printf("  4.产品序列号:%s\r\n\r\n",pbuff);

		printf("【传感器配置】\r\n");
		printf("  1.X轴加速度修正值：%d \r\n",SystemGlobalVar.ADXL355.X_Offset);
		printf("  2.Y轴加速度修正值：%d \r\n",SystemGlobalVar.ADXL355.Y_Offset);
		printf("  3.Z轴加速度修正值：%d \r\n\r\n",SystemGlobalVar.ADXL355.Z_Offset);

		printf("【杆塔倾斜复位值（偏移值）】\r\n");
		printf("  1.顺线倾斜角偏移值：%.2f°\r\n",SystemGlobalVar.TT_Offset.Angle_X);			// 4 顺线倾斜角偏移值（浮点数，精确到小数点后 2 位，单位：°）
		printf("  2.横向倾斜角偏移值：%.2f°\r\n",SystemGlobalVar.TT_Offset.Angle_Y);			// 4 横向倾斜角偏移值
		printf("  3.报警值:%.2f°\r\n\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit);

		printf("【LoRa模块配置】\r\n");
		printf("  1.模块地址:%d\r\n",SystemGlobalVar.LoRa_Config.Addr);
		if(SystemGlobalVar.LoRa_Config.SerialInfMode==0)
			printf("  2.串口通信模式：8N1(默认)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==1)
			printf("  2.串口通信模式：801\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==2)
			printf("  2.串口通信模式：8E1\r\n");
				
		if(SystemGlobalVar.LoRa_Config.SerialInfBaud==0)
			printf("  3.串口波特率：1200\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==1)
			printf("  3.串口波特率：4800\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==2)
			printf("  3.串口波特率：9600(默认)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==3)
			printf("  3.串口波特率：19200\r\n");
		else //if(SystemGlobalVar.LoRa_Config.SerialInfBaud==4)
			printf("  3.串口波特率出错，请重新配置\r\n");
				
		if(SystemGlobalVar.LoRa_Config.AirSpeed==0)
			printf("  4.空中速率自适应（连续传输）\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==1)
			printf("  4.空中速率:1K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==2)
			printf("  4.空中速率:5K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==3)
			printf("  4.空中速率:10K(默认)\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==4)
			printf("  4.空中速率:50K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==5)
			printf("4.空中速率:100K\r\n");
		//else if(SystemGlobalVar.LoRa_Config.AirSpeed==6)
		//	printf("4.空中速率:1M\r\n");
		//else if(SystemGlobalVar.LoRa_Config.AirSpeed==7)
		//printf("4.空中速率:2M\r\n");
				
		printf("  5.通信信道:%d\r\n",SystemGlobalVar.LoRa_Config.Channel);
		printf("  6.发射功率:");
		key = SystemGlobalVar.LoRa_Config.Option & 0x03;
		if(key==0)
			printf("12dBm(默认)\r\n");
		else if(key==1)
			printf("10dBm\r\n");
		else if(key==2)
			printf("7dBm\r\n");
		else if(key==3)
			printf("4dBm\r\n");
	}while(res==0);
	printf("\r\n任意按键继续\r\n");
	HyperTerminal_GetKey(0,0);  //不超时等待用户输入
}

////系统功能配置
//void SystemFunctiongConfig(void)
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("a.系统功能配置\r\n");
//		printf("========================================================\r\n");
//		printf("1.杆塔倾斜功能(%d):",SystemGlobalVar.SystemFunction.InclinationFun);
//		printf("2.微气象功能(%d):",SystemGlobalVar.SystemFunction.MeteoroFun);
//		printf("4.覆冰功能(%d):",SystemGlobalVar.SystemFunction.IceFun);
//		printf("5.图像功能(%d):",SystemGlobalVar.SystemFunction.ImageFun);
//		printf("\r\n修改请输入：");
//		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '1':
//				printf("输入1启用0不启用:");
//				key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//			  switch(key)
//				{
//					case '0': SystemGlobalVar.SystemFunction.InclinationFun=0;break;
//					case '1': SystemGlobalVar.SystemFunction.InclinationFun=1;break;
//					default:  err=1;break;
//				}
//				break;
//			case '2':
//				printf("输入1启用0不启用:");
//				key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//			  switch(key)
//				{
//					case '0': SystemGlobalVar.SystemFunction.MeteoroFun=0;break;
//					case '1': SystemGlobalVar.SystemFunction.MeteoroFun=1;break;
//					default:  err=1;break;
//				}
//			  break;
//				case '3':
//				printf("输入1启用0不启用:");
//				key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//			  switch(key)
//				{
//					case '0': SystemGlobalVar.SystemFunction.IceFun=0;break;
//					case '1': SystemGlobalVar.SystemFunction.IceFun=1;break;
//					default:  err=1;break;
//				}
//			  break;
//				case '4':
//				printf("输入1启用0不启用:");
//				key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//			  switch(key)
//				{
//					case '0': SystemGlobalVar.SystemFunction.ImageFun=0;break;
//					case '1': SystemGlobalVar.SystemFunction.ImageFun=1;break;
//					default:  err=1;break;
//				}
//			  break;
//			default:err=1;
//				break;	
//		}
//	}while(err==0);
//}	

void SystemFactoryResetConfig(void)		//恢复出厂设置
{
	uint8_t key;
	printf("\r\n\r\n确认恢复?(y/n):");
	key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
	printf("\r\n\r\n");
	if(key=='y'||key=='Y') 
	{
			printf("\r\n\r\n系统参数复位...\r\n");	
			SystemGlobalVarInitToFlash();						//	恢复出厂设置
      printf("\r\n系统参数复位完成\r\n");	
//		HAL_NVIC_SystemReset();
	}
}

////清空历史数据
//void ClearingHistoricalDataConfig(void)
//{
//	uint8_t key;
//	printf("\r\n\r\n是否要清空数据区(y/n):");
//	key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//	printf("\r\n\r\n");
//	if(key=='y'||key=='Y') 
//	{
//			printf("\r\n\r\n开始清空数据区...\r\n");	
//			SystemFun.ClearingHistoricalData();							//	清空历史数据
//			printf("\r\n清空历史数据完成\r\n");	
//	}
//}

////拉力修正值
//void TensionCorrectionConfig(void)
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("d.拉力修正值\r\n");
//		printf("========================================================\r\n");
//		printf("1.拉力修正值:\r\n\r\n");
//		printf("2.自动校正\r\n\r\n");
//		printf("当前拉力(此处可为负值)：\r\n\r\n");
//		printf("\r\n修改请输入：");
//		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '1':
//				//等待输入修正值
//				break;
//			case '2':
//				//自动修正
//			  break;
//			default:err=1;
//				break;	
//		}
//	}while(err==0);
//}

////SIM卡号
//void SimScardConfig(void)
//{
//	uint8_t res,i,err=0;
//	uint8_t pbuff[20];
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("e.SIM卡号\r\n");
//		printf("========================================================\r\n");
//		for(i=0;i<20;i++)
//			printf("%c",SystemGlobalVar.PhoneNumber[i]);	
//		
//		printf("\r\n\r\n修改请输入：");
//		res=WaitInput(pbuff,20,0);
//		if(res==0||res==1)
//		{
//			for(i=0;i<20;i++)
//				SystemGlobalVar.PhoneNumber[i]=pbuff[i];
//			printf("\r\n\r\n");
//		}
//		else
//			break;
//	}while(err==0);
//}

////杆塔倾斜配置
//void InclinationConfig(void)
//{
//	uint8_t key;
//	uint8_t res,i,err=0;
//	uint8_t pbuff[20];
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("h.杆塔倾斜配置\r\n");
//		printf("========================================================\r\n");
//		printf("注1:相对零点(以当前位置为零点)\r\n");
//		printf("注2:绝对零点(以出厂标定为零点)\r\n");
//		//传感器没有连接就打印 printf("未连接传感器\r\n");
//		
//		printf("\r\n\r\n修改请输入：\r\n");
//		printf("请输入0-绝对零点,1-相对零点：\r\n");
//		res=WaitInput(pbuff,20,0);
//		key = HyperTerminal_GetKey(0,1);  //不超时等待用户输入
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '0':
//				//绝对零点
//				break;
//			case '1':
//				//相对零点
//			  break;
//			default:err=1;
//				break;	
//		}
//	}while(err==0);
//}

/*

*/
/**
  * @}
  */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
