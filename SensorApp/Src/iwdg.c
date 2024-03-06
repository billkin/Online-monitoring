/**
  ******************************************************************************
  * @file    iwdg.c
  * @brief   This file provides code for the configuration
  *          of the IWDG instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "iwdg.h"

/* USER CODE BEGIN 0 */
uint8_t WDG_Open_Flag = 0;
/* USER CODE END 0 */

IWDG_HandleTypeDef hiwdg;

/* IWDG init function */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */
	#if 0
  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */
	#endif
  /* USER CODE END IWDG_Init 2 */

}

/* USER CODE BEGIN 1 */
//��ʼ���������Ź�
//prer:��Ƶ��:IWDG_PRESCALER_4~IWDG_PRESCALER_256
//rlr:�Զ���װ��ֵ,0~0XFFF.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).
void IWDG_Init(uint8_t prer,uint16_t rlr)
{
    hiwdg.Instance=IWDG;
    hiwdg.Init.Prescaler=prer;	//����IWDG��Ƶϵ��
		hiwdg.Init.Window = rlr;
    hiwdg.Init.Reload=rlr;		//��װ��ֵ
    HAL_IWDG_Init(&hiwdg);		//��ʼ��IWDG,Ĭ�ϻῪ���������Ź�	
		WDG_Open_Flag = 1;
}
//ι�������Ź�
void IWDG_Feed(void)
{   
  if(WDG_Open_Flag) 
		HAL_IWDG_Refresh(&hiwdg); 	//ι��
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
