#include "LowPowerMode.h"
#include "stdio.h"
#include "main.h"
#include "rtc.h"
#include "timestamp.h"
#include "E28_2G4T12S_LoRa.h"
#include "SystemGlobalVar.h"
#include "ADXL355.h"
//void Sys_Standby(void)
//{  
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
//	PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
//	PWR_EnterSTANDBYMode();	  //进入待命（STANDBY）模式 		 
//}
void SleepMode_Measure(void)
{
  //关闭系统时钟中断
  /* 挂起滴答定时器增加以避免滴答定时器中断唤醒睡眠模式 */
  HAL_SuspendTick();
  /* 进入睡眠模式， 任意中断唤醒 */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  
  /* 恢复系统时钟中断 */
  HAL_ResumeTick();

  /* Add a delay of 2 second after exit from Sleep mode */
  HAL_Delay(200);
}

void StopMode_Measure(void)
{
  /* 挂起滴答定时器增加以避免滴答定时器中断唤醒睡眠模式 */
    HAL_SuspendTick();
  /* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒*/
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI); 
  /* 恢复系统时钟中断 */
  HAL_ResumeTick();

  /* Add a delay of 2 second after exit from Sleep mode */
  HAL_Delay(100);
}


void SYSCLKConfig_STOP(void)
{
  /* 使能 HSE */
 // __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
	__HAL_RCC_HSI_ENABLE();

  /* 等待 HSI 准备就绪 */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET);
  
  /* 使能 PLL */ 
  __HAL_RCC_PLL_ENABLE();

  /* 等待 PLL 准备就绪 */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* 选择PLL作为系统时钟源 */
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

  /* 等待PLL被选择为系统时钟源 */
  while(__HAL_RCC_GET_SYSCLK_SOURCE() != 0x08)
  {
  }
}

#if 1
int8_t CheckIsStandby(void)
{
	/* 检测系统是否是从待机模式启动的 */ 
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
   //APP_LOG("Standby awaken\r\n");
    /* 清除待机标志位 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		return 0;
  }
  else
  {
    //APP_LOG("Systems Power On\r\n");
		//StandbyMode();
		return 1;
  }
}

void StandbyMode(void)
{
//	__HAL_RCC_PWR_CLK_ENABLE();
	/* 禁用所有唤醒源: 唤醒引脚PB5 */
//   HAL_PWR_DisableWakeUpPin(PWR_FLAG_WUF);
//   __HAL_RCC_APB2_FORCE_RESET();
//	 __HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_DisableWakeUpPin(PWR_FLAG_WUF);//禁用所有使用的唤醒源:PWR_WAKEUP_PIN1 connected to PA.00

    /* 清除所有唤醒标志位 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF);
    /* RTC clock enable */
    __HAL_RCC_APB2_FORCE_RESET();       //复位所有IO口
//		__HAL_RCC_SPI2_FORCE_RESET();
//		__HAL_RCC_USART1_FORCE_RESET();
//		__HAL_RCC_USART2_FORCE_RESET();
//		__HAL_RCC_ADC_FORCE_RESET();
		__HAL_RCC_PWR_CLK_ENABLE();         //使能PWR时钟 
   /* 使能唤醒引脚：系统唤醒输入 */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN);
 
   /* 进入待机模式 */
   HAL_PWR_EnterSTANDBYMode();
}
#endif


void Sleep_Opt()
{
	if(SystemSleepTime==0) SystemSleepTime=HAL_GetTick();
	HAL_GetTick();
	if(HAL_GetTick()-SystemSleepTime >=SLEEP_TIME)
	{
		//APP_LOG("SystemReset\r\n");
		HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0X99CC);//标记已经初始化过了
		HAL_Delay(1);
		HAL_NVIC_SystemReset();
	}
}

void SystemIntoStandbyMode()
{
		APP_LOG("System Enter STANDBY Mode\r\n\r\n\r\n");
		HAL_Delay(20);
		StandbyMode();
		SystemSleepTime=0;
}
