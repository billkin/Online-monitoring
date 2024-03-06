#ifndef __LORA_uart_H
#define __LORA_uart_H

#include "main.h"
#include "usart.h"

#define LORA_Handler huart1 				//UART���
#define LORA_UART 		USART1
////IO��������											   
#define LORA_BUF_LEN	128

extern char LoRa_buf[LORA_BUF_LEN];	// ���ջ���
extern uint32_t LoRa_cnt;           // ���ռ���
extern uint8_t LoRa_rev_flag,LoRa_rev_Timeout,LoRa_rev_finish;

uint8_t LoRa_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime);
void LoRa_send_data(unsigned char *pbuf, unsigned short len);          
void LoRa_Buf_clear(void);   
void LoRa_Rec_interrupt_process(uint8_t data);   //��usart.c�еĻص�����HAL_UART_RxCpltCallback�е���
uint8_t LoRa_Rec_TimeOut_Process(void);
void LoRa_UartParaInit(void);

#endif   
