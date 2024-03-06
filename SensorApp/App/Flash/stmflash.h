#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "main.h"

//0x0800 F800 - 0x0800 FFFF 2 K Page 31
#define FLASH_SAVE_ADDR  0x0800F800 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define FLASH_SAVE_SIZE  256 				//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
//#define STM32_FLASH_SIZE 	128 	 		    //所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 	1              	//使能FLASH写入(0，不是能;1，使能)
//#define FLASH_WAITETIME  	50000          //FLASH等待超时时间

//FLASH起始地址
//#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH的起始地址

//uint8_t STMFLASH_GetStatus(void);				  //获得状态
//uint8_t STMFLASH_WaitDone(uint16_t time);				  //等待操作结束
//uint8_t STMFLASH_ErasePage(uint32_t paddr);			  //擦除页
//uint8_t STMFLASH_WriteHalfWord(uint32_t faddr, uint16_t dat);//写入半字
//uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		  //读出半字  
//void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//指定地址开始写入指定长度的数据
//uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);										//指定地址开始读取指定长度数据
//void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
//void Test_Write(uint32_t WriteAddr,uint16_t WriteData);	
void STM_FLASH_Save(uint32_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);

#endif

















