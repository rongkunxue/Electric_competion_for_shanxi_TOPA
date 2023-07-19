//-----------------------------------------------------------------
// 程序描述:
//     STM32与FPGA的SPI通讯驱动程序头文件
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
#ifndef __SPI_H
#define __SPI_H
#include "stm32f429_winner.h"
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 宏定义
//-----------------------------------------------------------------
//#define SPI_FPGA_MISO 		HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)
#define SPI_FPGA_MISO 		PEin(2);
#define SPI_FPGA_SCL_Set 	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET) 
#define SPI_FPGA_SCL_Clr	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET)
#define CS_CMD_Set 				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET) 
#define CS_CMD_Clr 				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET)
#define CS_DATA_Set 			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET) 
#define CS_DATA_Clr 			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET)
#define SPI_FPGA_MOSI_Set	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET) 
#define SPI_FPGA_MOSI_Clr	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET)

#define	DATA_WIDTH 32	 // 定义发送数据的宽度---注意:如果要更改数据位宽,对应的发送和接收数据程序也要更改
#define	CMD_WIDTH  8   // 地址位宽度

//-----------------------------------------------------------------
// 函数声明
//-----------------------------------------------------------------
void SPI_Communication_Init(void);
void SPI_Communication_Send_Cmd(u8 Send_Data);
void SPI_Communication_Send_Data(u32 Send_Data);
u32	SPI_Communication_Rece_Data(void);
void SPI_Communication_Send_Cmd_Data(u8 Cmd,u32 Send_Data);
u32	SPI_Communication_Rece_Cmd_Data(u8 cmd);

#endif

//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
