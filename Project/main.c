//-----------------------------------------------------------------
// ��������:
// 		 ADCģ��ת��ʵ��
// ��    ��: ���ǵ���
// ��ʼ����: 2018-08-04
// �������: 2018-08-04
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0: (2018-08-04)ADCģ��ת��ʵ��-ADC1-PA4
// ���Թ���: ����STM32F429+Cyclone IV����ϵͳ��ƿ����塢LZE_ST_LINK2
// ˵    ��: ADC1���Խӿ�PA4
//    
//-----------------------------------------------------------------
#define AD_R	50			// ADC��׼��ѹ
#define MY_R	50			// ADC��׼��ѹ

#define				FRE_IN					5000000					// �����ź�����Ƶ��ֵ
// У��ʱ�Ƚ�FRE_TEST�����������ź�Ƶ��һ�� 
#define				FRE_TEST				5000029					// ʵ�ʲ���ֵ  (ȡ��)
#define				STANDERD_TIM_CLK   ((float)FRE_IN / FRE_TEST * 150000000)
//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "stm32f429_winner.h"    
#include "adc.h"								
#include "delay.h"						
#include "lcd.h"	
#include "daw.h"
#include "led.h"
#include "ltdc.h"
#include "sdram.h"
#include "timer.h"
#include "usart.h"
#include "key.h"
#include "math.h"
#include "dma.h"
#include "stm32f4xx_hal.h"
#include "spi_communication.h"

//-----------------------------------------------------------------
//-----------------------------------------------------------------
// ������
//-----------------------------------------------------------------

u8 key;
int voice_flag;
u8 flag=0;
u8 buf[40];									 			// char�ݴ���
u32 Xbegin=1000;
u32 Ybegin=220;
u32 Auf0=0;
u8  cnt_r=0;
float Auf=0;
int flag_mymy;

int main(void)
{
	HAL_Init();                     // ��ʼ��HAL��   
	
	SystemClock_Config(360,25,2,8); // ����ʱ��,180Mhz
	LED_Init();                     // ��ʼ��LED 
	SDRAM_Init();                   // ��ʼ��SDRAM
	LCD_Init();                     // ��ʼ��LCD
	//KEY_Init();                     // ��ʼ������
	MY_ADC_Init();                  // ��ʼ��ADC1ͨ��4
	MY_ADC2_Init();
	MYDMA_Config(DMA2_Stream0,DMA_CHANNEL_0);// ��ʼ��DMA 
	uart_init(115200);              // ��ʼ��USART
	SPI_Communication_Init();					// SPIͨѶ��ʼ��
	//TIM3_Init(5000-1,9000-1); 
	MYDMA_ADC_Transmit(&ADC_Handler,(uint32_t *)ADC_DMA_ConvertedValue,1); // ��������
	POINT_COLOR=RED;
	LCD_ShowString(30,30,260,32,32,"Device test begin");	
	LCD_ShowString(30,70,260,32,32,"Talk with me");	
		POINT_COLOR=BLACK;
	LCD_ShowString(30,130,260,16,16,"First: basic test for Vpp,Fre");
		LCD_ShowString(30,160,260,16,16,"Second: detection of output resistance");
		LCD_ShowString(30,190,260,16,16,"Third :Pattern recognition of waveforms");
		POINT_COLOR=GREEN;
	LCD_DrawLine(35, 110, 285, 110);
	LCD_DrawLine(35, 220, 285, 220);
	float myoffset_temp=2041.0;
	int i_tmp=0;
	while(1)
	{
		//LED_B=0;
		u16 adc_in1;
		u16 adc_in2;
		adc_in1=Get_Adc_Average2(ADC_CHANNEL_0,50);		//��0ͨ��ȡ��adc1������				
		float adc_temp=(float)adc_in1*(ADC_Vref/4095);  //adc_in1=Get_Adc_Average1(ADC_CHANNEL_4,50);
		//printf("%f",adc_temp);
		delay_ms(50);
		adc_in2=Get_Adc_Average1(ADC_CHANNEL_4,50);
		float adc_temp2=adc_in2*(ADC_Vref/4095);
		delay_ms(50);
		key=KEY_Scan(1);    // ����ɨ��
		if (key ==3){
			myoffset_temp++;
		}
		if (key ==4){
			myoffset_temp--;
		}
		if (adc_temp<1.5 &&  adc_temp2<1.5){
			voice_flag=1;
		}
		else if (adc_temp<1.5 &&  adc_temp2>1.5){
			voice_flag=2;
		}
		else if (adc_temp>1.5 &&  adc_temp2<1.5){
			voice_flag=3;
		}
		else if(adc_temp>1.5 &&  adc_temp2>1.5){
			voice_flag=4;
		}
		else {
			voice_flag=5;
		}
	
	if(voice_flag==1 && flag !=1)
	{
	LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//�ð�ɫ����
	 POINT_COLOR=RED;
	LCD_ShowString(30,30,260,32,32,"Device test begin");	
	LCD_ShowString(30,70,260,32,32,"Talk with me");	
		POINT_COLOR=BLACK;
	LCD_ShowString(30,130,260,16,16,"First: basic test for Vpp,Fre");
		LCD_ShowString(30,160,260,16,16,"Second: detection of output resistance");
		LCD_ShowString(30,190,260,16,16,"Third :Pattern recognition of waveforms");
		POINT_COLOR=GREEN;
	LCD_DrawLine(35, 110, 285, 110);
	LCD_DrawLine(35, 220, 285, 220);
		
	 flag=1;
	}
		
		
	else if(voice_flag==2 && flag !=2 )
	{
	 LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//�ð�ɫ����
	 POINT_COLOR=RED;
	 LCD_ShowString(30,30,260,32,32,"Part of Basic");
	 LCD_ShowString(30,70,260,32,32,"Siganal Decting!");
		POINT_COLOR=BLUE;
	 LCD_ShowString(30,130,260,32,32,"Vpp:");
	 LCD_ShowString(30,170,260,32,32,"Fre:");
		POINT_COLOR=GREEN;
	LCD_DrawLine(35, 120, 285, 120);
	LCD_DrawLine(35, 210, 285, 210);
	// LCD_ShowString(30,180,260,16,16,"Au:");
	 flag=2;
	}
	
	else if(voice_flag==3 && flag !=3)
	{
	 LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//�ð�ɫ����
	 POINT_COLOR=RED;
	LCD_ShowString(30,30,260,32,32,"Ouput R Decting");
	 LCD_ShowString(30,70,260,32,32,"Output 1k Sinewave");
		POINT_COLOR=BLUE;
		LCD_ShowString(30,160,260,32,32,"R:");
		POINT_COLOR=GREEN;
	LCD_DrawLine(35, 140, 285, 140);
	LCD_DrawLine(35, 210, 285, 210);
	// LCD_ShowString(30,150,260,16,16,"FRE:");
	 //LCD_DrawRectangle(10, 10, 310,230);
	// LCD_DrawLine(35, 210, 285, 210);//��35��280��һ����250��x�ᣬ������ʾƵ�ʣ�
	// LCD_DrawLine(35, 210, 35 , 20);//��35��210��һ����Ϊ200��y�ᣬ������ʾ���棻
	 flag=3;
	}

	else if(voice_flag==4 && flag !=4)
	{
  LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//�ð�ɫ����
	POINT_COLOR=RED;
	LCD_ShowString(30,30,260,32,32,"Part of Increase");
	LCD_ShowString(30,70,260,32,32,"Wave Detacting");
	POINT_COLOR=BLUE;
	LCD_ShowString(30,130,260,32,32,"Wave:");
	LCD_ShowString(30,170,260,32,32,"Offset:");
	POINT_COLOR=GREEN;
	LCD_DrawLine(35, 110, 285, 110);
	LCD_DrawLine(35, 210, 285, 210);
	flag=4;
	}
	
	else{
	flag=flag;
	}
	while (flag==1){
	delay_ms(500);
	break;
	}

	while(flag==2)	//��1�����֣������������ݣ�������1���º�flag����1״̬�������ģʽ��
{
	
	POINT_COLOR=BLACK;
	SPI_Communication_Send_Cmd_Data(0,0)	;
	delay_ms(10);	
	SPI_Communication_Send_Cmd_Data(0,1)	;	
	delay_ms(10);
	u32 data_3 = SPI_Communication_Rece_Cmd_Data(3);
	u32 data_4 = SPI_Communication_Rece_Cmd_Data(4);	 
	delay_ms(50);	
	u32 data_1 = SPI_Communication_Rece_Cmd_Data(1);		// ��0ͨ��FPGA���ձ�׼����ֵ
//	u32 data_2 = SPI_Communication_Rece_Cmd_Data(2);		// ��0ͨ��FPGA���ձ�׼����ֵ
	//printf("%d ",data_2);	
	delay_ms(10);	
	
	float myff_tmp[20];
	float myvp_1=(float)(2.0*(data_1/4095.0*10.0)-0.02);
	float my_ff5=(float)(STANDERD_TIM_CLK*( (float)data_4/ (float)data_3));	// Ƶ�ʼ���
	sprintf ( (char *)buf, "(kHz)%.2f ", my_ff5 );		// ��ʾ��׼����ֵ
	LCD_ShowString(100,170,260,32,32, (char *)buf );
	
	
//	float myoffset_1=(float)(-1)*2.0*(data_2-2037.0)/4095.0*10.0;
	printf("%d %d \n",data_3,data_4);
	delay_ms(100);
	sprintf ( (char *)buf, "(V)%.4f",myvp_1);		// ��ʾ��׼����ֵ
  LCD_ShowString(100,130,260,32,32, (char *)buf );
	//sprintf ( (char *)buf, "%.2f   V  %d", myoffset_1,  data_2);		// ��ʾ��׼����ֵ
	//LCD_ShowString(100,150,260,16,16, (char *)buf );
	break;
	}
	
	while(flag==3)	//��3�����֣������������ݣ�������1���º�flag����1״̬�������ģʽ��
{
	POINT_COLOR=BLACK;
	LED_B=0;
	SPI_Communication_Send_Cmd_Data(0,0)	;
	delay_ms(50);
	SPI_Communication_Send_Cmd_Data(0,1)	;		
	delay_ms(250);	
	u32 data_5 = SPI_Communication_Rece_Cmd_Data(1);
	float myvp_2=(float)data_5/4095.0*10.0;
	printf("1: %f \n",myvp_2);
	LED_B=1;
	SPI_Communication_Send_Cmd_Data(0,0)	;
	delay_ms(50);
	SPI_Communication_Send_Cmd_Data(0,1)	;		
	delay_ms(250);	
	u32 data_6 = SPI_Communication_Rece_Cmd_Data(1);
	float myvp_3=(float)data_6/4095.0*10.0;
	printf("2: %f \n",myvp_3);
	float R_temp=(100.0*myvp_3-50.0*myvp_2)/(myvp_2-myvp_3)*1.0;
	sprintf ( (char *)buf, "%.2f ", R_temp);		// ��ʾ��׼����ֵ
  LCD_ShowString(100,160,260,32,32, (char *)buf );
	LED_B=0;
	break;
	}

	while(flag==4){
	POINT_COLOR=BLACK;
	SPI_Communication_Send_Cmd_Data(0,0)	;
	delay_ms(50);
	SPI_Communication_Send_Cmd_Data(0,1)	;
	delay_ms(50);
	u32 data_9 = SPI_Communication_Rece_Cmd_Data(1);
	u32 data_10 = SPI_Communication_Rece_Cmd_Data(2);
	u32 data_6 = SPI_Communication_Rece_Cmd_Data(5);
	u32 data_7 = SPI_Communication_Rece_Cmd_Data(6);
	u32 data_8 = SPI_Communication_Rece_Cmd_Data(7);
	
	double aaa;
	aaa=data_6*data_6+data_7*data_7;
	aaa=sqrt(aaa);
	printf("xx\n");
	int b,c,d;
	b=(signed)data_8;
	c=(signed)(10-b);
	double m=c;
	int xxx=1;
	for (int x=0;x<m;x++){
	xxx=xxx*2;
	}
	aaa=aaa/xxx;
	//aaa=aaa/(2^(10-data_8));
	delay_ms(50);
	float myvp_5=(float)(data_9/4095.0*10.0);
	aaa=(double)aaa/myvp_5;
	if (aaa<90){
	LCD_ShowString(130,130,260,32,32, "Ramp" );
	}
	else if (95<aaa && aaa<115){
	LCD_ShowString(130,130,260,32,32, "Sine" );
	}
	else if(aaa>115 && aaa<150){
	LCD_ShowString(130,130,260,32,32, "Square" );
	}
	else{
	}
	printf("aaa: %.3f offset: %d ",aaa,data_10);
	//sprintf ( (char *)buf, "%.3f ", aaa);		// 
	//LCD_ShowString(100,150,260,16,16, (char *)buf );
	float myoffset_2=(float)(-1)*2.0*(data_10-myoffset_temp)/4095.0*10.0;
	sprintf ( (char *)buf, "(v)%.4f     ", myoffset_2);		// ��ʾ��׼����ֵ
	LCD_ShowString(150,170,260,32,32, (char *)buf );
	break;
	}
}

}
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------

