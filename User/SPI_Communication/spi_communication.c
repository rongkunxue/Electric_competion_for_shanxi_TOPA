//-----------------------------------------------------------------
// 程序描述:
//     STM32与FPGA的SPI通讯驱动程序
// 作    者: 凌智电子
// 开始日期: 2018-08-04
// 完成日期: 2018-08-04
// 修改日期: 
// 当前版本: V1.0
// 历史版本:
//  - V1.0: (2018-08-04)	STM32与FPGA的SPI通讯
// 调试工具: 凌智STM32F429+CycloneIV电子系统设计开发板、LZE_ST_LINK2
// 说    明: 
//    
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include "spi_communication.h"
#include "delay.h"
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// void SPI_Communication_Init(void)
//-----------------------------------------------------------------
// 
// 函数功能: SPI IO配置
// 入口参数: 无 
// 返 回 值: 无  
// 注意事项: 引脚连接如下：
//		SPI_FPGA_MISO			PE2				
//		SPI_FPGA_SCL			PE3
//		CS_CMD						PE4
//		CS_DATA						PE5
//		SPI_FPGA_MOSI			PE6 
//
//-----------------------------------------------------------------
void SPI_Communication_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_GPIOE_CLK_ENABLE();           					// 开启GPIOB时钟
	
	GPIO_Initure.Pin=GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;	// PE3 4 5 6
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  					// 推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          					// 上拉
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     // 高速
  HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_2;											// PE2
  GPIO_Initure.Mode=GPIO_MODE_INPUT;  							// 推挽输出
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     // 高速
  HAL_GPIO_Init(GPIOE,&GPIO_Initure);

	SPI_FPGA_MOSI_Set;	
	SPI_FPGA_SCL_Set;
	CS_CMD_Set;
	CS_DATA_Set;
}

//-----------------------------------------------------------------
// void SPI_Communication_Send_Cmd(u8 Send_Data)
//-----------------------------------------------------------------
//
// 函数功能: 发送8位的命令(默认)
// 入口参数: u8 Send_Data：要发送的命令
// 返 回 值: 无
// 注意事项: 时钟是上升沿的时候发送命令片选低电平有效有效
//
//-----------------------------------------------------------------
void SPI_Communication_Send_Cmd(u8 Send_Data)
{	
	u8	i;
	CS_CMD_Clr;																// 片选选中
	for(i=0;i<CMD_WIDTH;i++)
	{		
		if((Send_Data&0x80) == 0x80) 
		{
			SPI_FPGA_MOSI_Set;
		}
		else 
		{
			SPI_FPGA_MOSI_Clr;
		}				
		SPI_FPGA_SCL_Set;												// 上升沿发送数据
		Send_Data <<= 1;												// 高位在前
		SPI_FPGA_SCL_Clr;
	}
	CS_CMD_Set;																// 片选拉高
}

//-----------------------------------------------------------------
// void SPI_Communication_Send_Data(u32 Send_Data)
//-----------------------------------------------------------------
//
// 函数功能: spi发送数据
// 入口参数: 待发的数据
// 返 回 值: 无
// 注意事项: 时钟是上升沿的时候发送数据,片选低电平有效有效
//					 数据类型是32位无符号长整形,数据类型可以根据需要更改    
//
//-----------------------------------------------------------------
void SPI_Communication_Send_Data(u32 Send_Data)
{	
	u8	i;
	CS_DATA_Clr;															// 片选选中
	for(i=0;i<DATA_WIDTH;i++)
	{ 
		if((Send_Data&0x80000000) == 0x80000000)
		{
			SPI_FPGA_MOSI_Set;
		}
		else
		{
			SPI_FPGA_MOSI_Clr;
		}				
		SPI_FPGA_SCL_Set;												// 上升沿发送数据
		Send_Data <<= 1				;									// 高位在前
		SPI_FPGA_SCL_Clr;
	}
	CS_DATA_Set;														  // 片选拉高
}

//-----------------------------------------------------------------
// u32	SPI_Communication_Rece_Data(void)
//-----------------------------------------------------------------
//
// 函数功能: spi接收数据---数据类型是32位无符号长整形,数据类型
// 入口参数: 无
// 返 回 值: 接收到的数据
// 注意事项: 时钟是下降接收数据,片选下降沿有效
//
//-----------------------------------------------------------------
u32	SPI_Communication_Rece_Data(void)
{
	u8	i;
	u32 Data_Buf=0;														// 接收到得数据
	
	CS_DATA_Clr;															// 片选选中
	for(i=0;i<DATA_WIDTH;i++)
	{
		Data_Buf<<=1;														// 左移	
		Data_Buf = Data_Buf | SPI_FPGA_MISO;		// 接收数据
		SPI_FPGA_SCL_Set;	
		SPI_FPGA_SCL_Clr;												// 下降沿接收数据	
	}
	CS_DATA_Set;															// 片选拉高
	
	return Data_Buf;
}

//-----------------------------------------------------------------
// void SPI_Communication_Send_Cmd_Data(u8 Cmd,u32 Send_Data)
//-----------------------------------------------------------------
//
// 函数功能: 先发送命令,在发送数据
// 入口参数: 要发送的命令,要发送的数据
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void SPI_Communication_Send_Cmd_Data(u8 Cmd,u32 Send_Data)
{
	delay_us(1);
	SPI_Communication_Send_Cmd(Cmd);
	delay_us(1);
	SPI_Communication_Send_Data(Send_Data);
}

//-----------------------------------------------------------------
// u32	SPI_Communication_Rece_Cmd_Data(u8 cmd)
//-----------------------------------------------------------------
//
// 函数功能: 先发送命令,在接收数据
// 入口参数: 要发送的命令
// 返 回 值: 接收到的数据
// 注意事项: 无
//
//-----------------------------------------------------------------
u32	SPI_Communication_Rece_Cmd_Data(u8 cmd)
{
	delay_us(1);
	SPI_Communication_Send_Cmd(cmd);
	delay_us(1);
	return(SPI_Communication_Rece_Data());
}

//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
