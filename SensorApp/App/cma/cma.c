//=============================================================================
//=============================================================================
#include "cma.h"
#include "stdio.h"
#include "main.h"

struct CMD_TX_LING_DEF CMD_TxLine[MAX_TX_CMD];
struct CMD_TX_LING_DEF *pCMD_TxLine;

void CMA_talk_init(void)
{
	uint8_t i;
	for(i=0;i<MAX_TX_CMD-1;i++)
	{
		CMD_TxLine[i].CMD = 0;
		CMD_TxLine[i].TxCount = 0;
		CMD_TxLine[i].Interval = 0;
		CMD_TxLine[i].Next = &CMD_TxLine[i+1];
		CMD_TxLine[i+1].Prev = &CMD_TxLine[i];
	}
	CMD_TxLine[MAX_TX_CMD-1].CMD = 0;
	CMD_TxLine[MAX_TX_CMD-1].TxCount = 0;
	CMD_TxLine[MAX_TX_CMD-1].Interval = 0;
	CMD_TxLine[MAX_TX_CMD-1].Next = &CMD_TxLine[0];
	CMD_TxLine[0].Prev = &CMD_TxLine[MAX_TX_CMD-1];
	pCMD_TxLine =  &CMD_TxLine[0];
}

//=============================================================================
//功能：设置发送列表
//输入： cmd: 命令
//      count:  		0-删除   1-增加
//      interval:  	发送间隔
//			flag:   		-1：前面加入命令  0：清除当前命令   		1：后面加入命令
//      
//=============================================================================
void Set_cmdlist_struct(struct CMD_TX_LING_DEF *cmd_tx,int8_t flag)
{
	uint16_t i=0;
	struct CMD_TX_LING_DEF *pCMD;
	if(-1 == flag)  //
	{
		if(pCMD_TxLine->CMD)								//当前列表不为空，指前一个列表
			pCMD_TxLine = pCMD_TxLine->Prev;   //指前一个列表，优先发送
		pCMD_TxLine->CMD = cmd_tx->CMD;
		pCMD_TxLine->TxCount = cmd_tx->TxCount;
		pCMD_TxLine->Interval = cmd_tx->Interval;
		pCMD_TxLine->Interval_Flag = 1;
		pCMD_TxLine->len = cmd_tx->len;
		for(i=0;i<cmd_tx->len;i++)
		{
			pCMD_TxLine->SendBuf[i] = cmd_tx->SendBuf[i];
		}
	}
	else if(0 == flag)
	{
		if(cmd_tx==NULL)												//指令为空，清空当前列表
		{
			pCMD_TxLine->CMD = 0;
			pCMD_TxLine->TxCount = 0;
			pCMD_TxLine->Interval = 0;
			pCMD_TxLine->len=0;
			//printf("\r\n【指令清零】\r\n");
			if(pCMD_TxLine->Next->CMD)
				pCMD_TxLine = pCMD_TxLine->Next;  //指向下一个列表
		}
		else																//当前列表中更改
		{
			//printf("\r\n【当前列表插入指令】\r\n");
			pCMD_TxLine->CMD = cmd_tx->CMD;
			pCMD_TxLine->TxCount = cmd_tx->TxCount;
			pCMD_TxLine->Interval = cmd_tx->Interval;
			pCMD_TxLine->Interval_Flag = 1;
			pCMD_TxLine->len = cmd_tx->len;
			for(i=0;i<cmd_tx->len;i++)
			{
				pCMD_TxLine->SendBuf[i] = cmd_tx->SendBuf[i];
			}
		}
	}
	else //if(1 == flag)   //增加发送列表
	{
		pCMD = pCMD_TxLine;
		for(i=0;i<MAX_TX_CMD+1;i++)  //循环一次，查找空列表存放发送指令
		{
			if(pCMD->CMD==0)	break;
			else							pCMD = pCMD->Next;		
		}
		if(i==(MAX_TX_CMD+1)) return;   //查找不到空列表存放发送指令
		pCMD->CMD = cmd_tx->CMD;
		pCMD->TxCount = cmd_tx->TxCount;
		pCMD->Interval = cmd_tx->Interval;
		pCMD->Interval_Flag = 1;
		pCMD->len = cmd_tx->len;
		for(i=0;i<cmd_tx->len;i++)
		{
			pCMD->SendBuf[i] = cmd_tx->SendBuf[i];
		}
		if(pCMD_TxLine->CMD==0)								//当前列表为空，指向下一个列表
			pCMD_TxLine = pCMD_TxLine->Next;
	}
}

//功能：更新发送列表
void update_cmdlist(int8_t opt)
{
	if(-1==opt)
	{
		if(pCMD_TxLine->TxCount>0)
		{
			pCMD_TxLine->TxCount--;
		}
	}
	
	else if(0==opt)  pCMD_TxLine->TxCount = 0;
	
	if(pCMD_TxLine->TxCount==0)  //发送次数完成，指向下一个发送列表
	{
		pCMD_TxLine->CMD = 0;
		pCMD_TxLine->TxCount = 0;
		pCMD_TxLine->Interval = 0;
		pCMD_TxLine->Interval_Flag = 0;
		pCMD_TxLine = pCMD_TxLine->Next;
		//printf("\r\n【当前列表指令清零指向下一个发送列表】\r\n");
	}
}

void Delete_cmdlist(uint8_t cmd)
{
	if(pCMD_TxLine->CMD==cmd)
	{
		Set_cmdlist_struct(NULL,0);	//清除发送列表，避免重发
	}
}

//=============================================================================
//功能：查询命令表，进行报文上送
//=============================================================================
void cma_talk(void)
{
	uint8_t *src=NULL;													//报文内容
	uint16_t srcLen=0;													//报文内容长度
	uint8_t frameType, packetType, frameNo;			//报文参数
	uint16_t crc16=0;														//报文校验位
	uint16_t Interval;
	
	static uint32_t nowTime=0;										//当前时间
	//if(pCMD_TxLine->TxCount)			//大于1说明有数据需要发送
	if(pCMD_TxLine->CMD)						//大于1说明有数据需要发送
	{
		if(pCMD_TxLine->Interval_Flag)			//不同报文之间的发送隔间为 INTERVAL_FIRST
		{
			pCMD_TxLine->Interval_Flag = 0;
			Interval = INTERVAL_FIRST;
		}
		else	Interval = pCMD_TxLine->Interval;  //报文发送间隔
	  
		if( (HAL_GetTick()-nowTime)>=Interval)		//
		{
				APP_LOG("【LORA发送】\r\n");
//				I1_SendData(pCMD_TxLine->msgType, pCMD_TxLine->SendBuf, pCMD_TxLine->len, pCMD_TxLine->frmType);
				APP_LOG("【LORA发送结束】\r\n");
				update_cmdlist(-1); 		//更新发送列表	
			nowTime = HAL_GetTick();
		}
	}
}








