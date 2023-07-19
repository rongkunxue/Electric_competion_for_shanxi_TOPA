//-----------------------------------------------------------------
// 程序描述:
// 		 ADC模数转换实验
// 作    者: 凌智电子
// 开始日期: 2018-08-04
// 完成日期: 2018-08-04
// 修改日期: 
// 当前版本: V1.0
// 历史版本:
//  - V1.0: (2018-08-04)ADC模数转换实验-ADC1-PA4
// 调试工具: 凌智STM32F429+Cyclone IV电子系统设计开发板、LZE_ST_LINK2
// 说    明: ADC1测试接口PA4
//    
//-----------------------------------------------------------------
#define AD_R	50			// ADC基准电压
#define MY_R	50			// ADC基准电压

#define				FRE_IN					5000000					// 待测信号输入频率值
// 校正时先将FRE_TEST设置与输入信号频率一致 
#define				FRE_TEST				5000029					// 实际测量值  (取整)
#define				STANDERD_TIM_CLK   ((float)FRE_IN / FRE_TEST * 150000000)
//-----------------------------------------------------------------
// 头文件包含
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
// 主程序
//-----------------------------------------------------------------

u8 key;
int voice_flag;
u8 flag=0;
u8 buf[40];									 			// char暂存区
u32 Xbegin=1000;
u32 Ybegin=220;
u32 Auf0=0;
u8  cnt_r=0;
float Auf=0;
int flag_mymy;

int main(void)
{
	HAL_Init();                     // 初始化HAL库   
	
	SystemClock_Config(360,25,2,8); // 设置时钟,180Mhz
	LED_Init();                     // 初始化LED 
	SDRAM_Init();                   // 初始化SDRAM
	LCD_Init();                     // 初始化LCD
	//KEY_Init();                     // 初始化按键
	MY_ADC_Init();                  // 初始化ADC1通道4
	MY_ADC2_Init();
	MYDMA_Config(DMA2_Stream0,DMA_CHANNEL_0);// 初始化DMA 
	uart_init(115200);              // 初始化USART
	SPI_Communication_Init();					// SPI通讯初始化
	//TIM3_Init(5000-1,9000-1); 
	MYDMA_ADC_Transmit(&ADC_Handler,(uint32_t *)ADC_DMA_ConvertedValue,1); // 启动传输
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
		adc_in1=Get_Adc_Average2(ADC_CHANNEL_0,50);		//从0通道取得adc1的数据				
		float adc_temp=(float)adc_in1*(ADC_Vref/4095);  //adc_in1=Get_Adc_Average1(ADC_CHANNEL_4,50);
		//printf("%f",adc_temp);
		delay_ms(50);
		adc_in2=Get_Adc_Average1(ADC_CHANNEL_4,50);
		float adc_temp2=adc_in2*(ADC_Vref/4095);
		delay_ms(50);
		key=KEY_Scan(1);    // 按键扫描
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
	LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//用白色清屏
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
	 LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//用白色清屏
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
	 LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//用白色清屏
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
	// LCD_DrawLine(35, 210, 285, 210);//从35到280画一条长250的x轴，用来表示频率；
	// LCD_DrawLine(35, 210, 35 , 20);//从35到210画一条长为200的y轴，用来表示增益；
	 flag=3;
	}

	else if(voice_flag==4 && flag !=4)
	{
  LCD_Fill( 0, 0, lcddev.width, lcddev.height ,WHITE);//用白色清屏
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

	while(flag==2)	//第1个部分，基础测量部份，当按键1按下后，flag进入1状态，进入该模式中
{
	
	POINT_COLOR=BLACK;
	SPI_Communication_Send_Cmd_Data(0,0)	;
	delay_ms(10);	
	SPI_Communication_Send_Cmd_Data(0,1)	;	
	delay_ms(10);
	u32 data_3 = SPI_Communication_Rece_Cmd_Data(3);
	u32 data_4 = SPI_Communication_Rece_Cmd_Data(4);	 
	delay_ms(50);	
	u32 data_1 = SPI_Communication_Rece_Cmd_Data(1);		// 从0通道FPGA接收标准计数值
//	u32 data_2 = SPI_Communication_Rece_Cmd_Data(2);		// 从0通道FPGA接收标准计数值
	//printf("%d ",data_2);	
	delay_ms(10);	
	
	float myff_tmp[20];
	float myvp_1=(float)(2.0*(data_1/4095.0*10.0)-0.02);
	float my_ff5=(float)(STANDERD_TIM_CLK*( (float)data_4/ (float)data_3));	// 频率计算
	sprintf ( (char *)buf, "(kHz)%.2f ", my_ff5 );		// 显示标准计数值
	LCD_ShowString(100,170,260,32,32, (char *)buf );
	
	
//	float myoffset_1=(float)(-1)*2.0*(data_2-2037.0)/4095.0*10.0;
	printf("%d %d \n",data_3,data_4);
	delay_ms(100);
	sprintf ( (char *)buf, "(V)%.4f",myvp_1);		// 显示标准计数值
  LCD_ShowString(100,130,260,32,32, (char *)buf );
	//sprintf ( (char *)buf, "%.2f   V  %d", myoffset_1,  data_2);		// 显示标准计数值
	//LCD_ShowString(100,150,260,16,16, (char *)buf );
	break;
	}
	
	while(flag==3)	//第3个部分，基础测量部份，当按键1按下后，flag进入1状态，进入该模式中
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
	sprintf ( (char *)buf, "%.2f ", R_temp);		// 显示标准计数值
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
	sprintf ( (char *)buf, "(v)%.4f     ", myoffset_2);		// 显示标准计数值
	LCD_ShowString(150,170,260,32,32, (char *)buf );
	break;
	}
}

}
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------

