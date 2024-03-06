#include "LoRa_uart.h"
#include <string.h>


//串口接收回调函数在

char LoRa_buf[LORA_BUF_LEN];     // 接收缓存
uint32_t LoRa_cnt = 0;           // 接收计数
uint8_t LoRa_rev_flag= 0,LoRa_rev_Timeout= 0,LoRa_rev_finish= 0;  // 串口1接收计数，串口1接收标志串口接收完成标志
uint32_t Uart_Timeout_ms=0;
//void usart1_parameter_config(uint32_t baud); 
void LoRa_UartParaInit()
{
	LoRa_cnt = 0;           // 接收计数
	LoRa_rev_flag= 0;
	LoRa_rev_Timeout= 0;
	LoRa_rev_finish= 0;
}


/**
  * @brief  串口发送字符串
  * @param  	str：要发送的数据
  *				  len：数据长度
  * @retval 无
	* @note   无
  */
void LoRa_send_data(unsigned char *pbuf, unsigned short len)
{
	uint8_t count = 0;
	for(; count < len; count++)
	{
		//while(__HAL_UART_GET_FLAG(&LORA_Handler, UART_FLAG_TC)==0);		//循环发送,直到发送完毕 
		while((LORA_Handler.Instance->ISR&0X40)==0);//循环发送,直到发送完毕	
		LORA_Handler.Instance->TDR = (uint8_t) pbuf[count];
	}
}

/**
  * @brief  串口1发送命令
  * @param  cmd：命令
  *				  res：需要检查的返回指令
  *         timeOut 返回时间
  *         resnum 命令发送次数
  * @retval 无
	* @note   	0-成功	1-失败
  */
uint8_t LoRa_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime)
{
	uint8_t i = 0;
	uint32_t timeout;
	LoRa_Buf_clear();										                // 清空缓存	
	for(i=0;i<retime;i++)
	{
	  timeout = timeOut;
		LoRa_send_data((unsigned char *)cmd, strlen((const char *)cmd));
		while(timeout--)
		{
			//LoRa_Rec_TimeOut_Process(); //串口接收超时调度
			if(LoRa_Rec_TimeOut_Process())									// 如果串口接收完成
			{
				if(strstr((const char *)LoRa_buf, res) != NULL)		// 如果检索到关键词
				{
//					printf("%s",lte_buf);
					return 0;
				}
			}	
			HAL_Delay(1);
		}
		LoRa_Buf_clear();										                // 清空缓存	
  }
	return 1;
}

/**
  * @brief  lte清空缓存
  * @param  无
  * @retval 无
	* @note   无
  */
void LoRa_Buf_clear(void)
{
	memset(LoRa_buf, 0, LORA_BUF_LEN);
	LoRa_cnt = 0;
}


/**
  * @brief  串口 中断处理
  * @param  无
  * @retval 无
	* @note   无
  */
void LoRa_Rec_interrupt_process(uint8_t data)
{
		LoRa_rev_flag 		= 1;                                     // 串口 接收标志
    LoRa_rev_Timeout = 0;	                                    	// 串口接收计数清零	
	  //LoRa_buf[LoRa_cnt++] = LORA_Handler.Instance->RDR;    		// 存入缓存数组
		LoRa_buf[LoRa_cnt++] = data;    		// 存入缓存数组
		if(LoRa_cnt >= sizeof(LoRa_buf))
		{
			 LoRa_cnt = 0;                                    			// 防止数据量过大
		}
}

uint8_t LoRa_Rec_TimeOut_Process()
{
	uint8_t res;
	res = 0;
	if(LoRa_rev_flag)
	{
		if(LoRa_rev_Timeout==0) 
		{
			Uart_Timeout_ms = HAL_GetTick();
			LoRa_rev_Timeout++;
		}
		else if(HAL_GetTick()-Uart_Timeout_ms>=3)
		{
			LoRa_rev_flag=0;
			res = 1;
		}
	}
	return res;
}

///**
//  * @brief  定时器3中断处理 
//  * @param  无
//  * @retval 无
//  */
//	//回调函数，定时器中断服务函数调用
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	static uint16_t tim_cnt = 0;                         // 定时器计数
//	static uint8_t heart_beat_cnt = 0;                   // 心跳计数
//	if(htim==(&TIM3_Handler))   													// 检查TIM3更新中断发生与否
//	{	
//		/* 串口接收处理 */
//		if(LoRa_rev_flag)                                // 如果 usart1 接收数据标志为1
//	  {
//			tim_cnt = 0;
//      heart_beat_cnt = 0;					                 // 当串口有数据过来时，心跳计数清零
//		  LoRa_rev_cnt++;                                // usart1 接收计数	
//		  if(LoRa_rev_cnt >= 3)                          // 当超过 3 ms 未接收到数据，则认为数据接收完成。
//		  {
//			  LoRa_rev_finish = 1;
//				LoRa_rev_flag = 0;
//				LoRa_rev_cnt = 0;
//		  }
//	  }
//	}
//}
