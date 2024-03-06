#include "stmflash.h"

//#include "SystemGlobalVar.h"

static uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint8_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(__IO uint8_t*)faddr; 
}

#if STM32_FLASH_WREN	//如果使能了写   

//1 page = 2KB  
void STM_FLASH_Save(uint32_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	uint16_t i,j,DataSize;
	uint32_t PageError;
	uint8_t FlashBuf[FLASH_SAVE_SIZE]; //最多是2K字节
	uint64_t *ProgramData;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_FLASH_Unlock();    //解锁
	//DataSize=FLASH_SAVE_SIZE>>1
	DataSize=FLASH_SAVE_SIZE;
	STMFLASH_Read(FLASH_SAVE_ADDR,FlashBuf,DataSize);  //读出FLASH_SAVE_SIZE个字节 FLASH_SAVE_SIZE/2个双字节
	
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page        = GetPage(FLASH_SAVE_ADDR);
	EraseInitStruct.NbPages     = 1;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError); 
	//8*6=48,
  for(i=0;i<FLASH_SAVE_SIZE;i++)	   //只保存FLASH_SAVE_SIZE个字节
	{
		if(FLASH_SAVE_ADDR+i>=WriteAddr)
		{
			for(j=0;j<NumToWrite;j++)FlashBuf[i+j]=pBuffer[j];
			break;
		}
	}
	DataSize=FLASH_SAVE_SIZE/8;
	for(i=0;i<DataSize;i++)
	{
		ProgramData = (uint64_t*)&pBuffer[i*8];
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_SAVE_ADDR+i*8, *ProgramData);
	}
	HAL_FLASH_Lock();
}

#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(uint32_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取1个字节.
		ReadAddr+=1;//偏移1个字节.	
	}
}

////////////////////////////////////////////测试用///////////////////////////////////////////
////WriteAddr:起始地址
////WriteData:要写入的数据
//void Test_Write(uint32_t WriteAddr,uint16_t WriteData)   	
//{
//	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
//}
















