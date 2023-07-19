//-----------------------------------------------------------------
// ��������:
//     STM32��FPGA��SPIͨѶ��������ͷ�ļ�
// ��    ��: ���ǵ���
// ��ʼ����: 2018-08-04
// �������: 2018-08-04
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0: (2018-08-04)	STM32��FPGA��SPIͨѶ
// ���Թ���: ����STM32F429+CycloneIV����ϵͳ��ƿ����塢LZE_ST_LINK2
// ˵    ��: 
//    
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#ifndef __SPI_H
#define __SPI_H
#include "stm32f429_winner.h"
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// �궨��
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

#define	DATA_WIDTH 32	 // ���巢�����ݵĿ��---ע��:���Ҫ��������λ��,��Ӧ�ķ��ͺͽ������ݳ���ҲҪ����
#define	CMD_WIDTH  8   // ��ַλ���

//-----------------------------------------------------------------
// ��������
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
