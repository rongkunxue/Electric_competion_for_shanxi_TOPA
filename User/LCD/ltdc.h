//-----------------------------------------------------------------
// ��������:
//     4.3��/7��RGBҺ����ʾ����������ͷ�ļ�
// ��    ��: ���ǵ���
// ��ʼ����: 2018-08-04
// �������: 2018-08-04
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//	- V1.0��4.3��/7�� RGBҺ����ʾ  
// ���Թ���: ����STM32F429+CycloneIV����ϵͳ��ƿ����塢LZE_ST_LINK2
// ˵    ��: 
//    
//-----------------------------------------------------------------
#ifndef _LCD_H
#define _LCD_H
#include "stm32f429_winner.h"
//-----------------------------------------------------------------
// ��������
//-----------------------------------------------------------------
// LCD LTDC��Ҫ������
typedef struct  
{							 
	u32 pwidth;				// LCD���Ŀ��,�̶�����,������ʾ����ı�,���Ϊ0,˵��û���κ�RGB������
	u32 pheight;			// LCD���ĸ߶�,�̶�����,������ʾ����ı�
	u16 hsw;					// ˮƽͬ�����
	u16 vsw;					// ��ֱͬ�����
	u16 hbp;					// ˮƽ����
	u16 vbp;					// ��ֱ����
	u16 hfp;					// ˮƽǰ��
	u16 vfp;					// ��ֱǰ�� 
	u8 activelayer;		// ��ǰ����:0/1	
	u8 dir;						// ��ʾ����
	u16 width;				// LCD���
	u16 height;				// LCD�߶�
	u32 pixsize;			// ÿ��������ռ�ֽ���
}_ltdc_dev; 

extern _ltdc_dev lcdltdc;		            		// ����LCD LTDC����
extern LTDC_HandleTypeDef LTDC_Handler;	    // LTDC���
extern DMA2D_HandleTypeDef DMA2D_Handler;   // DMA2D���

//-----------------------------------------------------------------
// �궨��
//-----------------------------------------------------------------
//LCD����	PC6	
#define LTCD_LED     PCout(6)   

#define LCD_PIXEL_FORMAT_ARGB8888       0X00    
#define LCD_PIXEL_FORMAT_RGB888         0X01    
#define LCD_PIXEL_FORMAT_RGB565         0X02       
#define LCD_PIXEL_FORMAT_ARGB1555       0X03      
#define LCD_PIXEL_FORMAT_ARGB4444       0X04     
#define LCD_PIXEL_FORMAT_L8             0X05     
#define LCD_PIXEL_FORMAT_AL44           0X06     
#define LCD_PIXEL_FORMAT_AL88           0X07      

// �û��޸����ò���:
// ������ɫ���ظ�ʽ,һ����RGB565
#define LCD_PIXFORMAT				LCD_PIXEL_FORMAT_RGB565	
// ����Ĭ�ϱ�������ɫ
#define LTDC_BACKLAYERCOLOR			0X00000000	
// LCD֡�������׵�ַ,���ﶨ����SDRAM����.
#define LCD_FRAME_BUF_ADDR			0XC0000000  

//-----------------------------------------------------------------
// ��������
//-----------------------------------------------------------------
extern void LTDC_Switch(u8 sw);											// LTDC����
extern void LTDC_Layer_Switch(u8 layerx,u8 sw);			// �㿪��
extern void LTDC_Select_Layer(u8 layerx);						// ��ѡ��
extern void LTDC_Display_Dir(u8 dir);								// ��ʾ�������
extern void LTDC_Draw_Point(u16 x,u16 y,u32 color);	// ���㺯��
extern u32 LTDC_Read_Point(u16 x,u16 y);						// ���㺯��
extern void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color);					// ���ε�ɫ��亯��
extern void LTDC_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);	// ���β�ɫ��亯��
extern void LTDC_Clear(u32 color);									// ��������
extern u8 LTDC_Clk_Set(u32 pllsain,u32 pllsair,u32 pllsaidivr);// LTDCʱ������
extern void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height);// LTDC�㴰������
extern void LTDC_Layer_Parameter_Config(u8 layerx,u32 bufaddr,u8 pixformat,u8 alpha,u8 alpha0,u8 bfac1,u8 bfac2,u32 bkcolor);// LTDC������������
extern void LTDC_Init(void);						// LTDC��ʼ������
extern void Lcd_Initialize(void);
#endif 
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------  
