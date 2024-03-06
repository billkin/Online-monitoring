#ifndef __LORA_uart_H
#define __LORA_uart_H

#include "main.h"
#include "usart.h"

#define LORA_Handler huart1 				//UART句柄
#define LORA_UART 		USART1
////IO操作函数											   
#define LORA_BUF_LEN	128

extern char LoRa_buf[LORA_BUF_LEN];	// 接收缓存
extern uint32_t LoRa_cnt;           // 接收计数
extern uint8_t LoRa_rev_flag,LoRa_rev_Timeout,LoRa_rev_finish;

uint8_t LoRa_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime);
void LoRa_send_data(unsigned char *pbuf, unsigned short len);          
void LoRa_Buf_clear(void);   
void LoRa_Rec_interrupt_process(uint8_t data);   //在usart.c中的回调函数HAL_UART_RxCpltCallback中调用
uint8_t LoRa_Rec_TimeOut_Process(void);
void LoRa_UartParaInit(void);

#endif   
