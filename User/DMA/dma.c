//-----------------------------------------------------------------
// 程序描述:
//     DMA驱动程序
// 作    者: 凌智电子
// 开始日期: 2018-08-04
// 完成日期: 2018-08-04
// 修改日期: 
// 当前版本: V1.0
// 历史版本:
//  - V1.0: (2018-08-04)DMA配置和传输
// 调试工具: 凌智STM32F429+CycloneIV电子系统设计开发板、LZE_ST_LINK2
// 说    明: 
//    
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include "dma.h"
#include "lcd.h"
#include "adc.h"
//-----------------------------------------------------------------

DMA_HandleTypeDef  ADCDMA_Handler;      // DMA句柄
extern ADC_HandleTypeDef ADC_Handler;
//-----------------------------------------------------------------
// void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
//-----------------------------------------------------------------
// 
// 函数功能: DMAx的各通道配置
// 入口参数: DMA_Streamx：DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//					 chx:DMA通道选择,DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
// 返 回 值: 无
// 注意事项: 这里的传输形式是固定的,这点要根据不同的情况来修改
//
//-----------------------------------------------------------------
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)// 得到当前stream是属于DMA2还是DMA1
	{
    __HAL_RCC_DMA2_CLK_ENABLE();// DMA2时钟使能	
	}
	else 
	{
    __HAL_RCC_DMA1_CLK_ENABLE();// DMA1时钟使能 
	}
    
	__HAL_LINKDMA(&ADC_Handler,DMA_Handle,ADCDMA_Handler);    // 将DMA与ADC1联系起来(发送DMA)
	
	// Tx DMA配置
	ADCDMA_Handler.Instance=DMA_Streamx;                            // 数据流选择
	ADCDMA_Handler.Init.Channel=chx;                                // 通道选择
	ADCDMA_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;             // 外设到存储器
	ADCDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 // 外设非增量模式
	ADCDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     // 存储器增量模式
	ADCDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;// 外设数据长度:16位
	ADCDMA_Handler.Init.MemDataAlignment=DMA_PDATAALIGN_HALFWORD;   // 存储器数据长度:16位
	ADCDMA_Handler.Init.Mode=DMA_CIRCULAR;                          // 外设普通模式
	ADCDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               // 中等优先级
	ADCDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
	ADCDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL;      
	ADCDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 // 存储器突发单次传输
	ADCDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              // 外设突发单次传输
	
	HAL_DMA_DeInit(&ADCDMA_Handler);   
	HAL_DMA_Init(&ADCDMA_Handler);
	
} 

//-----------------------------------------------------------------
// void MYDMA_ADC_Transmit(ADC_HandleTypeDef *hadc, uint32_t *pData, uint16_t Size)
//-----------------------------------------------------------------
// 
// 函数功能: 开启一次DMA
// 入口参数: UART_HandleTypeDef *hadc： ADC句柄
//					 uint8_t *pData：传输的数据指针
//	         uint16_t Size：传输的数据量
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void MYDMA_ADC_Transmit(ADC_HandleTypeDef *hadc, uint32_t *pData, uint16_t Size)
{
	HAL_DMA_Start(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR, (u32)pData, Size);// 开启DMA传输
  HAL_ADC_Start_DMA(hadc,(uint32_t *)pData, Size);  // 此函数的开启时间可能影响最终的结果，如果不行，就率先开启这个函数
}
//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
 
