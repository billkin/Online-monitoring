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
//���ܣ����÷����б�
//���룺 cmd: ����
//      count:  		0-ɾ��   1-����
//      interval:  	���ͼ��
//			flag:   		-1��ǰ���������  0�������ǰ����   		1�������������
//      
//=============================================================================
void Set_cmdlist_struct(struct CMD_TX_LING_DEF *cmd_tx,int8_t flag)
{
	uint16_t i=0;
	struct CMD_TX_LING_DEF *pCMD;
	if(-1 == flag)  //
	{
		if(pCMD_TxLine->CMD)								//��ǰ�б�Ϊ�գ�ָǰһ���б�
			pCMD_TxLine = pCMD_TxLine->Prev;   //ָǰһ���б����ȷ���
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
		if(cmd_tx==NULL)												//ָ��Ϊ�գ���յ�ǰ�б�
		{
			pCMD_TxLine->CMD = 0;
			pCMD_TxLine->TxCount = 0;
			pCMD_TxLine->Interval = 0;
			pCMD_TxLine->len=0;
			//printf("\r\n��ָ�����㡿\r\n");
			if(pCMD_TxLine->Next->CMD)
				pCMD_TxLine = pCMD_TxLine->Next;  //ָ����һ���б�
		}
		else																//��ǰ�б��и���
		{
			//printf("\r\n����ǰ�б����ָ�\r\n");
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
	else //if(1 == flag)   //���ӷ����б�
	{
		pCMD = pCMD_TxLine;
		for(i=0;i<MAX_TX_CMD+1;i++)  //ѭ��һ�Σ����ҿ��б��ŷ���ָ��
		{
			if(pCMD->CMD==0)	break;
			else							pCMD = pCMD->Next;		
		}
		if(i==(MAX_TX_CMD+1)) return;   //���Ҳ������б��ŷ���ָ��
		pCMD->CMD = cmd_tx->CMD;
		pCMD->TxCount = cmd_tx->TxCount;
		pCMD->Interval = cmd_tx->Interval;
		pCMD->Interval_Flag = 1;
		pCMD->len = cmd_tx->len;
		for(i=0;i<cmd_tx->len;i++)
		{
			pCMD->SendBuf[i] = cmd_tx->SendBuf[i];
		}
		if(pCMD_TxLine->CMD==0)								//��ǰ�б�Ϊ�գ�ָ����һ���б�
			pCMD_TxLine = pCMD_TxLine->Next;
	}
}

//���ܣ����·����б�
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
	
	if(pCMD_TxLine->TxCount==0)  //���ʹ�����ɣ�ָ����һ�������б�
	{
		pCMD_TxLine->CMD = 0;
		pCMD_TxLine->TxCount = 0;
		pCMD_TxLine->Interval = 0;
		pCMD_TxLine->Interval_Flag = 0;
		pCMD_TxLine = pCMD_TxLine->Next;
		//printf("\r\n����ǰ�б�ָ������ָ����һ�������б�\r\n");
	}
}

void Delete_cmdlist(uint8_t cmd)
{
	if(pCMD_TxLine->CMD==cmd)
	{
		Set_cmdlist_struct(NULL,0);	//��������б������ط�
	}
}

//=============================================================================
//���ܣ���ѯ��������б�������
//=============================================================================
void cma_talk(void)
{
	uint8_t *src=NULL;													//��������
	uint16_t srcLen=0;													//�������ݳ���
	uint8_t frameType, packetType, frameNo;			//���Ĳ���
	uint16_t crc16=0;														//����У��λ
	uint16_t Interval;
	
	static uint32_t nowTime=0;										//��ǰʱ��
	//if(pCMD_TxLine->TxCount)			//����1˵����������Ҫ����
	if(pCMD_TxLine->CMD)						//����1˵����������Ҫ����
	{
		if(pCMD_TxLine->Interval_Flag)			//��ͬ����֮��ķ��͸���Ϊ INTERVAL_FIRST
		{
			pCMD_TxLine->Interval_Flag = 0;
			Interval = INTERVAL_FIRST;
		}
		else	Interval = pCMD_TxLine->Interval;  //���ķ��ͼ��
	  
		if( (HAL_GetTick()-nowTime)>=Interval)		//
		{
				APP_LOG("��LORA���͡�\r\n");
//				I1_SendData(pCMD_TxLine->msgType, pCMD_TxLine->SendBuf, pCMD_TxLine->len, pCMD_TxLine->frmType);
				APP_LOG("��LORA���ͽ�����\r\n");
				update_cmdlist(-1); 		//���·����б�	
			nowTime = HAL_GetTick();
		}
	}
}








