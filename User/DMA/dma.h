//-----------------------------------------------------------------
// ��������:
//     DMA��������
// ��    ��: ���ǵ���
// ��ʼ����: 2018-08-04
// �������: 2018-08-04
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0: (2018-08-04)DMA���úʹ���
// ���Թ���: ����STM32F429+CycloneIV����ϵͳ��ƿ����塢LZE_ST_LINK2
// ˵    ��: 
//    
//-----------------------------------------------------------------
#ifndef __DMA_H
#define __DMA_H
#include "stm32f429_winner.h"
//-----------------------------------------------------------------
// ����
//-----------------------------------------------------------------
extern DMA_HandleTypeDef  ADC1DMA_Handler;      // DMA���

//-----------------------------------------------------------------
// ��������
//-----------------------------------------------------------------
extern void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
extern void MYDMA_ADC_Transmit(ADC_HandleTypeDef *hadc, uint32_t *pData, uint16_t Size);

#endif
//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 