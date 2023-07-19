//-----------------------------------------------------------------
// 程序描述:
// 		 关于幅频曲线的绘制
// 作    者: 薛荣坤
// 开始日期: 2021-07-20
// 完成日期: 2021-07-20
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
#include "lcd.h"
#include "daw.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void MY_Rectangle (void)；
//-----------------------------------------------------------------
//
// 函数功能: 画出一条曲线
// 入口参数: 无
// 返回参数: 无
// 注意事项: 无
//d
void MY_Rectangle (void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK;
	LCD_DrawRectangle(20, 20, 280, 210);
}

void drawstring_screen(void )
{	
	LCD_ShowString(30,50,200,16,16,"STM32F429+CycloneIV");	
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"LZ Electronic");
	LCD_ShowString(30,110,200,16,16,"2018/08/04");	  
	LCD_ShowString(30,130,200,16,16,"ADC1_CH4_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH4_VOL:0.000V");	//先 在固定位置显示小数点 
}

float findmax(float arry[],int n){
	float max=arry[0];
	for (int i=1;i<n;i++){
		if (max<arry[i]){
				max=arry[i];
		}
	}
	return max;
}

float  findmin(float arry[1000],int n){
	float min=arry[0];
	for (int i=0;i<n;i++){
		if (min>arry[i]){
		min=arry[i];}
	}
	return min;
	}
	
float findpingjun(float arry[1000],int n){
		float pingjun=0;
		for (int i=0;i<n;i++){
		pingjun=pingjun+arry[i];}
		pingjun=(float)pingjun/n;
		return pingjun;

}

