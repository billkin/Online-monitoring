#ifndef _ADC_APP_H_
#define _ADC_APP_H_

#include "adc.h"
#include "main.h"

#define BAT_HADC	hadc1

#define BAT_CHECK_ENABLE()	HAL_GPIO_WritePin(BATV_C_GPIO_Port,BATV_C_Pin,GPIO_PIN_SET)
#define BAT_CHECK_DISABLE()	HAL_GPIO_WritePin(BATV_C_GPIO_Port,BATV_C_Pin,GPIO_PIN_RESET)

float Read_Bat_Voltage(void);  //µÁ—π

#endif

