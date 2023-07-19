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

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "dma.h"
#include "lcd.h"
#include "adc.h"
//-----------------------------------------------------------------

DMA_HandleTypeDef  ADCDMA_Handler;      // DMA���
extern ADC_HandleTypeDef ADC_Handler;
//-----------------------------------------------------------------
// void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
//-----------------------------------------------------------------
// 
// ��������: DMAx�ĸ�ͨ������
// ��ڲ���: DMA_Streamx��DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//					 chx:DMAͨ��ѡ��,DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
// �� �� ֵ: ��
// ע������: ����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//
//-----------------------------------------------------------------
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)// �õ���ǰstream������DMA2����DMA1
	{
    __HAL_RCC_DMA2_CLK_ENABLE();// DMA2ʱ��ʹ��	
	}
	else 
	{
    __HAL_RCC_DMA1_CLK_ENABLE();// DMA1ʱ��ʹ�� 
	}
    
	__HAL_LINKDMA(&ADC_Handler,DMA_Handle,ADCDMA_Handler);    // ��DMA��ADC1��ϵ����(����DMA)
	
	// Tx DMA����
	ADCDMA_Handler.Instance=DMA_Streamx;                            // ������ѡ��
	ADCDMA_Handler.Init.Channel=chx;                                // ͨ��ѡ��
	ADCDMA_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;             // ���赽�洢��
	ADCDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 // ���������ģʽ
	ADCDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     // �洢������ģʽ
	ADCDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;// �������ݳ���:16λ
	ADCDMA_Handler.Init.MemDataAlignment=DMA_PDATAALIGN_HALFWORD;   // �洢�����ݳ���:16λ
	ADCDMA_Handler.Init.Mode=DMA_CIRCULAR;                          // ������ͨģʽ
	ADCDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               // �е����ȼ�
	ADCDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
	ADCDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL;      
	ADCDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 // �洢��ͻ�����δ���
	ADCDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              // ����ͻ�����δ���
	
	HAL_DMA_DeInit(&ADCDMA_Handler);   
	HAL_DMA_Init(&ADCDMA_Handler);
	
} 

//-----------------------------------------------------------------
// void MYDMA_ADC_Transmit(ADC_HandleTypeDef *hadc, uint32_t *pData, uint16_t Size)
//-----------------------------------------------------------------
// 
// ��������: ����һ��DMA
// ��ڲ���: UART_HandleTypeDef *hadc�� ADC���
//					 uint8_t *pData�����������ָ��
//	         uint16_t Size�������������
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void MYDMA_ADC_Transmit(ADC_HandleTypeDef *hadc, uint32_t *pData, uint16_t Size)
{
	HAL_DMA_Start(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR, (u32)pData, Size);// ����DMA����
  HAL_ADC_Start_DMA(hadc,(uint32_t *)pData, Size);  // �˺����Ŀ���ʱ�����Ӱ�����յĽ����������У������ȿ����������
}
//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
 
