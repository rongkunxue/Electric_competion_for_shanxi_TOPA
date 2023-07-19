//-----------------------------------------------------------------
// ��������:
// 		 SDRAM��������
// ��    ��: ���ǵ���
// ��ʼ����: 2018-08-04
// �������: 2018-08-04
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0: (2018-08-04)SDRAM��ʼ��
// ���Թ���: ����STM32F429+Cyclone IV����ϵͳ��ƿ����塢LZE_ST_LINK2
// ˵    ��: 
//    
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "sdram.h"
#include "delay.h"
//-----------------------------------------------------------------

SDRAM_HandleTypeDef SDRAM_Handler;   //SDRAM���

//-----------------------------------------------------------------
// void SDRAM_Init(void)
//-----------------------------------------------------------------
//
// ��������: SDRAM��ʼ��
// ��ڲ���: ��
// ���ز���: ��
// ע������: ��
//
//-----------------------------------------------------------------
void SDRAM_Init(void)
{
	FMC_SDRAM_TimingTypeDef SDRAM_Timing;
																									 
	SDRAM_Handler.Instance=FMC_SDRAM_DEVICE;                             // SDRAM�Ĵ�������ַ
	SDRAM_Handler.Init.SDBank=FMC_SDRAM_BANK1;                           // ��һ��SDRAM BANK
	SDRAM_Handler.Init.ColumnBitsNumber=FMC_SDRAM_COLUMN_BITS_NUM_10;    // ������
	SDRAM_Handler.Init.RowBitsNumber=FMC_SDRAM_ROW_BITS_NUM_13;          // ������
	SDRAM_Handler.Init.MemoryDataWidth=FMC_SDRAM_MEM_BUS_WIDTH_16;       // ���ݿ��Ϊ16λ
	SDRAM_Handler.Init.InternalBankNumber=FMC_SDRAM_INTERN_BANKS_NUM_4;  // һ��4��BANK
	SDRAM_Handler.Init.CASLatency=FMC_SDRAM_CAS_LATENCY_3;               // CASΪ3
	SDRAM_Handler.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;// ʧ��д����
	SDRAM_Handler.Init.SDClockPeriod=FMC_SDRAM_CLOCK_PERIOD_2;           // SDRAMʱ��ΪHCLK/2=180M/2=90M=11.1ns
	SDRAM_Handler.Init.ReadBurst=FMC_SDRAM_RBURST_ENABLE;                // ʹ��ͻ��
	SDRAM_Handler.Init.ReadPipeDelay=FMC_SDRAM_RPIPE_DELAY_1;            // ��ͨ����ʱ
	
	SDRAM_Timing.LoadToActiveDelay=2;                                   // ����ģʽ�Ĵ���������ʱ����ӳ�Ϊ2��ʱ������
	SDRAM_Timing.ExitSelfRefreshDelay=8;                                // �˳���ˢ���ӳ�Ϊ8��ʱ������
	SDRAM_Timing.SelfRefreshTime=6;                                     // ��ˢ��ʱ��Ϊ6��ʱ������                                 
	SDRAM_Timing.RowCycleDelay=6;                                       // ��ѭ���ӳ�Ϊ6��ʱ������
	SDRAM_Timing.WriteRecoveryTime=2;                                   // �ָ��ӳ�Ϊ2��ʱ������
	SDRAM_Timing.RPDelay=2;                                             // ��Ԥ����ӳ�Ϊ2��ʱ������
	SDRAM_Timing.RCDDelay=2;                                            // �е����ӳ�Ϊ2��ʱ������
	HAL_SDRAM_Init(&SDRAM_Handler,&SDRAM_Timing);
	
	SDRAM_Initialization_Sequence(&SDRAM_Handler);// ����SDRAM��ʼ������
	
	// ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���),���㷽��:
	// COUNT=SDRAMˢ������/����-20=SDRAMˢ������(us)*SDCLKƵ��(Mhz)/����
	// ����ʹ�õ�SDRAMˢ������Ϊ64ms,SDCLK=180/2=90Mhz,����Ϊ8192(2^13).
	// ����,COUNT=64*1000*90/8192-20=683
	HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler,683);// ����ˢ��Ƶ��

}

//-----------------------------------------------------------------
// void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
//-----------------------------------------------------------------
//
// ��������: ����SDRAM��ʼ������
// ��ڲ���: SDRAM_HandleTypeDef *hsdram��SDRAM���
// ���ز���: ��
// ע������: ��
//
//-----------------------------------------------------------------
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
	u32 temp=0;
	// SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM
	SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0); // ʱ������ʹ��
	delay_us(500);                                  // ������ʱ200us
	SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       // �����д洢��Ԥ���
	SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);// ������ˢ�´��� 
  // ����ģʽ�Ĵ���,SDRAM��bit0~bit2Ϊָ��ͻ�����ʵĳ��ȣ�
	// bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7��bit8Ϊ����ģʽ
	// bit9Ϊָ����дͻ��ģʽ��bit10��bit11λ����λ
	temp=(u32)SDRAM_MODEREG_BURST_LENGTH_1           |		// ����ͻ������:1(������1/2/4/8)
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |		// ����ͻ������:����(����������/����)
              SDRAM_MODEREG_CAS_LATENCY_3           |		// ����CASֵ:3(������2/3)
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   // ���ò���ģʽ:0,��׼ģʽ
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     // ����ͻ��дģʽ:1,�������
  SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);     // ����SDRAM��ģʽ�Ĵ���
}

//-----------------------------------------------------------------
// void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
//-----------------------------------------------------------------
//
// ��������: SDRAM�ײ��������������ã�ʱ��ʹ��
// ��ڲ���: SDRAM_HandleTypeDef *hsdram��SDRAM���
// ���ز���: ��
// ע������: �˺����ᱻHAL_SDRAM_Init()����
//					 SDS_D0  -> PD14				SDS_A0  -> PF0
//					 SDS_D1  -> PD15				SDS_A1  -> PF1
//					 SDS_D2  -> PD0 				SDS_A2  -> PF2
//					 SDS_D3  -> PD1 				SDS_A3  -> PF3
//					 SDS_D4  -> PE7 				SDS_A4  -> PF4
//					 SDS_D5  -> PE8 				SDS_A5  -> PF5
//					 SDS_D6  -> PE9 				SDS_A6  -> PF12
//					 SDS_D7  -> PE10				SDS_A7  -> PF13
//					 SDS_D8  -> PE11				SDS_A8  -> PF14
//					 SDS_D9  -> PE12				SDS_A9  -> PF15
//					 SDS_D10 -> PE13				SDS_A10 -> PG0
//					 SDS_D11 -> PE14				SDS_A11 -> PG1
//					 SDS_D12 -> PE15				SDS_A12 -> PG2
//					 SDS_D13 -> PD8 				
//					 SDS_D14 -> PD9
//					 SDS_D15 -> PD10
//
//					 SDS_SDNWE 	-> PH5			SDS_BA0   -> PG4
//					 SDS_SDNCAS -> PG15			SDS_BA1   -> PG5
//					 SDS_SDNRAS -> PF11			SDS_NBL0  -> PE0
//					 SDS_SDNE0 	-> PH3			SDS_NBL1  -> PE1
//					 SDS_SDCKE0 -> PH2      SDS_SDCLK -> PG8
//
//-----------------------------------------------------------------
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_FMC_CLK_ENABLE();                 // ʹ��FMCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();               // ʹ��GPIODʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();               // ʹ��GPIOEʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();               // ʹ��GPIOFʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();               // ʹ��GPIOGʱ��
		__HAL_RCC_GPIOH_CLK_ENABLE();               // ʹ��GPIOHʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5;  
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          // ���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              // ����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         // ����
    GPIO_Initure.Alternate=GPIO_AF12_FMC;       // ����ΪFMC    
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);         // ��ʼ��PH2,3,5
    
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     		// ��ʼ��PD0,1,8,9,10,14,15
    
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10| GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);     		// ��ʼ��PE0,1,7,8,9,10,11,12,13,14,15
    
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);     		// ��ʼ��PF0,1,2,3,4,5,11,12,13,14,15
    
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     		// ��ʼ��PG0,1,2,4,5,8,15 
}

//-----------------------------------------------------------------
// u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval)
//-----------------------------------------------------------------
//
// ��������: ��SDRAM��������
// ��ڲ���: u8 bankx��0,��BANK5�����SDRAM����ָ��			1,��BANK6�����SDRAM����ָ��
//					 u8 cmd��ָ��(0,����ģʽ/1,ʱ������ʹ��/2,Ԥ������д洢��/3,�Զ�ˢ��/4,����ģʽ�Ĵ���/5,��ˢ��/6,����)
//					 u8 refresh����ˢ�´���
//					 u16 regval��ģʽ�Ĵ����Ķ���
// ���ز���: 0,����;1,ʧ��.
// ע������: ��
//
//-----------------------------------------------------------------
u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval)
{
    u32 target_bank=0;
    FMC_SDRAM_CommandTypeDef Command;
    
    if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;       
    else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;   
    Command.CommandMode=cmd;                // ����
    Command.CommandTarget=target_bank;      // Ŀ��SDRAM�洢����
    Command.AutoRefreshNumber=refresh;      // ��ˢ�´���
    Command.ModeRegisterDefinition=regval;  // Ҫд��ģʽ�Ĵ�����ֵ
    if(HAL_SDRAM_SendCommand(&SDRAM_Handler,&Command,0X1000)==HAL_OK) // ��SDRAM��������
    {
        return 0;  
    }
    else return 1;    
}

//-----------------------------------------------------------------
// void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
//-----------------------------------------------------------------
//
// ��������: ��ָ����ַ(WriteAddr+Bank5_SDRAM_ADDR)��ʼ,����д��n���ֽ�.
// ��ڲ���: u8 *pBuffer���ֽ�ָ��
//					 u32 WriteAddr��Ҫд��ĵ�ַ
//					 u32 n��Ҫд����ֽ���
// ���ز���: 0,����;1,ʧ��.
// ע������: ��
//
//-----------------------------------------------------------------
void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank5_SDRAM_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

//-----------------------------------------------------------------
// void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
//-----------------------------------------------------------------
//
// ��������: ��ָ����ַ(ReadAddr+Bank5_SDRAM_ADDR)��ʼ,��������n���ֽ�.
// ��ڲ���: u8 *pBuffer���ֽ�ָ��
//					 u32 ReadAddr��Ҫ��������ʼ��ַ
//					 u32 n��Ҫд����ֽ���
// ���ز���: 0,����;1,ʧ��.
// ע������: ��
//
//-----------------------------------------------------------------
void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(vu8*)(Bank5_SDRAM_ADDR+ReadAddr);
		ReadAddr++;
	}
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
