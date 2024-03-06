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

uint8_t Main_Menu(void);								//���˵�
void ChangePassword(void);							//��������
void SensorDeviceConfig(void);   			//�������ն�����
void CycleSamplingConfig(void);				//��������������
void SystemTimeConfig(void);						//ϵͳʱ������
void NetworkModelConfig(void);					//����ģʽ����
void NetworkParameterConfig(void);			//�����������
void USART_BaudRateConfig(void);				//���ڲ���������
void SystemsParameterDisplay(void);     //ϵͳ������ʾ
void SystemFunctiongConfig(void);				//ϵͳ��������
void SystemFactoryResetConfig(void);		//�ָ���������
void ClearingHistoricalDataConfig(void);//�����ʷ����
//void TensionCorrectionConfig(void);			//��������ֵ
//void SimScardConfig(void);							//SIM����
void InclinationConfig(void);						//������б����
void LoRaConfig(void);										//LoRa����

void TiltSensorsConfig(void);		//5.��Ǵ���������

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
uint8_t HyperTerminal_GetKey(uint16_t timeout,uint8_t res)
{
  uint8_t key = 0;
	if(timeout==0)   //����ʱ�ȴ�
	{
		/* Waiting for user input */
		while (1)
		{
			if ( __HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_RXNE) )
			{
				key = (uint8_t)Shell_huart.Instance->RDR;
//				while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������
//				Shell_huart.Instance->TDR = key;
				return key;
			}
		}	
	}
	else		//��ʱ�ȴ�
	{
		while (timeout--)
		{
			if ( __HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_RXNE) )
			{
				key = (uint8_t)Shell_huart.Instance->RDR;
//				while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������
//				Shell_huart.Instance->TDR = key;
				return key;
			}
		}
	}
  return key;
}
/**
  * @brief  WaitInput
* @param  pbuff:������,len�����ճ���, mode��bit 0: 0 ����ȫ���ַ��� 1 ���������ַ� 
																			mode��bit 7: 0 ������ʾ��     1 *����ʾ
  * @retval 0����ȷ��1���س�  2�����󷵻�
  */
uint8_t WaitInput(uint8_t *pbuff,uint8_t len,uint8_t mode)
{
	uint8_t key,i=0;
	memset(pbuff, 0, len);
//	printf("�����룺");
	while(1)
	{
		key = HyperTerminal_GetKey(0,0);  //����ʱ�ȴ��û�����
		if(mode&0x0f)
		{
			if(((key>='0') && (key<='9'))|| (key=='-')|| (key=='.'))  //
			{
        while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������
        if(mode&0x80)  Shell_huart.Instance->TDR = '*';
        else Shell_huart.Instance->TDR = key;
				pbuff[i++]=key; if(key=='-'&& i>1) return 2;
				if(i==len)   //�������len���ַ�����
				{
					return 0;
				}
			}
			else if(key=='\b')   //ɾ����
			{
				if(i>0)        //�Ѿ����ַ�����
				{
					i--;
					pbuff[i]= 0 ;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������
          Shell_huart.Instance->TDR = key;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������   
          Shell_huart.Instance->TDR = ' ';
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������
          Shell_huart.Instance->TDR = key;
				}
			}
			else if(key==0x0D)   //�س�
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
				while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0){;}	//ѭ������,ֱ���������
        if(mode&0x80)  Shell_huart.Instance->TDR = '*';
        else Shell_huart.Instance->TDR = key;
        pbuff[i++]=key;
				if(i==len)   //�������len���ַ�����
				{	
					return 0;
				}
			}
			else if(key=='\b')   //ɾ����
			{
				if(i>0)        //�Ѿ����ַ�����
				{
					i--;
					pbuff[i]= 0 ;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������
          Shell_huart.Instance->TDR = key;
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������   
          Shell_huart.Instance->TDR = ' ';
          while(__HAL_UART_GET_FLAG(&Shell_huart, UART_FLAG_TC)==0);		//ѭ������,ֱ���������
          Shell_huart.Instance->TDR = key;
				}
			}
			else if(key==0x0D)   //�س�
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
* @param  Num:���յ������ݣ�size:������������λ��
* @retval 0:û���������룬	1:����������
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
				if(res==1) return 0; //2��С���㣬������
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
  * @param  ���������������
  * @retval 0��������ȷ��1���������
  */
uint8_t WaitInputPassWrod(uint8_t times)
{
	uint8_t i,res = 0;
	char password[8];
	while(times--)
	{
		printf("\r\n���������룺");
    res = WaitInput((uint8_t*)password,4,0x80);
		if((res==0) || (res==1))
		{
			if(strcmp(password,"kyne")==0) return 0;  //��������
			else 
			{
				res = 0;
				for(i=0;i<4;i++)
					if(password[i]!=SystemGlobalVar.PassWord[i]) {res=1;break;}
				if(res==0) return 0;
			}
		}
    else
      printf("\r\n�������");
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
	printf("************************ϵͳ��������****************************\r\n");
	printf("*               ����������Դ����Ƽ����޹�˾                   *\r\n");
	printf("*                      ���߼��ϵͳ                            *\r\n");
	printf("* ����:ϵͳ���ù���                                            *\r\n");
//	printf("* ��д:                                                        *\r\n");
//	printf("* ����:                                                        *\r\n");
	printf("* �汾:Sensor app V1.0                                         *\r\n");
	printf("* ����:2022.01                                                 *\r\n");
	printf("****************************************************************\r\n\r\n");
	printf("3����������,��s�����뽻��...\r\n\n");
	/* Receive key */
	uint32_t RunTime = HAL_GetTick();
	while (1)
	{
		if(HAL_GetTick()-RunTime>=timeout)
			return 1; 	//��ʱ����ϵͳ
		key = HyperTerminal_GetKey(10,0);
    if (key == 's'||key == 'S')          //�������ý���
    {
			if(WaitInputPassWrod(3))  return 1;  //����3�����룬����ȷ���˳�
      else
        break;
    }
    else if (key > 0) 
		{
			//printf("\r\n�������ý���\r\n");
			return 1;   //�м�������
		}
	}
	while(1)
	{
		if(Main_Menu()==0) break;   //û���޸����������ϵͳ
		HAL_Delay(1);
	}
	return 0;
}


///**
//  * @brief  ���˵�
//  * @param  None
//  * @retval None
//  */
uint8_t Main_Menu(void)
{
	uint8_t key = 0;
	uint8_t res=0xff;
	printf("\r\n\r\n************************���˵�****************************\r\n");
	printf("1.��������\r\n");
	printf("2.�������ն�����\r\n");
	printf("3.������������\r\n");
	printf("4.LoRaģ������\r\n");
	printf("5.��Ǵ������������\r\n");
	printf("6.ϵͳʱ��,Ӳ������ʱ��\r\n");
	printf("7.ʵʱ������ʾ\r\n");
	printf("8.ϵͳ������ʾ\r\n");
	printf("b.�ָ���������\r\n");
	printf("0.�����˳�\r\n\r\n");
   
  printf("\r\n�����룺");
	key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
  printf("\r\n\n");
	switch(key)
	{
		case '0':
			res = 0;			//
			printf("�����޸�?(y/n):");
			key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
		  if(key=='y'||key=='Y') 
			{
					printf("\r\n\r\n�����޸�...\r\n");	
					SystemConfigSave();//��������
				  HAL_Delay(100);
				  HAL_NVIC_SystemReset();  //����ϵͳ
			}
			break;
		case '1':       ChangePassword();	      //��������
			res = 1;
			break;
		case '2':  			SensorDeviceConfig();   //�������ն�����
			res = 2;
			break;
		case '3':				CycleSamplingConfig();   //��������������
			break;
		case '4':				LoRaConfig();   //LoRaģ������
			break;	
		case '5':				TiltSensorsConfig();				//5.��Ǵ������������
			break;
		case '6':				SystemTimeConfig();				//ϵͳʱ������
			break;
		case '7':				//ʵʱ����
			Shell_SensorStatusUp();				 				//������״̬��
			Shell_SensorMonitoringDataUp();				 //�������
			break;
//		case '5':				NetworkModelConfig();		  //����ģʽ����
//			break;
//		case '6':				NetworkParameterConfig();			//�����������		
//			break;
//		case '7':				USART_BaudRateConfig();				//���ڲ���������	
//			break;
//		
//			break;
		case '8':				SystemsParameterDisplay();       //ϵͳ������ʾ
			break;
//		case 'a':				SystemFunctiongConfig();				//ϵͳ��������	
//			break;
		case 'B':				
		case 'b':				SystemFactoryResetConfig();		  //�ָ���������		
			break;
//		case 'c':				ClearingHistoricalDataConfig();//�����ʷ����		
//			break;
//		case 'd':				TensionCorrectionConfig();			//��������ֵ		
//			break;
//		case 'e':				SimScardConfig();							//SIM����
//			break;
//		case 'f':				
//			break;
//		case 'g':				
//			break;
//		case 'h':				InclinationConfig();						//������б����			
//			break;
//		case 'o':				
//			break;
		default:break;
	}
	return res;
}

/**
  * @brief  ��������
  * @param  None
  * @retval None
  */
void ChangePassword()
{
	uint8_t res,i;
	char password[4];
	printf("\r\n\r\n========================================================\r\n");
	printf("1.��������\r\n");
	printf("========================================================\r\n");
	printf("\r\n�����루4�������ַ���Esc���˳�����");
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
				printf("\r\n��������ʧ�ܣ�\r\n");
				HAL_Delay(1000);
			}
	}	
}

///**
//  * @brief  �������ն�����
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
		printf("2.�������ն�����\r\n");
		printf("========================================================\r\n");
		printf("1.���̴���:%d\r\n",SystemGlobalVar.SensorID.ManufacturerID);
		printf("2.�汾��ǩ:%d\r\n",SystemGlobalVar.SensorID.VersionLetter);
		printf("3.�汾��:%d\r\n",SystemGlobalVar.SensorID.VersionNumber);
		//printf("4.��Ʒ���к�:%d\r\n",SystemGlobalVar.SensorID.SerialNumber);
		NumToSerialNumberString(SystemGlobalVar.SensorID.SerialNumber,pbuff,7);
		pbuff[7] = 0;
		printf("4.��Ʒ���к�:%s\r\n",pbuff);
		printf("\r\n�����룺");
		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("1.���̴���(0~65535):");
				if(WaitInputNum(&temp,5))
				{
//					if(((pbuff[0]>='0'&&pbuff[0]<='9')||(pbuff[0]>='A'&&pbuff[0]<='Z')||(pbuff[0]>='a'&&pbuff[0]<='z'))&&((pbuff[1]>='0'&&pbuff[1]<='9')||(pbuff[1]>='A'&&pbuff[1]<='Z')||(pbuff[1]>='a'&&pbuff[1]<='z')))
//						SystemGlobalVar.SensorID.ManufacturerID = (pbuff[0]<<8)|pbuff[1];
					if(temp>=0 &&temp<65536)SystemGlobalVar.SensorID.ManufacturerID=temp;
					else
					{
						printf("\r\n�������\r\n");
						HAL_Delay(1000);
					}
				}
				else
				{
					printf("\r\n�������\r\n");
					HAL_Delay(1000);
				}
			break;
			case '2':
				printf("2.�汾��ǩ(1~26):");
				if(WaitInputNum(&temp,2))
				{
					if(temp>0 &&temp<32)
						SystemGlobalVar.SensorID.VersionLetter = temp;
					else
						printf("\r\n�������\r\n");
				}		
			break;
			case '3':
				printf("3.�汾��(0~63):");
				if(WaitInputNum(&temp,2))
				{
					if(temp>=0 && temp<64)
						SystemGlobalVar.SensorID.VersionNumber = temp;	
					else
						printf("\r\n�������\r\n");
				}
			break;
			case '4':
				printf("4.��Ʒ���к�(0000000~2097151):");
				if(WaitInputNum(&temp,7))
				{
					if(temp<2097152)
						SystemGlobalVar.SensorID.SerialNumber = temp;
					else
						printf("\r\n�������\r\n");
				}
			break;
			default:err=1;break;
		}
	}while(err==0);
}


//��������������
void CycleSamplingConfig()
{
	uint8_t key;
	uint8_t err=0;
	float temp;
	do
	{
		printf("\r\n\r\n========================================================\r\n");
		printf("3.��������\r\n");
		printf("========================================================\r\n");
		printf("1.������б(����)��%d\r\n",SystemGlobalVar.SamplingPeriod/60);
		printf("�������������\r\n");
		
		printf("\r\n�����룺");
		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("\r\n1.������б��������(1~240����/����):"); 
				if(WaitInputNum(&temp,3))
				{
					if(temp>0 && temp<=240)
						SystemGlobalVar.SamplingPeriod = temp*60;	
					else
						printf("\r\n�������");
				}
				break;
			default:err=1;
				break;	
		}
	}while(err==0);
	
}

///**
//  * @brief  �������ն�����
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
		printf("����ֵ����\r\n");
		printf("========================================================\r\n");
		printf("1.����ֵ(��):%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit);
		//printf("2.����ֵ:%.2f\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit);
		printf("����������\r\n");
		
		printf("\r\n�����룺");
		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("1.����ֵ(0~60��,0��ʾ������):");
				if(WaitInputNum(&temp,2))
				{
					if(temp<90)
						SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit = temp;
					else
						printf("\r\n�������\r\n");
				}	
			break;
//			case '2':
//				printf("2.����ֵ(0~60�㣬0��ʾ������):-");
//				if(WaitInputNum(&temp,2))
//				{
//					if(temp<90)
//						SystemGlobalVar.AlarmLimit.Angle_X.AlarmLowerLimit = (0-(float)temp);
//					else
//						printf("\r\n�������\r\n");
//				}		
//			break;
			default:err=1;break;	
		}
	}while(err==0);
}

//������б��λֵ�����ԽǶ�ֵ��
//TiltSensorsConfig()�е���
void TiltAbsoluteSet(float Angle_x,float Angle_y)
{
	uint8_t key;
	uint8_t err=0;
	float temp;
	do
	{
		printf("\r\n\r\n��������б���������ԽǶ�ֵ-60~+60��)��\r\n");			
		printf("1.˳����б���ԽǶ�ֵ��%.2f \r\n",Angle_x-SystemGlobalVar.TT_Offset.Angle_X);					// 4 ˳����б��ƫ��ֵ������������ȷ��С����� 2 λ����λ���㣩
		printf("2.������б���ԽǶ�ֵ��%.2f \r\n",Angle_y-SystemGlobalVar.TT_Offset.Angle_Y);			// 4 ������б��ƫ��ֵ
		printf("��������\r\n\r\n");
		printf("������\r\n");
		key = HyperTerminal_GetKey(0,1);  					//����ʱ�ȴ��û�����
    printf("\r\n\r\n");
		switch(key)
		{
				case '1':
					printf("˳����б���ԽǶ�ֵ:");
					if(WaitInputNum(&temp,5))
					{
						if(temp >= -60 && temp <= 60)
						{
							SystemGlobalVar.TT_Offset.Angle_X = Angle_x-temp;
						}
						else
							printf("�Ƕȳ�����Χ������������");
					}
					break;
				case '2':
					printf("������б���ԽǶ�ֵ:");
					if(WaitInputNum(&temp,5))
					{
						if(temp >= -60 && temp <= 60)
						{
							SystemGlobalVar.TT_Offset.Angle_Y = Angle_y-temp;
						}
						else
							printf("�Ƕȳ�����Χ������������");
					}
					break;
			default:	err=1;	break;	
		}
	}while(err==0);
}

//5.��Ǵ���������
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
		printf("5.��Ǵ������������\r\n");
		printf("========================================================\r\n");
		//Shell_ADXL355_ACC_Read(&ADXL355_ACC);   //��ȡ���������ٶ�ֵ���Ƕ�ֵ
		GetTowerTiltedVals(&pVals);
		printf("�����������ֵ�������㣩��\r\n");
//		printf("��б��ƫ��ֵ��%.1f \r\n",SystemGlobalVar.TT_Offset.Inclination);				// 4 ��б��ƫ��ֵ������������ȷ��С����� 1 λ����λ�� mm/m��
//		printf("˳����б��ƫ��ֵ��%.1f \r\n",SystemGlobalVar.TT_Offset.Inclination_X);	// 4 ˳����б��ƫ��ֵ������������ȷ��С����� 1 λ����λ��mm/m��
//		printf("������б��ƫ��ֵ��%.1f \r\n",SystemGlobalVar.TT_Offset.Inclination_Y);	// 4 ������б��ƫ��ֵ������������ȷ��С����� 1 λ����λ��mm/m��
		printf("1.˳����б�ǣ�%.2f��\r\n",pVals.Angle_X);					// 4 ˳����б��ƫ��ֵ������������ȷ��С����� 2 λ����λ���㣩
		printf("2.������б�ǣ�%.2f��\r\n\r\n",pVals.Angle_Y);			// 4 ������б��ƫ��ֵ
		
//		printf("��ʵʱ����ֵ��\r\n");
////		printf("1.X����ٶ�ֵ��%.2fg \r\n",ADXL355_ACC.ax/ADXL355_ACC.Scale);
////		printf("2.Y����ٶ�ֵ��%.2fg \r\n",ADXL355_ACC.ay/ADXL355_ACC.Scale);
////		printf("3.Z����ٶ�ֵ��%.2fg \r\n",ADXL355_ACC.az/ADXL355_ACC.Scale);
//		printf("1.˳����б�ǣ�%.2f \r\n",ADXL355_ACC.x_ang);				// 4 ˳����б��ƫ��ֵ������������ȷ��С����� 2 λ����λ���㣩
//		printf("2.������б�ǣ�%.2f \r\n\r\n",ADXL355_ACC.y_ang);		// 4 ������б��ƫ��ֵ
		
//		ADXL355_Offset_RW(0,&ax_off,1);	 //X��ʧ�������Ĵ�����.
//		ADXL355_Offset_RW(1,&ay_off,1);	 //Y��ʧ�������Ĵ�����.
//		ADXL355_Offset_RW(2,&az_off,1);	 //Z��ʧ�������Ĵ�����.
//		
//		printf("�����ٶ�����ֵ��\r\n");
//		printf("1.X����ٶ�����ֵ��%d \r\n",ax_off);
//		printf("2.Y����ٶ�����ֵ��%d \r\n",ay_off);
//		printf("3.Z����ٶ�����ֵ��%d \r\n\r\n",az_off);
		
		Shell_SensorAlarmLimitDataRequest();				 //����������ֵ��ѯ
		
		
		printf("\r\n\r\n�޸������룺\r\n");
		printf("1:����������������\r\n");
		printf("2:˳�ߡ�������б�Ǳ���������ֵ����\r\n");
//		printf("3:X����ٶ�����ֵ(����ǰӦ��������X���������ϴ�ֱ��ˮƽ��,�ݲ�֧���޸�)\r\n");
//		printf("4:Y����ٶ�����ֵ(����ǰӦ��������Y���������ϴ�ֱ��ˮƽ��,�ݲ�֧���޸�)\r\n");
//		printf("5:Z����ٶ�����ֵ(����ǰӦ��������Z���������ϴ�ֱ��ˮƽ��,������ˮƽ����ʱ���ٶ�ֵ�ӽ�1g,ƫ������������)\r\n");
		printf("a.��������λ(��ʼ��)\r\n");
		printf("������\r\n");
		key = HyperTerminal_GetKey(0,1);  					//����ʱ�ȴ��û�����
    printf("\r\n\r\n");
		switch(key)
		{
				case '1':
				{
					#if 0   //�����������
					TiltAbsoluteSet(ADXL355_ACC.x_ang,ADXL355_ACC.y_ang);
					#else		//����������
					SystemGlobalVar.TT_Offset.Angle_X = 0;
					SystemGlobalVar.TT_Offset.Angle_Y = 0;
					GetTowerTiltedVals(&pVals);
					SystemGlobalVar.TT_Offset.Angle_X =	pVals.Angle_X;							// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
					SystemGlobalVar.TT_Offset.Angle_Y =	pVals.Angle_Y;							// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
//					Shell_ADXL355_ACC_Read(&ADXL355_ACC);   //��ȡ���������ٶ�ֵ���Ƕ�ֵ
//					SystemGlobalVar.TT_Offset.Angle_X =	ADXL355_ACC.x_ang;							// 4 ˳����б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
//					SystemGlobalVar.TT_Offset.Angle_Y =	ADXL355_ACC.y_ang;							// 4 ������б�ǣ�����������ȷ��С����� 2 λ����λ���㣩
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
				printf("�ݲ�֧���޸�\r\n");
//				ax_off = 0;
//				ADXL355_Offset_RW(0,&ax_off,0);	 //X��ʧ�������Ĵ���д
//				HAL_Delay(100);
//				Shell_ADXL355_ACC_Read(&ADXL355_ACC);	  
//				ax_off = (((ADXL355_ACC.ax - ADXL355_ACC.Scale)/16));
//				SystemGlobalVar.ADXL355.X_Offset = ax_off;
//				ADXL355_Offset_RW(0,&ax_off,0);	 //Z��ʧ�������Ĵ���д..
				break;
			case '4':
				printf("�ݲ�֧���޸�\r\n");
//				ay_off = 0;
//				ADXL355_Offset_RW(1,&ay_off,0);	 //X��ʧ�������Ĵ���д
//				HAL_Delay(100);
//				Shell_ADXL355_ACC_Read(&ADXL355_ACC);	  
//				ay_off = (((ADXL355_ACC.ay - ADXL355_ACC.Scale)/16));
//				SystemGlobalVar.ADXL355.Y_Offset = ay_off;
//				ADXL355_Offset_RW(1,&ay_off,0);	 //Z��ʧ�������Ĵ���д..
			  break;
			case '5':
				az_off = 0;
				ADXL355_Offset_RW(2,&az_off,0);	 //X��ʧ�������Ĵ���д
				HAL_Delay(100);
				Shell_ADXL355_ACC_Read(&ADXL355_ACC);
//				if(ADXL355_ACC.az>ADXL355_ACC.Scale) 	  
				az_off = (((ADXL355_ACC.az - ADXL355_ACC.Scale)/16));
				SystemGlobalVar.ADXL355.Z_Offset = az_off;
				ADXL355_Offset_RW(2,&az_off,0);	 //Z��ʧ�������Ĵ���д.
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
					SystemConfigSave();      //��������
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
		printf("4.LoRaģ������\r\n");
		printf("========================================================\r\n");
		printf("1.ģ���ַ:%d\r\n",SystemGlobalVar.LoRa_Config.Addr);
		
		if(SystemGlobalVar.LoRa_Config.SerialInfMode==0)
			printf("2.����ͨ��ģʽ��8N1(Ĭ��)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==1)
			printf("2.����ͨ��ģʽ��801\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==2)
			printf("2.����ͨ��ģʽ��8E1\r\n");
		
		if(SystemGlobalVar.LoRa_Config.SerialInfBaud==0)
			printf("3.���ڲ����ʣ�1200\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==1)
			printf("3.���ڲ����ʣ�4800\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==2)
			printf("3.���ڲ����ʣ�9600(Ĭ��)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==3)
			printf("3.���ڲ����ʣ�19200\r\n");
		else //if(SystemGlobalVar.LoRa_Config.SerialInfBaud==4)
			printf("3.���ڲ����ʳ�������������\r\n");
		
		if(SystemGlobalVar.LoRa_Config.AirSpeed==0)
			printf("4.������������Ӧ���������䣩\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==1)
			printf("4.��������:1K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==2)
			printf("4.��������:5K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==3)
			printf("4.��������:10K(Ĭ��)\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==4)
			printf("4.��������:50K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==5)
			printf("4.��������:100K\r\n");
//		else if(SystemGlobalVar.LoRa_Config.AirSpeed==6)
//			printf("4.��������:1M\r\n");
//		else if(SystemGlobalVar.LoRa_Config.AirSpeed==7)
//			printf("4.��������:2M\r\n");
		
		printf("5.ͨ���ŵ�:%d\r\n",SystemGlobalVar.LoRa_Config.Channel);
		printf("6.���书��:");
		key = SystemGlobalVar.LoRa_Config.Option & 0x03;
		if(key==0)
			printf("12dBm(Ĭ��)\r\n");
		else if(key==1)
			printf("10dBm\r\n");
		else if(key==2)
			printf("7dBm\r\n");
		else if(key==3)
			printf("4dBm\r\n");
		
		printf("\r\n�����룺");
		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				printf("1.ģ���ַ���ŵ���Χ0~65535��:");
			  if(WaitInputNum(&temp,5))
				{
					if(temp<65536)
					{
						LoRaCfg.Addr = temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
					else
						printf("��ַ������Χ������������");
				}
			break;
			case '2':
				printf("2.����ͨ��ģʽ,����������ѡ��\r\n\r\n");
				printf("  0. 8N1ģʽ(Ĭ��)\r\n");
				printf("  1. 8O1ģʽ\r\n");
				printf("  2. 8E1ģʽ\r\n");
				printf("  �����˳�\r\n\r\n");
				printf("�����룺");
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
				printf("3.���ڲ����ʣ�����������ѡ��\r\n\r\n");
				printf("  0. ������Ϊ1200\r\n");
				printf("  1. ������Ϊ4800\r\n");
				printf("  2. ������Ϊ9600(Ĭ��)\r\n");
				printf("  3. ������Ϊ19200\r\n");
//				printf("    4.������Ϊ57600\r\n");
//				printf("    5.������Ϊ115200\r\n");
				printf("  �����˳�\r\n\r\n");
				printf("�����룺");
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
				printf("4.��������:����������ѡ��\r\n\r\n");
				printf("  0.������������Ӧ���������䣩\r\n");
				printf("  1.��������:1K\r\n");
				printf("  2.��������:5K\r\n");
				printf("  3.��������:10K(Ĭ��)\r\n");
				printf("  4.��������:50K\r\n");
				printf("  5.��������:100K\r\n");
				printf("  �����˳�\r\n\r\n");
				printf("�����룺");
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
				printf("5.ͨ���ŵ����ŵ���Χ0~127��:");
				if(WaitInputNum(&temp,3))
				{
					//printf("\r\ntemp=%d\r\n",temp);
					if(temp<128)
					{
						LoRaCfg.Channel = temp;
						LoRa_WriteConfigPara(LoRaCfg);
					}
					else
						printf("��ֵ�������������ã�\r\n");
				}
				break;
			case '6':
				printf("6.���书�ʣ�����������ѡ��\r\n\r\n");
				printf("  0.12dBm(Ĭ��)\r\n");
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

//����0����ʾ���ݽ��ճɹ�������1��ʾ���ݽ���ʧ��
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

//ϵͳʱ������
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
		printf("%d��%d��%d�� ",SystemGlobalVar.BJ_Time.sDate.Year+1970,SystemGlobalVar.BJ_Time.sDate.Month,SystemGlobalVar.BJ_Time.sDate.Date);
		printf(" %dʱ%d��%d��\r\n",SystemGlobalVar.BJ_Time.sTime.Hours,SystemGlobalVar.BJ_Time.sTime.Minutes,SystemGlobalVar.BJ_Time.sTime.Seconds);
		printf("ϵͳ����ʱ�䣨�죩��%d\r\n\r\n",SystemGlobalVar.ResetTimeStamps/86400);
		
		printf("�޸�������\r\n");
		printf("  1.�޸�ϵͳʱ�䣺\r\n");
		printf("  2.�޸�ϵͳ����ʱ�䣺\r\n");
		printf("  ������������\r\n");
		
		printf("\r\n�����룺");
		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
    printf("\r\n\r\n");
		switch(key)
		{
			case '1':
				flag=0;
				printf("\r\n��:"); 
				if(WaitInputNum(&temp,4))
				{
					if(temp>=2022 && temp<=2099)	
					{
						year = temp;
						printf("\r\n��:");
						if(WaitInputNum(&temp,2))
						{
							if(temp>=1 && temp<=12)
							{
								mon = temp;
								printf("\r\n��:");
								if(WaitInputNum(&temp,2))
								{
									if(temp>=1 && temp<=31)
									{
										day = temp;
										printf("\r\nʱ:");
										if(WaitInputNum(&temp,2))
										{
											if(temp<=23)
											{
												hour = temp;
												printf("\r\n��:");
												if(WaitInputNum(&temp,2))
												{
													if(temp<=59)
													{
														min = temp;
														printf("\r\n��:");
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
					printf("\r\n�����������������");
				break;
			case '2':
				printf("2.ϵͳ����ʱ�䣨1~30�죩:");
			  if(WaitInputNum(&temp,2))
				{
					if(temp>=1 && temp<=30)
					{
						SystemGlobalVar.ResetTimeStamps = temp*86400;
					}
					else
						printf("\r\n�����������������");
				}
				break;
			default:	err=1;	break;	
		}
	}while(err==0);
	#endif
}



////����ģʽ����
//void NetworkModelConfig()
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("5.����ģʽ\r\n");
//		printf("(���������ͨѶ��ʽ)\r\n");
//		printf("========================================================\r\n");
//		printf("��ǰģʽ��");
//		if(SystemGlobalVar.NetworkMode)
//			printf("����ģʽ\r\n");
//		else
//	  printf("DTUģʽ\r\n\n");
//    printf("�޸���ѡ��\r\n");
//    printf("1.DTUģʽ\r\n");
//    printf("2.����ģʽ\r\n");
//    
//		printf("\r\n�����룺");
//		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '0':err=1;break;
//			case '1':
//				SystemGlobalVar.NetworkMode = 0; //DTUģʽ
//				err=0;
//				break;
//			case '2':
//				SystemGlobalVar.NetworkMode = 1; //����ģʽ
//				err=0;
//			  break;
//			default:err=1;
//				break;	
//		}
//	}while(err==0);
//}

////�����������
//void NetworkParameterConfig()
//{
//	uint8_t key,i,index;
//	uint8_t err=0;
//	uint8_t pbuff[16];
//  u16 temp;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("6.�������\r\n");
//		printf("========================================================\r\n");
//		printf("����IP:%d.%d.%d.%d:%d\r\n",SystemGlobalVar.NetInf.CMD_IP[0],SystemGlobalVar.NetInf.CMD_IP[1],SystemGlobalVar.NetInf.CMD_IP[2],SystemGlobalVar.NetInf.CMD_IP[3],SystemGlobalVar.NetInf.CMD_Port);
//		printf("Զ��IP:%d.%d.%d.%d:%d\r\n",SystemGlobalVar.NetInf.CMA_IP[0],SystemGlobalVar.NetInf.CMA_IP[1],SystemGlobalVar.NetInf.CMA_IP[2],SystemGlobalVar.NetInf.CMA_IP[3],SystemGlobalVar.NetInf.CMA_Port);
//		printf("��������:%d.%d.%d.%d\r\n",SystemGlobalVar.NetInf.Subnet_mask[0],SystemGlobalVar.NetInf.Subnet_mask[1],SystemGlobalVar.NetInf.Subnet_mask[2],SystemGlobalVar.NetInf.Subnet_mask[3]);
//		printf("����:%d.%d.%d.%d\r\n",SystemGlobalVar.NetInf.Gateway[0],SystemGlobalVar.NetInf.Gateway[1],SystemGlobalVar.NetInf.Gateway[2],SystemGlobalVar.NetInf.Gateway[3]);
//		printf("Mac��ַ:%d.%d.%d.%d\r\n",SystemGlobalVar.NetInf.MAC[0],SystemGlobalVar.NetInf.MAC[1],SystemGlobalVar.NetInf.MAC[2],SystemGlobalVar.NetInf.MAC[3]);
//		printf("ͨѶЭ��:");
//		if(SystemGlobalVar.NetInf.Modbus)  //ͨѶЭ��  0: udp , 1:tcp
//				printf("TCP\r\n\n");
//		else
//				printf("UDP\r\n\n");
//		printf("1.�޸ı���IP\r\n");
//		printf("2.�޸�Զ��IP\r\n");
//		printf("3.�޸ı��ض˿�\r\n");
//		printf("4.�޸�Զ�˶˿�\r\n");
//		printf("5.�޸���������\r\n");
//		printf("6.�޸�Ĭ������\r\n");
//		printf("7.�޸�MAC��ַ\r\n");
//		printf("8.�޸�ͨѶЭ��\r\n");
//		
//		printf("\r\n�����룺");
//		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '0':err=1;break;
//			case '1':
//				index = 0;
//				temp = 0;
//        printf("����IP:");
//				if(WaitInput(pbuff,16,0)==1)  //�յ��س���
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
//        printf("Զ��IP:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //�յ��س���
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
//          printf("���ض˿�:");
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //�յ��س���
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
//          printf("Զ�˶˿�:");
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //�յ��س���
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
//          printf("��������:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //�յ��س���
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
//        printf("Ĭ������:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //�յ��س���
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
//        printf("MAC��ַ:");
//				index = 0;
//				temp = 0;
//				if(WaitInput(pbuff,16,0)==1)  //�յ��س���
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
//          printf("ͨѶЭ��:\r\n");
//          printf("1.TCPģʽ\r\n");
//          printf("2.UDPģʽ\r\n");
//          printf("\r\n�����룺");
//					key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//          printf("\r\n\r\n");
//				  switch(key)
//					{
//						case '0':err=1;break;
//						case '1':
//							SystemGlobalVar.NetInf.Modbus = 1; //TCPģʽ
//							err=0;
//							break;
//						case '2':
//							SystemGlobalVar.NetInf.Modbus = 0; //UDPģʽ
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

////���ڲ���������
//void USART_BaudRateConfig()
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("7.���ڲ�����\r\n");
//		printf("��С����Ĭ��115200,����Ĭ��Ϊ9600��\r\n");
//		printf("========================================================\r\n");
//		printf("1.����1(С����)(Closed...Default=115200)%d\r\n",SystemGlobalVar.UsartConfig.UART1_BaudRate);
//		printf("2.����2(����DTU)��%d\r\n",SystemGlobalVar.UsartConfig.UART2_BaudRate);
//		printf("3.����3(������ͨѶ��)��%d\r\n",SystemGlobalVar.UsartConfig.UART3_BaudRate);
//		printf("4.����6(������ͨѶ��)��%d\r\n",SystemGlobalVar.UsartConfig.UART6_BaudRate);
//		
//		printf("\r\n�޸������룺");
//		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//		switch(key)
//		{
//			case '1':
//				printf("����1(С����)ΪĬ�ϲ����ʣ������޸�,���޸��������ڲ�����\r\n\r\n");
//				err=0;
//				break;
//			case '2':
//        printf("\r\n");
//        printf("2.����2(����DTU)\r\n");
//				printf("1.9600\r\n");
//			  printf("2.14400\r\n");
//				printf("3.19200\r\n");
//				printf("4.38400\r\n");
//				printf("5.115200\r\n");
//				printf("6.128000\r\n");
//			  printf("\r\n�����룺");
//			  err=0;
//			  key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
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
//        printf("3.����3(������ͨѶ��)\r\n");
//				printf("1.9600\r\n");
//			  printf("2.14400\r\n");
//				printf("3.19200\r\n");
//				printf("4.38400\r\n");
//				printf("5.115200\r\n");
//				printf("6.128000\r\n");
//			  printf("�����룺");
//			  err=0;
//			  key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
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
//        printf("4.����6(������ͨѶ��)\r\n");
//				printf("1.9600\r\n");
//			  printf("2.14400\r\n");
//				printf("3.19200\r\n");
//				printf("4.38400\r\n");
//				printf("5.115200\r\n");
//				printf("6.128000\r\n");
//			  printf("�����룺");
//			  err=0;
//			  key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
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

//ϵͳ������ʾ
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
		printf("8.ϵͳ������ʾ\r\n");
		printf("========================================================\r\n");

		printf("��ʱ�������\r\n");
		printf("  1.��ǰʱ�䣺%d��%d��%d�� ",SystemGlobalVar.BJ_Time.sDate.Year+2000,SystemGlobalVar.BJ_Time.sDate.Month,SystemGlobalVar.BJ_Time.sDate.Date);
		printf(" %dʱ%d��%d��\r\n",SystemGlobalVar.BJ_Time.sTime.Hours,SystemGlobalVar.BJ_Time.sTime.Minutes,SystemGlobalVar.BJ_Time.sTime.Seconds);
		printf("  2.ϵͳ����ʱ�䣨�죩��%d\r\n",SystemGlobalVar.ResetTimeStamps/86400);
		printf("  3.��������(����)��%d\r\n\r\n",SystemGlobalVar.SamplingPeriod/60);

		printf("���ն�ID���á�\r\n");
		printf("  1.���̴���:%c%c\r\n",SystemGlobalVar.SensorID.ManufacturerID>>8,SystemGlobalVar.SensorID.ManufacturerID&0xff);
		printf("  2.�汾��ǩ:%d\r\n",SystemGlobalVar.SensorID.VersionLetter);
		printf("  3.�汾��:%d\r\n",SystemGlobalVar.SensorID.VersionNumber);
		NumToSerialNumberString(SystemGlobalVar.SensorID.SerialNumber,pbuff,7);
		pbuff[7] = 0;
		printf("  4.��Ʒ���к�:%s\r\n\r\n",pbuff);

		printf("�����������á�\r\n");
		printf("  1.X����ٶ�����ֵ��%d \r\n",SystemGlobalVar.ADXL355.X_Offset);
		printf("  2.Y����ٶ�����ֵ��%d \r\n",SystemGlobalVar.ADXL355.Y_Offset);
		printf("  3.Z����ٶ�����ֵ��%d \r\n\r\n",SystemGlobalVar.ADXL355.Z_Offset);

		printf("��������б��λֵ��ƫ��ֵ����\r\n");
		printf("  1.˳����б��ƫ��ֵ��%.2f��\r\n",SystemGlobalVar.TT_Offset.Angle_X);			// 4 ˳����б��ƫ��ֵ������������ȷ��С����� 2 λ����λ���㣩
		printf("  2.������б��ƫ��ֵ��%.2f��\r\n",SystemGlobalVar.TT_Offset.Angle_Y);			// 4 ������б��ƫ��ֵ
		printf("  3.����ֵ:%.2f��\r\n\r\n",SystemGlobalVar.AlarmLimit.Angle_X.AlarmUpperLimit);

		printf("��LoRaģ�����á�\r\n");
		printf("  1.ģ���ַ:%d\r\n",SystemGlobalVar.LoRa_Config.Addr);
		if(SystemGlobalVar.LoRa_Config.SerialInfMode==0)
			printf("  2.����ͨ��ģʽ��8N1(Ĭ��)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==1)
			printf("  2.����ͨ��ģʽ��801\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfMode==2)
			printf("  2.����ͨ��ģʽ��8E1\r\n");
				
		if(SystemGlobalVar.LoRa_Config.SerialInfBaud==0)
			printf("  3.���ڲ����ʣ�1200\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==1)
			printf("  3.���ڲ����ʣ�4800\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==2)
			printf("  3.���ڲ����ʣ�9600(Ĭ��)\r\n");
		else if(SystemGlobalVar.LoRa_Config.SerialInfBaud==3)
			printf("  3.���ڲ����ʣ�19200\r\n");
		else //if(SystemGlobalVar.LoRa_Config.SerialInfBaud==4)
			printf("  3.���ڲ����ʳ�������������\r\n");
				
		if(SystemGlobalVar.LoRa_Config.AirSpeed==0)
			printf("  4.������������Ӧ���������䣩\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==1)
			printf("  4.��������:1K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==2)
			printf("  4.��������:5K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==3)
			printf("  4.��������:10K(Ĭ��)\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==4)
			printf("  4.��������:50K\r\n");
		else if(SystemGlobalVar.LoRa_Config.AirSpeed==5)
			printf("4.��������:100K\r\n");
		//else if(SystemGlobalVar.LoRa_Config.AirSpeed==6)
		//	printf("4.��������:1M\r\n");
		//else if(SystemGlobalVar.LoRa_Config.AirSpeed==7)
		//printf("4.��������:2M\r\n");
				
		printf("  5.ͨ���ŵ�:%d\r\n",SystemGlobalVar.LoRa_Config.Channel);
		printf("  6.���书��:");
		key = SystemGlobalVar.LoRa_Config.Option & 0x03;
		if(key==0)
			printf("12dBm(Ĭ��)\r\n");
		else if(key==1)
			printf("10dBm\r\n");
		else if(key==2)
			printf("7dBm\r\n");
		else if(key==3)
			printf("4dBm\r\n");
	}while(res==0);
	printf("\r\n���ⰴ������\r\n");
	HyperTerminal_GetKey(0,0);  //����ʱ�ȴ��û�����
}

////ϵͳ��������
//void SystemFunctiongConfig(void)
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("a.ϵͳ��������\r\n");
//		printf("========================================================\r\n");
//		printf("1.������б����(%d):",SystemGlobalVar.SystemFunction.InclinationFun);
//		printf("2.΢������(%d):",SystemGlobalVar.SystemFunction.MeteoroFun);
//		printf("4.��������(%d):",SystemGlobalVar.SystemFunction.IceFun);
//		printf("5.ͼ����(%d):",SystemGlobalVar.SystemFunction.ImageFun);
//		printf("\r\n�޸������룺");
//		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '1':
//				printf("����1����0������:");
//				key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//			  switch(key)
//				{
//					case '0': SystemGlobalVar.SystemFunction.InclinationFun=0;break;
//					case '1': SystemGlobalVar.SystemFunction.InclinationFun=1;break;
//					default:  err=1;break;
//				}
//				break;
//			case '2':
//				printf("����1����0������:");
//				key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//			  switch(key)
//				{
//					case '0': SystemGlobalVar.SystemFunction.MeteoroFun=0;break;
//					case '1': SystemGlobalVar.SystemFunction.MeteoroFun=1;break;
//					default:  err=1;break;
//				}
//			  break;
//				case '3':
//				printf("����1����0������:");
//				key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//			  switch(key)
//				{
//					case '0': SystemGlobalVar.SystemFunction.IceFun=0;break;
//					case '1': SystemGlobalVar.SystemFunction.IceFun=1;break;
//					default:  err=1;break;
//				}
//			  break;
//				case '4':
//				printf("����1����0������:");
//				key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
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

void SystemFactoryResetConfig(void)		//�ָ���������
{
	uint8_t key;
	printf("\r\n\r\nȷ�ϻָ�?(y/n):");
	key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
	printf("\r\n\r\n");
	if(key=='y'||key=='Y') 
	{
			printf("\r\n\r\nϵͳ������λ...\r\n");	
			SystemGlobalVarInitToFlash();						//	�ָ���������
      printf("\r\nϵͳ������λ���\r\n");	
//		HAL_NVIC_SystemReset();
	}
}

////�����ʷ����
//void ClearingHistoricalDataConfig(void)
//{
//	uint8_t key;
//	printf("\r\n\r\n�Ƿ�Ҫ���������(y/n):");
//	key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//	printf("\r\n\r\n");
//	if(key=='y'||key=='Y') 
//	{
//			printf("\r\n\r\n��ʼ���������...\r\n");	
//			SystemFun.ClearingHistoricalData();							//	�����ʷ����
//			printf("\r\n�����ʷ�������\r\n");	
//	}
//}

////��������ֵ
//void TensionCorrectionConfig(void)
//{
//	uint8_t key;
//	uint8_t err=0;
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("d.��������ֵ\r\n");
//		printf("========================================================\r\n");
//		printf("1.��������ֵ:\r\n\r\n");
//		printf("2.�Զ�У��\r\n\r\n");
//		printf("��ǰ����(�˴���Ϊ��ֵ)��\r\n\r\n");
//		printf("\r\n�޸������룺");
//		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '1':
//				//�ȴ���������ֵ
//				break;
//			case '2':
//				//�Զ�����
//			  break;
//			default:err=1;
//				break;	
//		}
//	}while(err==0);
//}

////SIM����
//void SimScardConfig(void)
//{
//	uint8_t res,i,err=0;
//	uint8_t pbuff[20];
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("e.SIM����\r\n");
//		printf("========================================================\r\n");
//		for(i=0;i<20;i++)
//			printf("%c",SystemGlobalVar.PhoneNumber[i]);	
//		
//		printf("\r\n\r\n�޸������룺");
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

////������б����
//void InclinationConfig(void)
//{
//	uint8_t key;
//	uint8_t res,i,err=0;
//	uint8_t pbuff[20];
//	do
//	{
//		printf("\r\n\r\n========================================================\r\n");
//		printf("h.������б����\r\n");
//		printf("========================================================\r\n");
//		printf("ע1:������(�Ե�ǰλ��Ϊ���)\r\n");
//		printf("ע2:�������(�Գ����궨Ϊ���)\r\n");
//		//������û�����Ӿʹ�ӡ printf("δ���Ӵ�����\r\n");
//		
//		printf("\r\n\r\n�޸������룺\r\n");
//		printf("������0-�������,1-�����㣺\r\n");
//		res=WaitInput(pbuff,20,0);
//		key = HyperTerminal_GetKey(0,1);  //����ʱ�ȴ��û�����
//    printf("\r\n\r\n");
//		switch(key)
//		{
//			case '0':
//				//�������
//				break;
//			case '1':
//				//������
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
