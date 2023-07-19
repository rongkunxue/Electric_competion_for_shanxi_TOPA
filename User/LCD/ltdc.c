//-----------------------------------------------------------------
// ��������:
//     4.3��/7��RGBҺ����ʾ����������  
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

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "ltdc.h"
#include "lcd.h"
#include "delay.h"	 
//-----------------------------------------------------------------

#define LCD_SPI_CS(a)	\
						if (a)	\
						GPIOA->BSRR = GPIO_PIN_0;	\
						else		\
						GPIOA->BSRR  = (uint32_t)GPIO_PIN_0 << 16;
#define SPI_DCLK(a)	\
						if (a)	\
						GPIOC->BSRR = GPIO_PIN_2;	\
						else		\
						GPIOC->BSRR  = (uint32_t)GPIO_PIN_2 << 16;
#define SPI_SDA(a)	\
						if (a)	\
						GPIOA->BSRR = GPIO_PIN_1;	\
						else		\
						GPIOA->BSRR  = (uint32_t)GPIO_PIN_1 << 16;

LTDC_HandleTypeDef  LTDC_Handler;	    // LTDC���
DMA2D_HandleTypeDef DMA2D_Handler; 	  // DMA2D���

// ���ݲ�ͬ����ɫ��ʽ,����֡��������
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	u32 ltdc_lcd_framebuf[1024][600] __attribute__((at(LCD_FRAME_BUF_ADDR)));	// ����������ֱ���ʱ,LCD�����֡���������С
#else
	u16 ltdc_lcd_framebuf[1024][600] __attribute__((at(LCD_FRAME_BUF_ADDR)));	// ����������ֱ���ʱ,LCD�����֡���������С
#endif

u32 *ltdc_framebuf[2];	// LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����
_ltdc_dev lcdltdc;			// ����LCD LTDC����Ҫ����

//-----------------------------------------------------------------
// void LTDC_Switch(u8 sw)
//-----------------------------------------------------------------
// 
// ��������: ��LCD����
// ��ڲ���: u8 sw��1 ��,0���ر�
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Switch(u8 sw)
{
	if(sw==1) 
		__HAL_LTDC_ENABLE(&LTDC_Handler);
	else if(sw==0)
		__HAL_LTDC_DISABLE(&LTDC_Handler);
}

//-----------------------------------------------------------------
// void LTDC_Layer_Switch(u8 layerx,u8 sw)
//-----------------------------------------------------------------
// 
// ��������: ����ָ����
// ��ڲ���: u8 layerx�����,0,��һ��; 1,�ڶ���
//					 u8 sw��1 ��,0���ر�
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Layer_Switch(u8 layerx,u8 sw)
{
	if(sw==1) 
		__HAL_LTDC_LAYER_ENABLE(&LTDC_Handler,layerx);
	else if(sw==0) 
		__HAL_LTDC_LAYER_DISABLE(&LTDC_Handler,layerx);
	__HAL_LTDC_RELOAD_CONFIG(&LTDC_Handler);
}


//-----------------------------------------------------------------
// void LTDC_Select_Layer(u8 layerx)
//-----------------------------------------------------------------
// 
// ��������: ѡ���
// ��ڲ���: u8 layerx�����,0,��һ��; 1,�ڶ���
//					 u8 sw��1 ��,0���ر�
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Select_Layer(u8 layerx)
{
	lcdltdc.activelayer=layerx;
}

//-----------------------------------------------------------------
// void LTDC_Display_Dir(u8 dir)
//-----------------------------------------------------------------
// 
// ��������: ����LCD��ʾ����
// ��ڲ���: u8 dir��dir:0,������1,����
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Display_Dir(u8 dir)
{
  lcdltdc.dir=dir;// ��ʾ����
	lcddev.dir=dir;	// ��ʾ����
	if(dir==DISPLAY_DIR_D || dir==DISPLAY_DIR_U)		
	{
		lcdltdc.width=lcdltdc.pheight;
		lcdltdc.height=lcdltdc.pwidth;	
		lcddev.width=lcdltdc.width;
		lcddev.height=lcdltdc.height;
	}
	else if(dir==DISPLAY_DIR_L || dir==DISPLAY_DIR_R)
	{
		lcdltdc.width=lcdltdc.pwidth;
		lcdltdc.height=lcdltdc.pheight;
		lcddev.width=lcdltdc.width;
		lcddev.height=lcdltdc.height;
	}
}

//-----------------------------------------------------------------
// void LTDC_Draw_Point(u16 x,u16 y,u32 color)
//-----------------------------------------------------------------
// 
// ��������: ���㺯��
// ��ڲ���: u16 x��X����
//					 u16 y��Y����
//					 u32 color����ɫ
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Draw_Point(u16 x,u16 y,u32 color)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	if(lcdltdc.dir==DISPLAY_DIR_R)			
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-y-1)+x))=color;
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-x-1)+y))=color; 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_L)	
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+(lcdltdc.pwidth-x-1)))=color;
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*x+(lcdltdc.pheight-y-1)))=color; 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_U) 
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+(lcdltdc.pwidth-y-1)))=color; 
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*y+x))=color;
	}
	else																
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*x+y))=color; 
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-y-1)+(lcdltdc.pheight-x-1)))=color;
	}
#else
	if(lcdltdc.dir==DISPLAY_DIR_R)			
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-y-1)+x))=color;
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-x-1)+y))=color; 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_L)	
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+(lcdltdc.pwidth-x-1)))=color;
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*x+(lcdltdc.pheight-y-1)))=color; 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_U) 
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+(lcdltdc.pwidth-y-1)))=color; 
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*y+x))=color;
	}
	else																
	{
		if(lcddev.id == 0x7016)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*x+y))=color; 
		else if(lcddev.id == 0x8000)
			*(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-y-1)+(lcdltdc.pheight-x-1)))=color;
	}
#endif
}

//-----------------------------------------------------------------
// u32 LTDC_Read_Point(u16 x,u16 y)
//-----------------------------------------------------------------
// 
// ��������: ���㺯��
// ��ڲ���: u16 x��X����
//					 u16 y��Y����
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
u32 LTDC_Read_Point(u16 x,u16 y)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	if(lcdltdc.dir==DISPLAY_DIR_R)			
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-y-1)+x));
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-x-1)+y)); 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_L)	
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+(lcdltdc.pwidth-x-1)));
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*x+(lcdltdc.pheight-y-1))); 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_U) 
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+(lcdltdc.pwidth-y-1))); 
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*y+x));
	}
	else																
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*x+y)); 
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-y-1)+(lcdltdc.pheight-x-1)));
	}
	return 0;
#else
	if(lcdltdc.dir==DISPLAY_DIR_R)			
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-y-1)+x));
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-x-1)+y)); 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_L)	
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+(lcdltdc.pwidth-x-1)));
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*x+(lcdltdc.pheight-y-1))); 
	}
	else if(lcdltdc.dir==DISPLAY_DIR_U) 
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+(lcdltdc.pwidth-y-1))); 
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*y+x));
	}
	else																
	{
		if(lcddev.id == 0x7016)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*x+y)); 
		else if(lcddev.id == 0x8000)
			return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*(lcdltdc.pwidth-y-1)+(lcdltdc.pheight-x-1)));
	}
	return 0;
#endif 
}

//-----------------------------------------------------------------
// void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
//-----------------------------------------------------------------
// 
// ��������: LTDC������,DMA2D���
// ��ڲ���: u16 sx����ʼX����
//					 u16 sy����ʼY����
//					 u16 ex����ֹX����
//					 u16 ey����ֹY����
//					 u32 color����ɫ
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{ 
	u32 psx,psy,pex,pey;	// ��LCD���Ϊ��׼������ϵ,����������仯���仯
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	// ����ϵת��
	if(lcdltdc.dir==DISPLAY_DIR_R)	
	{
		if(lcddev.id==0x8000)
		{
			psx=sy;
			psy=lcdltdc.pwidth-ex-1;
			pex=ey;
			pey=lcdltdc.pwidth-sx-1;
		}
		else if(lcddev.id==0x7016)
		{
			psx=sx;
			psy=lcdltdc.pheight-ey-1;
			pex=ex;
			pey=lcdltdc.pheight-sy-1;
		}
	}
	else if(lcdltdc.dir==DISPLAY_DIR_L)
	{
		if(lcddev.id==0x8000)
		{
			psx=lcdltdc.pheight-ey-1;
			psy=sx;
			pex=lcdltdc.pheight-sy-1;
			pey=ex;
		}
		else if(lcddev.id==0x7016)
		{
			psx=lcdltdc.pwidth-ex-1;
			psy=sy;
			pex=lcdltdc.pwidth-sx-1;
			pey=ey;
		}
	}
	else if(lcdltdc.dir==DISPLAY_DIR_U)			
	{
		if(lcddev.id==0x8000)
		{
			psx=sx;psy=sy;
			pex=ex;pey=ey;
		}
		else if(lcddev.id==0x7016)
		{
			psx=lcdltdc.pwidth-ey-1;
			psy=lcdltdc.pheight-ex-1;
			pex=lcdltdc.pwidth-sy-1;
			pey=lcdltdc.pheight-sx-1;
		}
	}
	else
	{
		if(lcddev.id==0x8000)
		{
			psx=lcdltdc.pheight-ex-1;
			psy=lcdltdc.pwidth-ey-1;
			pex=lcdltdc.pheight-sx-1;
			pey=lcdltdc.pwidth-sy-1;
		}
		else if(lcddev.id==0x7016)
		{
			psx=sy;
			psy=sx;
			pex=ey;
			pey=ex;
		}
	}
	if(lcddev.id==0x8000)
	{
		offline=lcdltdc.pheight-(pex-psx+1);
		addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pheight*psy+psx));
	}
	else if(lcddev.id==0x7016)
	{
		offline=lcdltdc.pwidth-(pex-psx+1);
		addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	}
	__HAL_RCC_DMA2D_CLK_ENABLE();	// ʹ��DM2Dʱ��
	DMA2D->CR&=~(DMA2D_CR_START);	// ��ֹͣDMA2D
	DMA2D->CR=DMA2D_R2M;					// �Ĵ������洢��ģʽ
	DMA2D->OPFCCR=LCD_PIXFORMAT;	// ������ɫ��ʽ
	DMA2D->OOR=offline;						// ������ƫ�� 

	DMA2D->OMAR=addr;							// ����洢����ַ
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	// �趨�����Ĵ���
	DMA2D->OCOLR=color;						// �趨�����ɫ�Ĵ��� 
	DMA2D->CR|=DMA2D_CR_START;								// ����DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)		// �ȴ��������
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	// ��ʱ�˳�
	} 
	DMA2D->IFCR|=DMA2D_FLAG_TC;		// ���������ɱ�־ 		
}

//-----------------------------------------------------------------
// void LTDC_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
//-----------------------------------------------------------------
// 
// ��������: ��ָ�����������ָ����ɫ��,DMA2D���	
// ��ڲ���: u16 sx����ʼX����
//					 u16 sy����ʼY����
//					 u16 ex����ֹX����
//					 u16 ey����ֹY����
//					 u32 color����ɫ
// �� �� ֵ: ��
// ע������: sx,ex,���ܴ���lcddev.width-1;sy,ey,���ܴ���lcddev.height-1!!!
//color:Ҫ������ɫ�����׵�ַ
//
//-----------------------------------------------------------------
void LTDC_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u16 height,width;
	u16 i,j;
	
	width=ex-sx+1; 			// �õ����Ŀ��
	height=ey-sy+1;			// �߶�
	for(i=0;i<height;i++)
			for(j=0;j<width;j++)
				LTDC_Draw_Point(sx+j,sy+i,color[i*width+j]);// д������ 
}  


//-----------------------------------------------------------------
// void LTDC_Clear(u32 color)
//-----------------------------------------------------------------
// 
// ��������: LCD����
// ��ڲ���: u32 color����ɫ
// �� �� ֵ: ��
// ȫ�ֱ���: lcdltdc.width��LCD���
//					 lcdltdc.height��LCD�߶�
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Clear(u32 color)
{
	LTDC_Fill(0,0,lcdltdc.width-1,lcdltdc.height-1,color);	
}

//-----------------------------------------------------------------
// u8 LTDC_Clk_Set(u32 pllsain,u32 pllsair,u32 pllsaidivr)
//-----------------------------------------------------------------
// 
// ��������: LTDCʱ��(Fdclk)���ú���
// ��ڲ���: u32 pllsain��SAIʱ�ӱ�Ƶϵ��N,ȡֵ��Χ:50~432. 
//					 u32 pllsair��SAIʱ�ӵķ�Ƶϵ��R,ȡֵ��Χ:2~7
//					 u32 pllsaidivr��LCDʱ�ӷ�Ƶϵ��,ȡֵ��Χ:RCC_PLLSAIDIVR_2/4/8/16,��Ӧ��Ƶ2~16 
// �� �� ֵ: 0,�ɹ�;1,ʧ�ܡ�
// ע������: 
//					 LTDCʱ��(Fdclk)���ú���
//					 Fvco=Fin*pllsain; 
//					 Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;
//					 Fvco:VCOƵ��
//					 Fin:����ʱ��Ƶ��һ��Ϊ1Mhz(����ϵͳʱ��PLLM��Ƶ���ʱ��,��ʱ����ͼ)
//					 ����:�ⲿ����Ϊ25M,pllm=25��ʱ��,Fin=1Mhz.
//					 ����:Ҫ�õ�20M��LTDCʱ��,���������:pllsain=400,pllsair=5,pllsaidivr=RCC_PLLSAIDIVR_4
//					 Fdclk=1*400/5/4=400/20=20Mhz
//
//-----------------------------------------------------------------
u8 LTDC_Clk_Set(u32 pllsain,u32 pllsair,u32 pllsaidivr)
{
	RCC_PeriphCLKInitTypeDef PeriphClkIniture;
	
	// LTDC�������ʱ�ӣ���Ҫ�����Լ���ʹ�õ�LCD�����ֲ������ã�
  PeriphClkIniture.PeriphClockSelection=RCC_PERIPHCLK_LTDC;	// LTDCʱ�� 	
	PeriphClkIniture.PLLSAI.PLLSAIN=pllsain;    
	PeriphClkIniture.PLLSAI.PLLSAIR=pllsair;  
	PeriphClkIniture.PLLSAIDivR=pllsaidivr;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkIniture)==HAL_OK)  // ��������ʱ��
  {
		return 0;   // �ɹ�
  }
  else 
		return 1;   // ʧ��    
}

//-----------------------------------------------------------------
// void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height)
//-----------------------------------------------------------------
// 
// ��������: LTDC���մ�������,������LCD�������ϵΪ��׼
// ��ڲ���: u8 layerx����ֵ,0/1.
//					 u16 sx����ʼX����
//					 u16 sy����ʼY����
//					 u16 width�����ڿ��
//					 u16 height�����ڸ߶�
// �� �� ֵ: 0,�ɹ�;1,ʧ�ܡ�
// ע������: �˺���������LTDC_Layer_Parameter_Config֮��������.
//
//-----------------------------------------------------------------
void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height)
{
	HAL_LTDC_SetWindowPosition(&LTDC_Handler,sx,sy,layerx);  	// ���ô��ڵ�λ��
	HAL_LTDC_SetWindowSize(&LTDC_Handler,width,height,layerx);// ���ô��ڴ�С   
}

//-----------------------------------------------------------------
// void LTDC_Layer_Parameter_Config(u8 layerx,u32 bufaddr,u8 pixformat,u8 alpha,u8 alpha0,u8 bfac1,u8 bfac2,u32 bkcolor)
//-----------------------------------------------------------------
// 
// ��������: LTDC,������������.
// ��ڲ���: u8 layerx����ֵ,0/1.
//					 u32 bufaddr������ɫ֡������ʼ��ַ
//					 u8 pixformat����ɫ��ʽ.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
//					 u8 alpha������ɫAlphaֵ,0,ȫ͸��;255,��͸��
//					 u8 alpha0��Ĭ����ɫAlphaֵ,0,ȫ͸��;255,��͸��
//					 u8 bfac1�����ϵ��1,4(100),�㶨��Alpha;6(101),����Alpha*�㶨Alpha
//					 u8 bfac2�����ϵ��2,5(101),�㶨��Alpha;7(111),����Alpha*�㶨Alpha
//					 u32 bkcolor����Ĭ����ɫ,32λ,��24λ��Ч,RGB888��ʽ
// �� �� ֵ: ��
// ע������: �˺���,������LTDC_Layer_Window_Config֮ǰ����.
//
//-----------------------------------------------------------------
void LTDC_Layer_Parameter_Config(u8 layerx,u32 bufaddr,u8 pixformat,u8 alpha,u8 alpha0,u8 bfac1,u8 bfac2,u32 bkcolor)
{
	LTDC_LayerCfgTypeDef pLayerCfg;
	
	pLayerCfg.WindowX0=0;                       // ������ʼX����
	pLayerCfg.WindowY0=0;                       // ������ʼY����
	pLayerCfg.WindowX1=lcdltdc.pwidth;          // ������ֹX����
	pLayerCfg.WindowY1=lcdltdc.pheight;         // ������ֹY����
	pLayerCfg.PixelFormat=pixformat;		    		// ���ظ�ʽ
	pLayerCfg.Alpha=alpha;				        			// Alphaֵ���ã�0~255,255Ϊ��ȫ��͸��
	pLayerCfg.Alpha0=alpha0;			        			// Ĭ��Alphaֵ
	pLayerCfg.BlendingFactor1=(u32)bfac1<<8;    // ���ò���ϵ��
	pLayerCfg.BlendingFactor2=(u32)bfac2<<8;		// ���ò���ϵ��
	pLayerCfg.FBStartAdress=bufaddr;	        	// ���ò���ɫ֡������ʼ��ַ
	pLayerCfg.ImageWidth=lcdltdc.pwidth;        // ������ɫ֡�������Ŀ��    
	pLayerCfg.ImageHeight=lcdltdc.pheight;      // ������ɫ֡�������ĸ߶�
	pLayerCfg.Backcolor.Red=(u8)(bkcolor&0X00FF0000)>>16;   // ������ɫ��ɫ����
	pLayerCfg.Backcolor.Green=(u8)(bkcolor&0X0000FF00)>>8;  // ������ɫ��ɫ����
	pLayerCfg.Backcolor.Blue=(u8)bkcolor&0X000000FF;        // ������ɫ��ɫ����
	HAL_LTDC_ConfigLayer(&LTDC_Handler,&pLayerCfg,layerx);  // ������ѡ�еĲ�
}  


//-----------------------------------------------------------------
// void LTDC_Init(void)
//-----------------------------------------------------------------
// 
// ��������: LCD��ʼ������
// ��ڲ���: ��
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void LTDC_Init(void)
{   
	u16 lcdid=0;

	lcdid=lcddev.id;						// ��ȡLCD���ID	
	if(lcdid==0X8000)
	{
		Lcd_Initialize();					// 4.3��RGB����
		lcdltdc.pwidth=800;			  // �����,��λ:����
		lcdltdc.pheight=480;		  // ���߶�,��λ:����
		lcdltdc.hsw=1;				  	// ˮƽͬ�����
		lcdltdc.vsw=1;				  	// ��ֱͬ�����
		lcdltdc.hbp=10;				  	// ˮƽ���� 0.5us
		lcdltdc.vbp=10;				  	// ��ֱ����
		lcdltdc.hfp=10;			 			// ˮƽǰ�� 0.5us
		lcdltdc.vfp=43;				    // ��ֱǰ��
    LTDC_Clk_Set(360,3,RCC_PLLSAIDIVR_4); // ��������ʱ�� 33M(�����˫��,��Ҫ����DCLK��:18.75Mhz  300/4/4,�Ż�ȽϺ�)
	}
	else if(lcdid==0X7016)		
	{
		lcdltdc.pwidth=1024;			// �����,��λ:����
		lcdltdc.pheight=600;			// ���߶�,��λ:����
    lcdltdc.hsw=20;						// ˮƽͬ�����
		lcdltdc.vsw=3;				    // ��ֱͬ�����
		lcdltdc.hbp=140;			    // ˮƽ����
		lcdltdc.vbp=20;				    // ��ֱ����
		lcdltdc.hfp=160;			    // ˮƽǰ��
		lcdltdc.vfp=12;				    // ��ֱǰ��
		LTDC_Clk_Set(360,2,RCC_PLLSAIDIVR_4);
		// ������������.
	}

	lcddev.width=lcdltdc.pwidth;
	lcddev.height=lcdltdc.pheight;
    
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888 
	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;
	lcdltdc.pixsize=4;				  // ÿ������ռ4���ֽ�
#else 
    lcdltdc.pixsize=2;				// ÿ������ռ2���ֽ�
	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;
#endif 	
	if(lcdid==0X8000)
	{
		// LTDC����
		LTDC_Handler.Instance=LTDC;
		LTDC_Handler.Init.HSPolarity=LTDC_HSPOLARITY_AL;         // ˮƽͬ������
		LTDC_Handler.Init.VSPolarity=LTDC_VSPOLARITY_AL;         // ��ֱͬ������
		LTDC_Handler.Init.DEPolarity=LTDC_DEPOLARITY_AL;         // ����ʹ�ܼ���
		LTDC_Handler.Init.PCPolarity=LTDC_PCPOLARITY_IIPC;       // ����ʱ�Ӽ���
		LTDC_Handler.Init.HorizontalSync=lcdltdc.hsw-1;          // ˮƽͬ�����
		LTDC_Handler.Init.VerticalSync=lcdltdc.vsw-1;            // ��ֱͬ�����
		LTDC_Handler.Init.AccumulatedHBP=lcdltdc.hsw+lcdltdc.hbp-1; // ˮƽͬ�����ؿ��
		LTDC_Handler.Init.AccumulatedVBP=lcdltdc.vsw+lcdltdc.vbp-1; // ��ֱͬ�����ظ߶�
		LTDC_Handler.Init.AccumulatedActiveW=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pheight-1;// ��Ч���
		LTDC_Handler.Init.AccumulatedActiveH=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pwidth-1; // ��Ч�߶�
		LTDC_Handler.Init.TotalWidth=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pheight+lcdltdc.hfp-1;   // �ܿ��
		LTDC_Handler.Init.TotalHeigh=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pwidth+lcdltdc.vfp-1;  	// �ܸ߶�
		LTDC_Handler.Init.Backcolor.Red=0;           // ��Ļ�������ɫ����
		LTDC_Handler.Init.Backcolor.Green=0;         // ��Ļ��������ɫ����
		LTDC_Handler.Init.Backcolor.Blue=0;          // ��Ļ����ɫ��ɫ����
		HAL_LTDC_Init(&LTDC_Handler);
		
		// ������
		LTDC_Layer_Parameter_Config(0,(u32)ltdc_framebuf[0],LCD_PIXFORMAT,255,0,6,7,0X000000);// ���������
		LTDC_Layer_Window_Config(0,0,0,lcdltdc.pheight,lcdltdc.pwidth);	// �㴰������,��LCD�������ϵΪ��׼,��Ҫ����޸�!
	}
	else if(lcdid==0X7016)	
	{
		// LTDC����
		LTDC_Handler.Instance=LTDC;
		LTDC_Handler.Init.HSPolarity=LTDC_HSPOLARITY_AL;         // ˮƽͬ������
		LTDC_Handler.Init.VSPolarity=LTDC_VSPOLARITY_AL;         // ��ֱͬ������
		LTDC_Handler.Init.DEPolarity=LTDC_DEPOLARITY_AL;         // ����ʹ�ܼ���
		LTDC_Handler.Init.PCPolarity=LTDC_PCPOLARITY_IPC;        // ����ʱ�Ӽ���
		LTDC_Handler.Init.HorizontalSync=lcdltdc.hsw-1;          // ˮƽͬ�����
		LTDC_Handler.Init.VerticalSync=lcdltdc.vsw-1;            // ��ֱͬ�����
		LTDC_Handler.Init.AccumulatedHBP=lcdltdc.hsw+lcdltdc.hbp-1; // ˮƽͬ�����ؿ��
		LTDC_Handler.Init.AccumulatedVBP=lcdltdc.vsw+lcdltdc.vbp-1; // ��ֱͬ�����ظ߶�
		LTDC_Handler.Init.AccumulatedActiveW=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth-1; // ��Ч���
		LTDC_Handler.Init.AccumulatedActiveH=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight-1;// ��Ч�߶�
		LTDC_Handler.Init.TotalWidth=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth+lcdltdc.hfp-1;   // �ܿ��
		LTDC_Handler.Init.TotalHeigh=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight+lcdltdc.vfp-1;  // �ܸ߶�
		LTDC_Handler.Init.Backcolor.Red=0;           // ��Ļ�������ɫ����
		LTDC_Handler.Init.Backcolor.Green=0;         // ��Ļ��������ɫ����
		LTDC_Handler.Init.Backcolor.Blue=0;          // ��Ļ����ɫ��ɫ����
		HAL_LTDC_Init(&LTDC_Handler);
		
		// ������
		LTDC_Layer_Parameter_Config(0,(u32)ltdc_framebuf[0],LCD_PIXFORMAT,255,0,6,7,0X000000);// ���������
		LTDC_Layer_Window_Config(0,0,0,lcdltdc.pwidth,lcdltdc.pheight);	// �㴰������,��LCD�������ϵΪ��׼,��Ҫ����޸�!
	}
	LTDC_Display_Dir( LCD_DIR );// ������ʾ����
	LTDC_Select_Layer(0); 			// ѡ���1��
	LTCD_LED=1;         		    // ��������
	LTDC_Clear(0XFFFFFFFF);			// ����
}

//-----------------------------------------------------------------
// void LTDC_Init(void)
//-----------------------------------------------------------------
// 
// ��������: LTDC�ײ�IO��ʼ����ʱ��ʹ��
// ��ڲ���: LTDC_HandleTypeDef* hltdc��hltdc:LTDC���
// �� �� ֵ: ��
// ע������: �˺����ᱻHAL_LTDC_Init()����
//					 LTDC_R3 -> PH9					
//					 LTDC_R4 -> PH10				RGB_BL 		 -> PC6
//					 LTDC_R5 -> PH11				LTDC_DE 	 -> PF10
//					 LTDC_R6 -> PH12				LTDC_VSYNC -> PI9
//					 LTDC_R7 -> PG6					LTDC_HSYNC -> PI10
//					 LTDC_G2 -> PH13				LTDC_CLK   -> PG7
//					 LTDC_G3 -> PH14
//					 LTDC_G4 -> PH15
//					 LTDC_G5 -> PI0
//					 LTDC_G6 -> PC7
//					 LTDC_G7 -> PI2
//					 LTDC_B3 -> PG11
//					 LTDC_B4 -> PI4
//					 LTDC_B5 -> PI5
//					 LTDC_B6 -> PI6
//					 LTDC_B7 -> PI7
//
//-----------------------------------------------------------------
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_LTDC_CLK_ENABLE();                // ʹ��LTDCʱ��
    __HAL_RCC_DMA2D_CLK_ENABLE();               // ʹ��DMA2Dʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();               // ʹ��GPIOBʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();               // ʹ��GPIOCʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();               // ʹ��GPIOFʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();               // ʹ��GPIOGʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();               // ʹ��GPIOHʱ��
    __HAL_RCC_GPIOI_CLK_ENABLE();               // ʹ��GPIOIʱ��
    
    // ��ʼ��PC6����������
    GPIO_Initure.Pin=GPIO_PIN_6;                // PC6������������Ʊ���
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;      // �������
    GPIO_Initure.Pull=GPIO_PULLUP;              // ����        
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         // ����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);					// ��ʼ��
    
    // ��ʼ��PF10
    GPIO_Initure.Pin=GPIO_PIN_10; 							// ��ʼ��PF10
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          // ����
    GPIO_Initure.Pull=GPIO_NOPULL;              // ����������
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         // ����
    GPIO_Initure.Alternate=GPIO_AF14_LTDC;      // ����ΪLTDC
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);					// ��ʼ��
    
    // ��ʼ��PG6,7,11
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    
    // ��ʼ��PH9,10,11,12,13��14��15
    GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    // ��ʼ��PI0,2,4,5,6,7,9,10
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|\
                     GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI,&GPIO_Initure); 
    // ��ʼ��PC7,
    GPIO_Initure.Pin=GPIO_PIN_7;                
    HAL_GPIO_Init(GPIOC,&GPIO_Initure); 
}

//-----------------------------------------------------------------
// static void LCD_GPIO_Config(void)
//-----------------------------------------------------------------
//
// ��������: 4.3��RGB�������ŷ���,SPI����
// ��ڲ���: ��
// ���ز���: ��
// ע������: ��
//					 
//-----------------------------------------------------------------
static void LCD_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();			// ʹ��GPIOAʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();			// ʹ��GPIOCʱ��

	GPIO_Initure.Pin=GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2; // PA0 -> T_MISO, PA1 - > T_MOSI , PA2 -> T_CS
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  // �������
	GPIO_Initure.Pull=GPIO_PULLUP;          // ����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     // ����
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_2 | GPIO_PIN_6; // PC2 -> T_CLK, PC6 -> RGB_BL
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);	 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);	
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);	
}

//-----------------------------------------------------------------
// void LCD_WriteByteSPI(unsigned char byte)
//-----------------------------------------------------------------
//
// ��������: SPIд����
// ��ڲ���: unsigned char byte��д�������
// ���ز���: ��
// ע������: ��
//					 
//-----------------------------------------------------------------
void LCD_WriteByteSPI(unsigned char byte)
{
	int i;
	for(i=7;i>=0;i--)
	{        
		SPI_DCLK(0);
		if(byte & (1<<i))
			SPI_SDA(1)
		else
			SPI_SDA(0)
		delay_us(1);
		SPI_DCLK(1);
		delay_us(1);
	}
}
//-----------------------------------------------------------------
// void SPI_WriteComm(u16 CMD)
//-----------------------------------------------------------------
//
// ��������: SPIд����
// ��ڲ���: uu16 CMD������
// ���ز���: ��
// ע������: ��
//					 
//-----------------------------------------------------------------
void SPI_WriteComm(u16 CMD)
{			
	delay_us(10);	
	LCD_SPI_CS(0);
	delay_us(1);	
	LCD_WriteByteSPI(0X20);
	delay_us(1);	
	LCD_WriteByteSPI(CMD>>8);
	delay_us(1);	
	LCD_SPI_CS(1);
	
	delay_us(10);	
	LCD_SPI_CS(0);
	delay_us(1);	
	LCD_WriteByteSPI(0X00);
	delay_us(1);	
	LCD_WriteByteSPI(CMD);
	delay_us(1);	
	LCD_SPI_CS(1);
	delay_us(1);	
}
//-----------------------------------------------------------------
// void SPI_WriteData(u16 tem_data)
//-----------------------------------------------------------------
//
// ��������: SPIд����
// ��ڲ���: u16 tem_data��д�������
// ���ز���: ��
// ע������: ��
//					 
//-----------------------------------------------------------------
void SPI_WriteData(u16 tem_data)
{			
	delay_us(10);	
	LCD_SPI_CS(0);
	delay_us(1);	
	LCD_WriteByteSPI(0x40);
	delay_us(1);	
	LCD_WriteByteSPI(tem_data);
	delay_us(1);	
	LCD_SPI_CS(1);
	delay_us(1);	
}

//-----------------------------------------------------------------
// void Lcd_Initialize(void)
//-----------------------------------------------------------------
//
// ��������: RGB����
// ��ڲ���: ��
// ���ز���: ��
// ע������: ��
//					 
//-----------------------------------------------------------------
void Lcd_Initialize(void)
{	
	LCD_GPIO_Config();
	delay_ms(20);	
	SPI_WriteComm(0x0000);		// NOP 
	SPI_WriteComm(0x0100);		// Software Reset 
	delay_ms(200);						// 200ms
	
	SPI_WriteComm(0xf000);SPI_WriteData(0x0055);	
	SPI_WriteComm(0xf001);SPI_WriteData(0x00aa);	
	SPI_WriteComm(0xf002);SPI_WriteData(0x0052);	
	SPI_WriteComm(0xf003);SPI_WriteData(0x0008);	
	SPI_WriteComm(0xf004);SPI_WriteData(0x0001);	
																						 
	SPI_WriteComm(0xbc01);SPI_WriteData(0x0086);	
	SPI_WriteComm(0xbc02);SPI_WriteData(0x006a);	
	SPI_WriteComm(0xbd01);SPI_WriteData(0x0086);	
	SPI_WriteComm(0xbd02);SPI_WriteData(0x006a);	
	SPI_WriteComm(0xbe01);SPI_WriteData(0x0067);	
																						 
	SPI_WriteComm(0xd100);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd101);SPI_WriteData(0x005d);	
	SPI_WriteComm(0xd102);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd103);SPI_WriteData(0x006b);	
	SPI_WriteComm(0xd104);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd105);SPI_WriteData(0x0084);	
	SPI_WriteComm(0xd106);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd107);SPI_WriteData(0x009c);	
	SPI_WriteComm(0xd108);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd109);SPI_WriteData(0x00b1);	
	SPI_WriteComm(0xd10a);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd10b);SPI_WriteData(0x00d9);	
	SPI_WriteComm(0xd10c);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd10d);SPI_WriteData(0x00fd);	
	SPI_WriteComm(0xd10e);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd10f);SPI_WriteData(0x0038);	
	SPI_WriteComm(0xd110);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd111);SPI_WriteData(0x0068);	
	SPI_WriteComm(0xd112);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd113);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd114);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd115);SPI_WriteData(0x00fb);	
	SPI_WriteComm(0xd116);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd117);SPI_WriteData(0x0063);	
	SPI_WriteComm(0xd118);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd119);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd11a);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd11b);SPI_WriteData(0x00bb);	
	SPI_WriteComm(0xd11c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd11d);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd11e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd11f);SPI_WriteData(0x0046);	
	SPI_WriteComm(0xd120);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd121);SPI_WriteData(0x0069);	
	SPI_WriteComm(0xd122);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd123);SPI_WriteData(0x008f);	
	SPI_WriteComm(0xd124);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd125);SPI_WriteData(0x00a4);	
	SPI_WriteComm(0xd126);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd127);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd128);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd129);SPI_WriteData(0x00c7);	
	SPI_WriteComm(0xd12a);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd12b);SPI_WriteData(0x00c9);	
	SPI_WriteComm(0xd12c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd12d);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd12e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd12f);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd130);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd131);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd132);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd133);SPI_WriteData(0x00cc);	
																						 
	SPI_WriteComm(0xd200);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd201);SPI_WriteData(0x005d);	
	SPI_WriteComm(0xd202);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd203);SPI_WriteData(0x006b);	
	SPI_WriteComm(0xd204);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd205);SPI_WriteData(0x0084);	
	SPI_WriteComm(0xd206);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd207);SPI_WriteData(0x009c);	
	SPI_WriteComm(0xd208);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd209);SPI_WriteData(0x00b1);	
	SPI_WriteComm(0xd20a);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd20b);SPI_WriteData(0x00d9);	
	SPI_WriteComm(0xd20c);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd20d);SPI_WriteData(0x00fd);	
	SPI_WriteComm(0xd20e);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd20f);SPI_WriteData(0x0038);	
	SPI_WriteComm(0xd210);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd211);SPI_WriteData(0x0068);	
	SPI_WriteComm(0xd212);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd213);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd214);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd215);SPI_WriteData(0x00fb);	
	SPI_WriteComm(0xd216);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd217);SPI_WriteData(0x0063);	
	SPI_WriteComm(0xd218);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd219);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd21a);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd21b);SPI_WriteData(0x00bb);	
	SPI_WriteComm(0xd21c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd21d);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd21e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd21f);SPI_WriteData(0x0046);	
	SPI_WriteComm(0xd220);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd221);SPI_WriteData(0x0069);	
	SPI_WriteComm(0xd222);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd223);SPI_WriteData(0x008f);	
	SPI_WriteComm(0xd224);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd225);SPI_WriteData(0x00a4);	
	SPI_WriteComm(0xd226);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd227);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd228);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd229);SPI_WriteData(0x00c7);	
	SPI_WriteComm(0xd22a);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd22b);SPI_WriteData(0x00c9);	
	SPI_WriteComm(0xd22c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd22d);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd22e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd22f);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd230);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd231);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd232);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd233);SPI_WriteData(0x00cc);	
																						 
																						 
	SPI_WriteComm(0xd300);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd301);SPI_WriteData(0x005d);	
	SPI_WriteComm(0xd302);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd303);SPI_WriteData(0x006b);	
	SPI_WriteComm(0xd304);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd305);SPI_WriteData(0x0084);	
	SPI_WriteComm(0xd306);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd307);SPI_WriteData(0x009c);	
	SPI_WriteComm(0xd308);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd309);SPI_WriteData(0x00b1);	
	SPI_WriteComm(0xd30a);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd30b);SPI_WriteData(0x00d9);	
	SPI_WriteComm(0xd30c);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd30d);SPI_WriteData(0x00fd);	
	SPI_WriteComm(0xd30e);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd30f);SPI_WriteData(0x0038);	
	SPI_WriteComm(0xd310);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd311);SPI_WriteData(0x0068);	
	SPI_WriteComm(0xd312);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd313);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd314);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd315);SPI_WriteData(0x00fb);	
	SPI_WriteComm(0xd316);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd317);SPI_WriteData(0x0063);	
	SPI_WriteComm(0xd318);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd319);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd31a);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd31b);SPI_WriteData(0x00bb);	
	SPI_WriteComm(0xd31c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd31d);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd31e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd31f);SPI_WriteData(0x0046);	
	SPI_WriteComm(0xd320);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd321);SPI_WriteData(0x0069);	
	SPI_WriteComm(0xd322);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd323);SPI_WriteData(0x008f);	
	SPI_WriteComm(0xd324);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd325);SPI_WriteData(0x00a4);	
	SPI_WriteComm(0xd326);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd327);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd328);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd329);SPI_WriteData(0x00c7);	
	SPI_WriteComm(0xd32a);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd32b);SPI_WriteData(0x00c9);	
	SPI_WriteComm(0xd32c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd32d);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd32e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd32f);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd330);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd331);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd332);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd333);SPI_WriteData(0x00cc);	
																						 
	SPI_WriteComm(0xd400);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd401);SPI_WriteData(0x005d);	
	SPI_WriteComm(0xd402);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd403);SPI_WriteData(0x006b);	
	SPI_WriteComm(0xd404);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd405);SPI_WriteData(0x0084);	
	SPI_WriteComm(0xd406);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd407);SPI_WriteData(0x009c);	
	SPI_WriteComm(0xd408);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd409);SPI_WriteData(0x00b1);	
	SPI_WriteComm(0xd40a);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd40b);SPI_WriteData(0x00d9);	
	SPI_WriteComm(0xd40c);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd40d);SPI_WriteData(0x00fd);	
	SPI_WriteComm(0xd40e);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd40f);SPI_WriteData(0x0038);	
	SPI_WriteComm(0xd410);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd411);SPI_WriteData(0x0068);	
	SPI_WriteComm(0xd412);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd413);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd414);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd415);SPI_WriteData(0x00fb);	
	SPI_WriteComm(0xd416);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd417);SPI_WriteData(0x0063);	
	SPI_WriteComm(0xd418);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd419);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd41a);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd41b);SPI_WriteData(0x00bb);	
	SPI_WriteComm(0xd41c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd41d);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd41e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd41f);SPI_WriteData(0x0046);	
	SPI_WriteComm(0xd420);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd421);SPI_WriteData(0x0069);	
	SPI_WriteComm(0xd422);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd423);SPI_WriteData(0x008f);	
	SPI_WriteComm(0xd424);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd425);SPI_WriteData(0x00a4);	
	SPI_WriteComm(0xd426);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd427);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd428);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd429);SPI_WriteData(0x00c7);	
	SPI_WriteComm(0xd42a);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd42b);SPI_WriteData(0x00c9);	
	SPI_WriteComm(0xd42c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd42d);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd42e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd42f);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd430);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd431);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd432);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd433);SPI_WriteData(0x00cc);	
																						 
																						 
	SPI_WriteComm(0xd500);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd501);SPI_WriteData(0x005d);	
	SPI_WriteComm(0xd502);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd503);SPI_WriteData(0x006b);	
	SPI_WriteComm(0xd504);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd505);SPI_WriteData(0x0084);	
	SPI_WriteComm(0xd506);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd507);SPI_WriteData(0x009c);	
	SPI_WriteComm(0xd508);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd509);SPI_WriteData(0x00b1);	
	SPI_WriteComm(0xd50a);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd50b);SPI_WriteData(0x00D9);	
	SPI_WriteComm(0xd50c);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd50d);SPI_WriteData(0x00fd);	
	SPI_WriteComm(0xd50e);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd50f);SPI_WriteData(0x0038);	
	SPI_WriteComm(0xd510);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd511);SPI_WriteData(0x0068);	
	SPI_WriteComm(0xd512);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd513);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd514);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd515);SPI_WriteData(0x00fb);	
	SPI_WriteComm(0xd516);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd517);SPI_WriteData(0x0063);	
	SPI_WriteComm(0xd518);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd519);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd51a);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd51b);SPI_WriteData(0x00bb);	
	SPI_WriteComm(0xd51c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd51d);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd51e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd51f);SPI_WriteData(0x0046);	
	SPI_WriteComm(0xd520);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd521);SPI_WriteData(0x0069);	
	SPI_WriteComm(0xd522);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd523);SPI_WriteData(0x008f);	
	SPI_WriteComm(0xd524);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd525);SPI_WriteData(0x00a4);	
	SPI_WriteComm(0xd526);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd527);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd528);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd529);SPI_WriteData(0x00c7);	
	SPI_WriteComm(0xd52a);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd52b);SPI_WriteData(0x00c9);	
	SPI_WriteComm(0xd52c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd52d);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd52e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd52f);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd530);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd531);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd532);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd533);SPI_WriteData(0x00cc);	
																						 
	SPI_WriteComm(0xd600);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd601);SPI_WriteData(0x005d);	
	SPI_WriteComm(0xd602);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd603);SPI_WriteData(0x006b);	
	SPI_WriteComm(0xd604);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd605);SPI_WriteData(0x0084);	
	SPI_WriteComm(0xd606);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd607);SPI_WriteData(0x009c);	
	SPI_WriteComm(0xd608);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd609);SPI_WriteData(0x00b1);	
	SPI_WriteComm(0xd60a);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd60b);SPI_WriteData(0x00d9);	
	SPI_WriteComm(0xd60c);SPI_WriteData(0x0000);	
	SPI_WriteComm(0xd60d);SPI_WriteData(0x00fd);	
	SPI_WriteComm(0xd60e);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd60f);SPI_WriteData(0x0038);	
	SPI_WriteComm(0xd610);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd611);SPI_WriteData(0x0068);	
	SPI_WriteComm(0xd612);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd613);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd614);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xd615);SPI_WriteData(0x00fb);	
	SPI_WriteComm(0xd616);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd617);SPI_WriteData(0x0063);	
	SPI_WriteComm(0xd618);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd619);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd61a);SPI_WriteData(0x0002);	
	SPI_WriteComm(0xd61b);SPI_WriteData(0x00bb);	
	SPI_WriteComm(0xd61c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd61d);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd61e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd61f);SPI_WriteData(0x0046);	
	SPI_WriteComm(0xd620);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd621);SPI_WriteData(0x0069);	
	SPI_WriteComm(0xd622);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd623);SPI_WriteData(0x008f);	
	SPI_WriteComm(0xd624);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd625);SPI_WriteData(0x00a4);	
	SPI_WriteComm(0xd626);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd627);SPI_WriteData(0x00b9);	
	SPI_WriteComm(0xd628);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd629);SPI_WriteData(0x00c7);	
	SPI_WriteComm(0xd62a);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd62b);SPI_WriteData(0x00c9);	
	SPI_WriteComm(0xd62c);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd62d);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd62e);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd62f);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd630);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd631);SPI_WriteData(0x00cb);	
	SPI_WriteComm(0xd632);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xd633);SPI_WriteData(0x00cc);	
																						 
	SPI_WriteComm(0xba00);SPI_WriteData(0x0024);	
	SPI_WriteComm(0xba01);SPI_WriteData(0x0024);	
	SPI_WriteComm(0xba02);SPI_WriteData(0x0024);	
																						 
	SPI_WriteComm(0xb900);SPI_WriteData(0x0024);	
	SPI_WriteComm(0xb901);SPI_WriteData(0x0024);	
	SPI_WriteComm(0xb902);SPI_WriteData(0x0024);	

	SPI_WriteComm(0xf000);SPI_WriteData(0x0055);          
	SPI_WriteComm(0xf001);SPI_WriteData(0x00aa);	
	SPI_WriteComm(0xf002);SPI_WriteData(0x0052);	
	SPI_WriteComm(0xf003);SPI_WriteData(0x0008);	
	SPI_WriteComm(0xf004);SPI_WriteData(0x0000);	
																						 
																						 
	SPI_WriteComm(0xb100);SPI_WriteData(0x00cc);	
																						 
																						 
	SPI_WriteComm(0xbc00);SPI_WriteData(0x0005);	
	SPI_WriteComm(0xbc01);SPI_WriteData(0x0005);	
	SPI_WriteComm(0xbc02);SPI_WriteData(0x0005);	
																						 
	SPI_WriteComm(0xb800);SPI_WriteData(0x0001);	
	SPI_WriteComm(0xb801);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xb802);SPI_WriteData(0x0003);	
	SPI_WriteComm(0xb803);SPI_WriteData(0x0003);	
																						 
																						 
	SPI_WriteComm(0xbd02);SPI_WriteData(0x0007);	
	SPI_WriteComm(0xbd03);SPI_WriteData(0x0031);	
	SPI_WriteComm(0xbe02);SPI_WriteData(0x0007);	
	SPI_WriteComm(0xbe03);SPI_WriteData(0x0031);	
	SPI_WriteComm(0xbf02);SPI_WriteData(0x0007);	
	SPI_WriteComm(0xbf03);SPI_WriteData(0x0031);	
																						 
																						 
	SPI_WriteComm(0xff00);SPI_WriteData(0x00aa);	
	SPI_WriteComm(0xff01);SPI_WriteData(0x0055);	
	SPI_WriteComm(0xff02);SPI_WriteData(0x0025);	
	SPI_WriteComm(0xff03);SPI_WriteData(0x0001);	


	SPI_WriteComm(0xf304);SPI_WriteData(0x0011);	
	SPI_WriteComm(0xf306);SPI_WriteData(0x0010);	
	SPI_WriteComm(0xf308);SPI_WriteData(0x0000);	
																						 
	SPI_WriteComm(0x3500);SPI_WriteData(0x0000);	

	SPI_WriteComm(0x3A00);SPI_WriteData(0x0005);


	SPI_WriteComm(0x2a00);SPI_WriteData(0x0000);
	SPI_WriteComm(0x2a01);SPI_WriteData(0x0000);	
	SPI_WriteComm(0x2a02);SPI_WriteData(0x0001);	
	SPI_WriteComm(0x2a03);SPI_WriteData(0x00df);	
																						 
	SPI_WriteComm(0x2b00);SPI_WriteData(0x0000);  
	SPI_WriteComm(0x2b01);SPI_WriteData(0x0000);	
	SPI_WriteComm(0x2b02);SPI_WriteData(0x0003);	
	SPI_WriteComm(0x2b03);SPI_WriteData(0x001f);	



	SPI_WriteComm(0x1100);
	delay_ms(120);

	SPI_WriteComm(0x2900);

	SPI_WriteComm(0x2c00);
	SPI_WriteComm(0x3c00);

	SPI_WriteComm(0x3A); SPI_WriteData(0x75);
	SPI_WriteComm(0x36); SPI_WriteData(0x00);
	
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------  
