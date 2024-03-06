/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BATV_C_Pin GPIO_PIN_0
#define BATV_C_GPIO_Port GPIOB
#define ADXL355_CS_Pin GPIO_PIN_11
#define ADXL355_CS_GPIO_Port GPIOB
#define ADXL355_DRDY_Pin GPIO_PIN_12
#define ADXL355_DRDY_GPIO_Port GPIOB
#define RF_M2_ACTIN_Pin GPIO_PIN_13
#define RF_M2_ACTIN_GPIO_Port GPIOB
#define RF_M1_Pin GPIO_PIN_14
#define RF_M1_GPIO_Port GPIOB
#define RF_M0_Pin GPIO_PIN_15
#define RF_M0_GPIO_Port GPIOB
#define RF_NRST_Pin GPIO_PIN_8
#define RF_NRST_GPIO_Port GPIOA
#define RF_BUZY_Pin GPIO_PIN_11
#define RF_BUZY_GPIO_Port GPIOA
#define LORA_AUX_Pin GPIO_PIN_5
#define LORA_AUX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define DEBUG	1
#if (DEBUG)
	#define APP_LOG(fmt,...)		printf("[APP_LOG]"fmt, ##__VA_ARGS__)
	#define APP_PRINTF(fmt,...)	printf(fmt, ##__VA_ARGS__)
#else
	#define APP_LOG(fmt, ...)
	#define APP_PRINTF(fmt,...)
#endif

#define LORA_USING	1        //0：上海微所  1：亿佰特
#if(LORA_USING==0)
	#define SIMIT_TCM_LORA_USING		1
	#define E28_2G4T12S_LORA_USING	0
#else
	#define SIMIT_TCM_LORA_USING		0
	#define E28_2G4T12S_LORA_USING	1
#endif

#define STM_RTC_USING		1		//RTC 1:启用  0:不启用


#define SLEEP_TIME	3000		//3s
extern uint16_t SystemSleepTime;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
