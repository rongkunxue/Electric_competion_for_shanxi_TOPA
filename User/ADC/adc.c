//-----------------------------------------------------------------
// ��������:
// 		 ADC��������
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

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "adc.h"
#include "delay.h"
//-----------------------------------------------------------------

ADC_HandleTypeDef ADC_Handler;// ADC���
ADC_HandleTypeDef ADC_Handler1;
u16 ADC_DMA_ConvertedValue[1];
//-----------------------------------------------------------------
// void MY_ADC_Init(void)
//-----------------------------------------------------------------
//
// ��������: ��ʼ��ADC
// ��ڲ���: ��
// ���ز���: ��
// ע������: ��
//
//-----------------------------------------------------------------
void MY_ADC_Init(void)
{ 
	ADC_ChannelConfTypeDef ADC_ChanConf;
	
	ADC_Handler.Instance=ADC_Instance;
	ADC_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   // 4��Ƶ��ADCCLK=PCLK2/4=90/4=22.5MHZ
	ADC_Handler.Init.Resolution=ADC_RESOLUTION_12B;             // 12λģʽ
	ADC_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             // �Ҷ���
	ADC_Handler.Init.ScanConvMode=ENABLE;                      	// ��ɨ��ģʽ
	ADC_Handler.Init.EOCSelection=DISABLE;                      // �ر�EOC�ж�
	ADC_Handler.Init.ContinuousConvMode=DISABLE;                // �ر�����ת��
	ADC_Handler.Init.NbrOfConversion=1;                         // 1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Handler.Init.DiscontinuousConvMode=DISABLE;             // ��ֹ����������ģʽ
	ADC_Handler.Init.NbrOfDiscConversion=0;                     // ����������ͨ����Ϊ0
	ADC_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       // �������
	ADC_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;// ʹ���������
	ADC_Handler.Init.DMAContinuousRequests=ENABLE;             	// ����DMA����
	HAL_ADC_Init(&ADC_Handler);                                 // ��ʼ�� 
	
	ADC_ChanConf.Channel=ADC_CHANNEL_4;                        	// ͨ��
  ADC_ChanConf.Rank=1;                                       	// ��1�����У�����1
  ADC_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;        	// ����ʱ��
  ADC_ChanConf.Offset=0;                 
  HAL_ADC_ConfigChannel(&ADC_Handler,&ADC_ChanConf);        	// ͨ������
	
}

void MY_ADC2_Init(void)
	{ 
	ADC_Handler1.Instance=ADC_Instance1;
	ADC_Handler1.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   // 4��Ƶ��ADCCLK=PCLK2/4=90/4=22.5MHZ
	ADC_Handler1.Init.Resolution=ADC_RESOLUTION_12B;             // 12λģʽ
	ADC_Handler1.Init.DataAlign=ADC_DATAALIGN_RIGHT;             // �Ҷ���
	ADC_Handler1.Init.ScanConvMode=DISABLE;                      // ��ɨ��ģʽ
	ADC_Handler1.Init.EOCSelection=DISABLE;                      // �ر�EOC�ж�
	ADC_Handler1.Init.ContinuousConvMode=DISABLE;                // �ر�����ת��
	ADC_Handler1.Init.NbrOfConversion=1;                         // 1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Handler1.Init.DiscontinuousConvMode=DISABLE;             // ��ֹ����������ģʽ
	ADC_Handler1.Init.NbrOfDiscConversion=0;                     // ����������ͨ����Ϊ0
	ADC_Handler1.Init.ExternalTrigConv=ADC_SOFTWARE_START;       // �������
	ADC_Handler1.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;// ʹ���������
	ADC_Handler1.Init.DMAContinuousRequests=DISABLE;             // �ر�DMA����
	HAL_ADC2_Init(&ADC_Handler1);                                 // ��ʼ�� 2
}
//-----------------------------------------------------------------
// void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
//-----------------------------------------------------------------
//
// ��������: ADC�ײ��������������ã�ʱ��ʹ��
// ��ڲ���: ADC_HandleTypeDef* hadc��ADC���
// ���ز���: ��
// ע������: �˺����ᱻHAL_ADC_Init()����
//
//-----------------------------------------------------------------
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_Initure;
	ADC_CLK_ENABLE();         					 // ʹ��ADCʱ��
	ADC_GPIO_CLK_ENABLE();				 			 // ����ADC��GPIOʱ��

	GPIO_Initure.Pin=ADC_PIN;         	 // PA4
	GPIO_Initure.Mode=GPIO_MODE_ANALOG;  // ģ��
	GPIO_Initure.Pull=GPIO_NOPULL;       // ����������
	HAL_GPIO_Init(ADC_GPIO_PORT,&GPIO_Initure);
}

void HAL_ADC_MspInit0(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_Initure;
	ADC2_CLK_ENABLE();         					 // ʹ��ADC2ʱ��
	ADC_GPIO_CLK_ENABLE();				 			 // ����ADC2��GPIOAʱ��

	GPIO_Initure.Pin=ADC_PIN0;         	 // PA0
	GPIO_Initure.Mode=GPIO_MODE_ANALOG;  // ģ��
	GPIO_Initure.Pull=GPIO_NOPULL;       // ����������
	HAL_GPIO_Init(ADC_GPIO_PORT,&GPIO_Initure);
}
//-----------------------------------------------------------------
// u16 Get_Adc_Average(u32 ch,u8 times)
//-----------------------------------------------------------------
//
// ��������: ��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
// ��ڲ���: u16 *temp��times��ת�����ƽ��ֵ
//					 u8 times����ȡ����
// ���ز���: ��
// ע������: �˺����ᱻHAL_ADC_Init()����
//
//-----------------------------------------------------------------
u16 Get_Adc(u32 ch)   
{
	ADC_ChannelConfTypeDef ADC1_ChanConf;
	
	ADC1_ChanConf.Channel=ch;                            // ͨ��
	ADC1_ChanConf.Rank=1;                                // ��1�����У�����1
	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES; // ����ʱ��
	ADC1_ChanConf.Offset=0;                 
	HAL_ADC_ConfigChannel(&ADC_Handler,&ADC1_ChanConf);  // ͨ������

	HAL_ADC_Start(&ADC_Handler);                         // ����ADC

	HAL_ADC_PollForConversion(&ADC_Handler,10);          // ��ѯת��
 
	return (u16)HAL_ADC_GetValue(&ADC_Handler);	         // �������һ��ADC1�������ת�����
}


u16 Get_Adc1(u32 ch)   
{
	ADC_ChannelConfTypeDef ADC1_ChanConf;
	
	ADC1_ChanConf.Channel=ch;                            // ͨ��
	ADC1_ChanConf.Rank=1;                                // ��1�����У�����1
	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES; // ����ʱ��
	ADC1_ChanConf.Offset=0;                 
	HAL_ADC_ConfigChannel(&ADC_Handler1,&ADC1_ChanConf);  // ͨ������

	HAL_ADC_Start(&ADC_Handler1);                         // ����ADC

	HAL_ADC_PollForConversion(&ADC_Handler1,10);          // ��ѯת��
 
	return (u16)HAL_ADC_GetValue(&ADC_Handler1);	         // �������һ��ADC1�������ת�����
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
		HAL_ADC_Start(&ADC_Handler); // ����ADC
		delay_ms(10);
		temp_val[0]+=ADC_DMA_ConvertedValue[0];
	}
	temp[0]=temp_val[0]/times;
	temp_val[0]=0;
} 


//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
