//-----------------------------------------------------------------
// ��������:
//     STM32��FPGA��SPIͨѶ��������
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
#include "spi_communication.h"
#include "delay.h"
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// void SPI_Communication_Init(void)
//-----------------------------------------------------------------
// 
// ��������: SPI IO����
// ��ڲ���: �� 
// �� �� ֵ: ��  
// ע������: �����������£�
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
  __HAL_RCC_GPIOE_CLK_ENABLE();           					// ����GPIOBʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;	// PE3 4 5 6
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  					// �������
	GPIO_Initure.Pull=GPIO_PULLUP;          					// ����
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     // ����
  HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_2;											// PE2
  GPIO_Initure.Mode=GPIO_MODE_INPUT;  							// �������
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     // ����
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
// ��������: ����8λ������(Ĭ��)
// ��ڲ���: u8 Send_Data��Ҫ���͵�����
// �� �� ֵ: ��
// ע������: ʱ���������ص�ʱ��������Ƭѡ�͵�ƽ��Ч��Ч
//
//-----------------------------------------------------------------
void SPI_Communication_Send_Cmd(u8 Send_Data)
{	
	u8	i;
	CS_CMD_Clr;																// Ƭѡѡ��
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
		SPI_FPGA_SCL_Set;												// �����ط�������
		Send_Data <<= 1;												// ��λ��ǰ
		SPI_FPGA_SCL_Clr;
	}
	CS_CMD_Set;																// Ƭѡ����
}

//-----------------------------------------------------------------
// void SPI_Communication_Send_Data(u32 Send_Data)
//-----------------------------------------------------------------
//
// ��������: spi��������
// ��ڲ���: ����������
// �� �� ֵ: ��
// ע������: ʱ���������ص�ʱ��������,Ƭѡ�͵�ƽ��Ч��Ч
//					 ����������32λ�޷��ų�����,�������Ϳ��Ը�����Ҫ����    
//
//-----------------------------------------------------------------
void SPI_Communication_Send_Data(u32 Send_Data)
{	
	u8	i;
	CS_DATA_Clr;															// Ƭѡѡ��
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
		SPI_FPGA_SCL_Set;												// �����ط�������
		Send_Data <<= 1				;									// ��λ��ǰ
		SPI_FPGA_SCL_Clr;
	}
	CS_DATA_Set;														  // Ƭѡ����
}

//-----------------------------------------------------------------
// u32	SPI_Communication_Rece_Data(void)
//-----------------------------------------------------------------
//
// ��������: spi��������---����������32λ�޷��ų�����,��������
// ��ڲ���: ��
// �� �� ֵ: ���յ�������
// ע������: ʱ�����½���������,Ƭѡ�½�����Ч
//
//-----------------------------------------------------------------
u32	SPI_Communication_Rece_Data(void)
{
	u8	i;
	u32 Data_Buf=0;														// ���յ�������
	
	CS_DATA_Clr;															// Ƭѡѡ��
	for(i=0;i<DATA_WIDTH;i++)
	{
		Data_Buf<<=1;														// ����	
		Data_Buf = Data_Buf | SPI_FPGA_MISO;		// ��������
		SPI_FPGA_SCL_Set;	
		SPI_FPGA_SCL_Clr;												// �½��ؽ�������	
	}
	CS_DATA_Set;															// Ƭѡ����
	
	return Data_Buf;
}

//-----------------------------------------------------------------
// void SPI_Communication_Send_Cmd_Data(u8 Cmd,u32 Send_Data)
//-----------------------------------------------------------------
//
// ��������: �ȷ�������,�ڷ�������
// ��ڲ���: Ҫ���͵�����,Ҫ���͵�����
// �� �� ֵ: ��
// ע������: ��
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
// ��������: �ȷ�������,�ڽ�������
// ��ڲ���: Ҫ���͵�����
// �� �� ֵ: ���յ�������
// ע������: ��
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
