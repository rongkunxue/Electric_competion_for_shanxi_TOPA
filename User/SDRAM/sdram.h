//-----------------------------------------------------------------
// ��������:
// 		 SDRAM��������ͷ�ļ�
// ��    ��: ���ǵ���
// ��ʼ����: 2018-08-04
// �������: 2018-08-04
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0: (2018-08-04)SDRAM��ʼ��
// ���Թ���: ����STM32F429+Cyclone IV����ϵͳ��ƿ����塢LZE_ST_LINK2
// ˵    ��: 
//    
//-----------------------------------------------------------------
#ifndef _SDRAM_H
#define _SDRAM_H
#include "stm32f429_winner.h"
//-----------------------------------------------------------------
// ����
//-----------------------------------------------------------------
extern SDRAM_HandleTypeDef SDRAM_Handler;// SDRAM���
//-----------------------------------------------------------------
// �궨��
//-----------------------------------------------------------------
#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) // SDRAM��ʼ��ַ

// SDRAM���ò���
#define SDRAM_MODEREG_BURST_LENGTH_1             ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((u16)0x0200)

//-----------------------------------------------------------------
// ��������
//-----------------------------------------------------------------
extern void SDRAM_Init(void);
extern void SDRAM_MPU_Config(void);
extern u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
extern void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
extern void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
extern void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);
#endif
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
