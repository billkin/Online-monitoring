/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SystemGlobalVar.h"
#include "LowPowerMode.h"
#include "SIMIT_TCM_LoRa.h"
#include "E28_2G4T12S_LoRa.h"
#include "ADXL355.h"
#include "ADC_App.h"
#include "timestamp.h"
#include "HyperTerminal.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t SystemSleepTime=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Sensor_TimeSynchronization()
{
	static uint32_t Timeout=0;
	static uint8_t count=0;
	if(Timeout==0)
	{
		Timeout = HAL_GetTick();
		SensorControl(Timing_CTL,0,NULL,0);
		count++;
	}
	else if(count<5)
	{
		if((SystemGlobalVar.Retry & CMD_TIMING_END) != CMD_TIMING_END)
		{
			if(HAL_GetTick()-Timeout >=500) Timeout=0;
		}
		else
		{
				count = 5;
			  SystemGlobalVar.Retry = 0;
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//uint16_t i,maintime=0;
	//struct ADXL355_Typedef ADXL355;
//	int16_t temp_int16;
	uint32_t temp_u32;
	uint32_t NowTime=0;
//	uint8_t StatuUpFlag=1;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  //SET_BIT(SYSCFG->CFGR1, 1<<9);   	//PB15 and PA8 pull-down configuration strobe
																												//Setting this bit disables (disconnects) the internal pull-down resistors	
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	LoRa_InterSleep(NULL);
	if(CheckIsStandby())    		//
	{
		if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)==0X99cc) 
		{
			HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0X00);//标记已经初始化过了
			SystemIntoStandbyMode();
		}
		SystemGlobalVar_init();
		SPI_SetSpeed(&XL355_hspi,SPI_BAUDRATEPRESCALER_4); //设置为21M时钟,高速模式
		HAL_Delay(10);
		ADXL355_Reset_Sensor();
		ADXL355_Init();
		if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0)!=0X55AA) RTC_Set_Time(SystemGlobalVar.BJ_Time);
		HyperTerminal_Menu(2000);  //系统上电提供配置接口，2s超时。系统唤醒不提供配置接口
		SystemGlobalVar_init();
		SystemGlobalVar.PowerOnTimeStamps = RTC_Get_Stamp();
		SystemConfigSave();      //保存配置
		ADXL355_Reset_Sensor();
		ADXL355_Init();
		__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
		HAL_RTCEx_SetWakeUpTimer(&hrtc, SystemGlobalVar.SamplingPeriod-1, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	}
	else
	{
	 SystemGlobalVar_init();
		//if(RTC_Irq_Flag)
	 if(__HAL_RTC_WAKEUPTIMER_GET_FLAG(&hrtc, RTC_FLAG_WUTF) != RESET)
	 {
		 //RTC_wakeup_flag = 1;
		 RTC_Irq_Flag = 0;
		 APP_LOG("RTC wakeup.\r\n");
		 __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
		 HAL_RTCEx_SetWakeUpTimer(&hrtc, SystemGlobalVar.SamplingPeriod-1, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	 }
	 else
			APP_LOG("WKUP pin wakeup\r\n");
	}
	IWDG_Init(IWDG_PRESCALER_64,3000);  	//分频数为8,重载值为500,溢出时间为1s
	IWDG_Feed();    			//喂狗
	SPI_SetSpeed(&XL355_hspi,SPI_BAUDRATEPRESCALER_4); //设置为21M时钟,高速模式
	SystemGlobalVar.TimeStamp = RTC_Get_Stamp();
	if((SystemGlobalVar.TimeStamp-SystemGlobalVar.PowerOnTimeStamps)>=SystemGlobalVar.ResetTimeStamps)
	{
			APP_LOG("硬件复位时间到，重启硬件\r\n");
		  HAL_Delay(5);
			HAL_NVIC_SystemReset();
	}
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
	IWDG_Feed();    			//喂狗
	ADXL355_ACT_Set();
	LoRa_WriteConfigPara(SystemGlobalVar.LoRa_Config);
	temp_u32 = LoRa_ParaToBaud(SystemGlobalVar.LoRa_Config.SerialInfBaud);
	set_baudrate(&LORA_Handler,temp_u32);
	LoRa_Init();				//LoRa上电初始化
	HAL_Delay(10);
	NowTime = HAL_GetTick();
	SystemGlobalVar.Retry=3;  //3次重发
	SystemGlobalVar.SamplingFlg=0;	
	SystemSleepTime = 0;
	int  rand_ms; //0~32768
	
	LoRa_Buf_clear();
	NowTime = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		IWDG_Feed();    			//喂狗
//		if(StatuUpFlag)
//		{
//			StatuUpFlag = 0;
//			//SensorStatusUp(1);				 //传感器状态量上传给汇聚节点
//			
//		}
		
		LoRa.RxLen = LORA_BUF_LEN;
		if(LoRa_DataPro(LoRa.RxData,&LoRa.RxLen)==1)	//对接收到的数据进行处理
		{
			SensorMessagePro(LoRa.RxData,LoRa.RxLen);
		}
		
		if((SystemGlobalVar.Retry&CMD_CONTROL)==CMD_CONTROL)  //收到控制命令
		{
			APP_LOG("收到控制指令\r\n");
			SystemGlobalVar.Retry &= (~CMD_CONTROL);
			SystemGlobalVar.Retry |= CMD_CONTROL_WAIT;
			NowTime = HAL_GetTick();
			SystemSleepTime = 0;
		}
		else if((SystemGlobalVar.Retry&CMD_CONTROL_WAIT)==CMD_CONTROL_WAIT)  //控制指令处理间隙
		{
			if( (HAL_GetTick()-NowTime)>=500)		//1S
			{
				SystemGlobalVar.Retry &= (~CMD_CONTROL_WAIT);
				if(SystemGlobalVar.Retry&0x0f) SystemGlobalVar.Retry = 3;				//3次发送
				NowTime = HAL_GetTick();
				APP_LOG("退出控制指令等待时间\r\n");
			}
		}
		else if((SystemGlobalVar.Retry==0)||((SystemGlobalVar.Retry & CMD_TIMING_END) == CMD_TIMING_END))		  //数据发送完成
		{
			Sensor_TimeSynchronization();
			Sleep_Opt();																								//超时进入低功耗模式
		}
		#if 1
		else if((SystemGlobalVar.Retry&CMD_RETRY)==CMD_RETRY)		//重发
		{
			if((SystemGlobalVar.Retry&0x0f) > 1) 
			{
				if( (HAL_GetTick()-NowTime)>=rand_ms)				//0.5s~3.8S
				{
					//APP_LOG("重发时间间隔:%dms\r\n",500+rand_ms);
					NowTime = HAL_GetTick();
					rand_ms = abs(rand())%2000+500; 		//随机数 500~2499
					//APP_LOG("随机延时时间:%dms\r\n",rand_ms);
					SystemGlobalVar.Retry--;
					SystemGlobalVar.SamplingFlg = 1;
				}
			}
			else 
			{
				SystemGlobalVar.Retry &= (~CMD_RETRY);
				SystemGlobalVar.Retry = 0; //到达发送次数上限，取消重发
				SystemSleepTime = 0;
			}
		}
		//if( (HAL_GetTick()-NowTime)>=SystemGlobalVar.SamplingPeriod*60000)		//
		else if(SystemGlobalVar.Retry&0x0f)					
		{
			if( (HAL_GetTick()-NowTime)>=500)		//0.5S
			{
				NowTime = HAL_GetTick();
				rand_ms = abs(rand())%2000+500; 	//随机数 500~2499
				//APP_LOG("随机延时时间:%dms\r\n",rand_ms);
				//ADXL355_Data_Scan(&ADXL355);
				//Read_Bat_Voltage();  //电池电压*100
				//RTC_Get_Stamp();
				SystemGlobalVar.SamplingFlg = 1;					//
				SystemGlobalVar.Retry |= CMD_RETRY;				//重发
				//SystemGlobalVar.Retry += 3;								//重发5次
			}
		}
		
		#endif
		
		if(SystemGlobalVar.SamplingFlg)								//采集时间到
		{
			SystemGlobalVar.SamplingFlg=0;
			//SensorMonitoringDataUp(1);				 						//监测数据报文上传给汇聚节点
			SensorMonitoringDataAndStatusUp();							//监测数据报文与传感器状态量上传给汇聚节点
			LoRa_Buf_clear();
			NowTime = HAL_GetTick();
			rand_ms = abs(rand())%2000+500; 		//随机数 500~2499
			//APP_LOG("随机延时时间:%dms\r\n",rand_ms);
		}

		//if(SystemGlobalVar.Retry == 0x10)		//重发
		//	Sleep_Opt();											//超时进入低功耗模式
		//HAL_Delay(1);												//系统滴答时钟每1ms循环一次
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HSE_SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
