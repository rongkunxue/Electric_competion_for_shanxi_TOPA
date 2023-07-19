//-----------------------------------------------------------------
// 程序描述:
// 		 ADC驱动程序
// 作    者: 凌智电子
// 开始日期: 2018-08-04
// 完成日期: 2018-08-04
// 修改日期: 
// 当前版本: V1.0
// 历史版本:
//  - V1.0: (2018-08-04)ADC初始化
// 调试工具: 凌智STM32F429+Cyclone IV电子系统设计开发板、LZE_ST_LINK2
// 说    明: 
//    
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include "adc.h"
#include "delay.h"
//-----------------------------------------------------------------

ADC_HandleTypeDef ADC_Handler;// ADC句柄
ADC_HandleTypeDef ADC_Handler1;
u16 ADC_DMA_ConvertedValue[1];
//-----------------------------------------------------------------
// void MY_ADC_Init(void)
//-----------------------------------------------------------------
//
// 函数功能: 初始化ADC
// 入口参数: 无
// 返回参数: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void MY_ADC_Init(void)
{ 
	ADC_ChannelConfTypeDef ADC_ChanConf;
	
	ADC_Handler.Instance=ADC_Instance;
	ADC_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   // 4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
	ADC_Handler.Init.Resolution=ADC_RESOLUTION_12B;             // 12位模式
	ADC_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             // 右对齐
	ADC_Handler.Init.ScanConvMode=ENABLE;                      	// 非扫描模式
	ADC_Handler.Init.EOCSelection=DISABLE;                      // 关闭EOC中断
	ADC_Handler.Init.ContinuousConvMode=DISABLE;                // 关闭连续转换
	ADC_Handler.Init.NbrOfConversion=1;                         // 1个转换在规则序列中 也就是只转换规则序列1 
	ADC_Handler.Init.DiscontinuousConvMode=DISABLE;             // 禁止不连续采样模式
	ADC_Handler.Init.NbrOfDiscConversion=0;                     // 不连续采样通道数为0
	ADC_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       // 软件触发
	ADC_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;// 使用软件触发
	ADC_Handler.Init.DMAContinuousRequests=ENABLE;             	// 开启DMA请求
	HAL_ADC_Init(&ADC_Handler);                                 // 初始化 
	
	ADC_ChanConf.Channel=ADC_CHANNEL_4;                        	// 通道
  ADC_ChanConf.Rank=1;                                       	// 第1个序列，序列1
  ADC_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;        	// 采样时间
  ADC_ChanConf.Offset=0;                 
  HAL_ADC_ConfigChannel(&ADC_Handler,&ADC_ChanConf);        	// 通道配置
	
}

void MY_ADC2_Init(void)
	{ 
	ADC_Handler1.Instance=ADC_Instance1;
	ADC_Handler1.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   // 4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
	ADC_Handler1.Init.Resolution=ADC_RESOLUTION_12B;             // 12位模式
	ADC_Handler1.Init.DataAlign=ADC_DATAALIGN_RIGHT;             // 右对齐
	ADC_Handler1.Init.ScanConvMode=DISABLE;                      // 非扫描模式
	ADC_Handler1.Init.EOCSelection=DISABLE;                      // 关闭EOC中断
	ADC_Handler1.Init.ContinuousConvMode=DISABLE;                // 关闭连续转换
	ADC_Handler1.Init.NbrOfConversion=1;                         // 1个转换在规则序列中 也就是只转换规则序列1 
	ADC_Handler1.Init.DiscontinuousConvMode=DISABLE;             // 禁止不连续采样模式
	ADC_Handler1.Init.NbrOfDiscConversion=0;                     // 不连续采样通道数为0
	ADC_Handler1.Init.ExternalTrigConv=ADC_SOFTWARE_START;       // 软件触发
	ADC_Handler1.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;// 使用软件触发
	ADC_Handler1.Init.DMAContinuousRequests=DISABLE;             // 关闭DMA请求
	HAL_ADC2_Init(&ADC_Handler1);                                 // 初始化 2
}
//-----------------------------------------------------------------
// void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
//-----------------------------------------------------------------
//
// 函数功能: ADC底层驱动，引脚配置，时钟使能
// 入口参数: ADC_HandleTypeDef* hadc：ADC句柄
// 返回参数: 无
// 注意事项: 此函数会被HAL_ADC_Init()调用
//
//-----------------------------------------------------------------
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_Initure;
	ADC_CLK_ENABLE();         					 // 使能ADC时钟
	ADC_GPIO_CLK_ENABLE();				 			 // 开启ADC的GPIO时钟

	GPIO_Initure.Pin=ADC_PIN;         	 // PA4
	GPIO_Initure.Mode=GPIO_MODE_ANALOG;  // 模拟
	GPIO_Initure.Pull=GPIO_NOPULL;       // 不带上下拉
	HAL_GPIO_Init(ADC_GPIO_PORT,&GPIO_Initure);
}

void HAL_ADC_MspInit0(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_Initure;
	ADC2_CLK_ENABLE();         					 // 使能ADC2时钟
	ADC_GPIO_CLK_ENABLE();				 			 // 开启ADC2的GPIOA时钟

	GPIO_Initure.Pin=ADC_PIN0;         	 // PA0
	GPIO_Initure.Mode=GPIO_MODE_ANALOG;  // 模拟
	GPIO_Initure.Pull=GPIO_NOPULL;       // 不带上下拉
	HAL_GPIO_Init(ADC_GPIO_PORT,&GPIO_Initure);
}
//-----------------------------------------------------------------
// u16 Get_Adc_Average(u32 ch,u8 times)
//-----------------------------------------------------------------
//
// 函数功能: 获取指定通道的转换值，取times次,然后平均 
// 入口参数: u16 *temp：times次转换结果平均值
//					 u8 times：获取次数
// 返回参数: 无
// 注意事项: 此函数会被HAL_ADC_Init()调用
//
//-----------------------------------------------------------------
u16 Get_Adc(u32 ch)   
{
	ADC_ChannelConfTypeDef ADC1_ChanConf;
	
	ADC1_ChanConf.Channel=ch;                            // 通道
	ADC1_ChanConf.Rank=1;                                // 第1个序列，序列1
	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES; // 采样时间
	ADC1_ChanConf.Offset=0;                 
	HAL_ADC_ConfigChannel(&ADC_Handler,&ADC1_ChanConf);  // 通道配置

	HAL_ADC_Start(&ADC_Handler);                         // 开启ADC

	HAL_ADC_PollForConversion(&ADC_Handler,10);          // 轮询转换
 
	return (u16)HAL_ADC_GetValue(&ADC_Handler);	         // 返回最近一次ADC1规则组的转换结果
}


u16 Get_Adc1(u32 ch)   
{
	ADC_ChannelConfTypeDef ADC1_ChanConf;
	
	ADC1_ChanConf.Channel=ch;                            // 通道
	ADC1_ChanConf.Rank=1;                                // 第1个序列，序列1
	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES; // 采样时间
	ADC1_ChanConf.Offset=0;                 
	HAL_ADC_ConfigChannel(&ADC_Handler1,&ADC1_ChanConf);  // 通道配置

	HAL_ADC_Start(&ADC_Handler1);                         // 开启ADC

	HAL_ADC_PollForConversion(&ADC_Handler1,10);          // 轮询转换
 
	return (u16)HAL_ADC_GetValue(&ADC_Handler1);	         // 返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average2(u32 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc1(ch);
		//delay_ms(5);
	}
	return temp_val/times;
} 

u16 Get_Adc_Average1(u32 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		//delay_ms(5);
	}
	return temp_val/times;
} 

void Get_Adc_Average(u16 *temp,u8 times)
{
	u32 temp_val[1]={0};
	u8 t;
	for(t=0;t<times;t++)
	{
		HAL_ADC_Start(&ADC_Handler); // 开启ADC
		delay_ms(10);
		temp_val[0]+=ADC_DMA_ConvertedValue[0];
	}
	temp[0]=temp_val[0]/times;
	temp_val[0]=0;
} 


//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
