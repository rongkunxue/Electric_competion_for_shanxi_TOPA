//-----------------------------------------------------------------
// ��������:
// 		 ADC��������ͷ�ļ�
// ��    ��: ���ǵ���
// ��ʼ����: 2018-08-04
// �������: 2018-08-04
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0: (2018-08-04)ADC��ʼ��
// ���Թ���: ����STM32F429+Cyclone IV����ϵͳ��ƿ����塢LZE_ST_LINK2
// ˵    ��: 
//    
//-----------------------------------------------------------------
#ifndef __ADC_H
#define __ADC_H
#include "stm32f429_winner.h"
//-----------------------------------------------------------------
extern ADC_HandleTypeDef ADC_Handler;// ADC���
extern u16 ADC_DMA_ConvertedValue[1];
//-----------------------------------------------------------------
// �궨��
//-----------------------------------------------------------------
#define ADC_Vref	3.0			// ADC��׼��ѹ

//-----------------------------------------------------------------
// ADC���Ŷ���
//-----------------------------------------------------------------
#define ADC_Instance													 ADC1
#define ADC_Instance1													 ADC2
#define ADC_PIN                                GPIO_PIN_4
#define ADC_PIN0  														 GPIO_PIN_0

#define ADC_GPIO_PORT                          GPIOA
#define ADC_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()  
#define ADC_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOA_CLK_DISABLE() 
#define ADC_CLK_ENABLE()                  		 __HAL_RCC_ADC1_CLK_ENABLE()  
#define ADC2_CLK_ENABLE()                      __HAL_RCC_ADC2_CLK_ENABLE()

//-----------------------------------------------------------------
// ��������
//-----------------------------------------------------------------
extern void MY_ADC_Init(void); 										// ADCͨ����ʼ��
extern void MY_ADC2_Init(void);
extern u16 Get_Adc_Average2(u32 ch,u8 times);
extern u16 Get_Adc1(u32 ch) ;
extern void Get_Adc_Average(u16 *temp,u8 times);	// �õ�ĳ��ͨ����������������ƽ��ֵ
extern u16 Get_Adc(u32 ch) ;
extern u16 Get_Adc_Average1(u32 ch,u8 times);	
#endif 
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
