/***********************************************************************
文件名称：CMA.h
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/

#ifndef __CMA_H__
#define __CMA_H__

#include "stdint.h"
#include "string.h"

#define	 LENGTH(DATA)	    (sizeof(DATA)/sizeof(char))

//#define	 P_TO_P(A,B)	     memcpy(A,	  B,	  LENGTH(B))
//#define	 P_TO_D(A,B)	     memcpy((int8 *)&A, B,	  LENGTH(B))
//#define	 D_TO_P(A,B)	     memcpy(A,	  (int8 *)&B, LENGTH(B))
//#define	 D_TO_D(A,B)	     (A = B)//memcpy((int8 *)&A, (int8 *)&B, LENGTH(B))

//=============================================================================
//=============================================================================
//typedef enum {COMMAND_SUCCESS = 0xFF, COMMAND_FAIL = 0X00} Command;
//typedef enum {READ_CONFIG = 0, SET_CONFIG = 1} Config;
////typedef enum {NORMAL = 0, DEBUG = 1} Work_Mode;
//typedef enum {BLACKWHITE = 0, CLOLOR = 1} Color_Select;
//typedef enum {C320_240=1,C640_480=2,C704_576=3,C720_480=4,C1280_720=5,C1920_1080=6,C2560_1440=7,C3840_2160=8,OTHET } Color_Resolution;

//=============================================================================
//=============================================================================
//#define         LEN_START               (sizeof(struct protocol_start)/sizeof(char))            
//#define         LEN_END                 (sizeof(struct protocol_end)/sizeof(char))      
//        
//#define         LEN_MIN                 0
//#define         LEN_MAX                 (1417-LEN_START-LEN_END)

//=============================================================================
#define MAX_TX_CMD	      5
#define INTERVAL_FIRST		50		//不同报文之间发送间隔，单位：ms

struct CMD_TX_LING_DEF
{
	uint8_t  CMD;	      	//发送命令
	uint8_t  TxCount;	  	//发送次数
	uint16_t  Interval;	  //发送间隔
	uint8_t		Interval_Flag;	  //发送间隔
	uint8_t  SendBuf[256];
	uint8_t  len;
//	uint8_t  msgType;
//	uint8_t  frmType;
//	uint8_t  id_state;
	struct CMD_TX_LING_DEF *Prev;  //指向上一个发送命令结构体
	struct CMD_TX_LING_DEF *Next;  //指向下一个发送命令结构体
};

extern struct CMD_TX_LING_DEF CMD_TxLine[MAX_TX_CMD];
extern struct CMD_TX_LING_DEF *pCMD_TxLine;


void CMA_talk_init(void);
//void Set_cmdlist(uint8_t cmd,uint8_t count, uint8_t interval,uint8_t *buf,uint8_t len,int8_t flag);
void Set_cmdlist_struct(struct CMD_TX_LING_DEF *cmd_tx,int8_t flag);
void update_cmdlist(int8_t opt);	
void Delete_cmdlist(uint8_t cmd);

void cma_talk(void);

#endif

