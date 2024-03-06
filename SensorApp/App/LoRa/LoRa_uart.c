#include "LoRa_uart.h"
#include <string.h>


//���ڽ��ջص�������

char LoRa_buf[LORA_BUF_LEN];     // ���ջ���
uint32_t LoRa_cnt = 0;           // ���ռ���
uint8_t LoRa_rev_flag= 0,LoRa_rev_Timeout= 0,LoRa_rev_finish= 0;  // ����1���ռ���������1���ձ�־���ڽ�����ɱ�־
uint32_t Uart_Timeout_ms=0;
//void usart1_parameter_config(uint32_t baud); 
void LoRa_UartParaInit()
{
	LoRa_cnt = 0;           // ���ռ���
	LoRa_rev_flag= 0;
	LoRa_rev_Timeout= 0;
	LoRa_rev_finish= 0;
}


/**
  * @brief  ���ڷ����ַ���
  * @param  	str��Ҫ���͵�����
  *				  len�����ݳ���
  * @retval ��
	* @note   ��
  */
void LoRa_send_data(unsigned char *pbuf, unsigned short len)
{
	uint8_t count = 0;
	for(; count < len; count++)
	{
		//while(__HAL_UART_GET_FLAG(&LORA_Handler, UART_FLAG_TC)==0);		//ѭ������,ֱ��������� 
		while((LORA_Handler.Instance->ISR&0X40)==0);//ѭ������,ֱ���������	
		LORA_Handler.Instance->TDR = (uint8_t) pbuf[count];
	}
}

/**
  * @brief  ����1��������
  * @param  cmd������
  *				  res����Ҫ���ķ���ָ��
  *         timeOut ����ʱ��
  *         resnum ����ʹ���
  * @retval ��
	* @note   	0-�ɹ�	1-ʧ��
  */
uint8_t LoRa_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime)
{
	uint8_t i = 0;
	uint32_t timeout;
	LoRa_Buf_clear();										                // ��ջ���	
	for(i=0;i<retime;i++)
	{
	  timeout = timeOut;
		LoRa_send_data((unsigned char *)cmd, strlen((const char *)cmd));
		while(timeout--)
		{
			//LoRa_Rec_TimeOut_Process(); //���ڽ��ճ�ʱ����
			if(LoRa_Rec_TimeOut_Process())									// ������ڽ������
			{
				if(strstr((const char *)LoRa_buf, res) != NULL)		// ����������ؼ���
				{
//					printf("%s",lte_buf);
					return 0;
				}
			}	
			HAL_Delay(1);
		}
		LoRa_Buf_clear();										                // ��ջ���	
  }
	return 1;
}

/**
  * @brief  lte��ջ���
  * @param  ��
  * @retval ��
	* @note   ��
  */
void LoRa_Buf_clear(void)
{
	memset(LoRa_buf, 0, LORA_BUF_LEN);
	LoRa_cnt = 0;
}


/**
  * @brief  ���� �жϴ���
  * @param  ��
  * @retval ��
	* @note   ��
  */
void LoRa_Rec_interrupt_process(uint8_t data)
{
		LoRa_rev_flag 		= 1;                                     // ���� ���ձ�־
    LoRa_rev_Timeout = 0;	                                    	// ���ڽ��ռ�������	
	  //LoRa_buf[LoRa_cnt++] = LORA_Handler.Instance->RDR;    		// ���뻺������
		LoRa_buf[LoRa_cnt++] = data;    		// ���뻺������
		if(LoRa_cnt >= sizeof(LoRa_buf))
		{
			 LoRa_cnt = 0;                                    			// ��ֹ����������
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
//  * @brief  ��ʱ��3�жϴ��� 
//  * @param  ��
//  * @retval ��
//  */
//	//�ص���������ʱ���жϷ���������
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	static uint16_t tim_cnt = 0;                         // ��ʱ������
//	static uint8_t heart_beat_cnt = 0;                   // ��������
//	if(htim==(&TIM3_Handler))   													// ���TIM3�����жϷ������
//	{	
//		/* ���ڽ��մ��� */
//		if(LoRa_rev_flag)                                // ��� usart1 �������ݱ�־Ϊ1
//	  {
//			tim_cnt = 0;
//      heart_beat_cnt = 0;					                 // �����������ݹ���ʱ��������������
//		  LoRa_rev_cnt++;                                // usart1 ���ռ���	
//		  if(LoRa_rev_cnt >= 3)                          // ������ 3 ms δ���յ����ݣ�����Ϊ���ݽ�����ɡ�
//		  {
//			  LoRa_rev_finish = 1;
//				LoRa_rev_flag = 0;
//				LoRa_rev_cnt = 0;
//		  }
//	  }
//	}
//}
