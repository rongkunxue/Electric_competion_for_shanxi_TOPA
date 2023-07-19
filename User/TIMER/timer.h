//-----------------------------------------------------------------
// 程序描述:
// 		 定时器中断驱动程序头文件
// 作    者: 凌智电子
// 开始日期: 2018-08-04
// 完成日期: 2018-08-04
// 修改日期: 
// 当前版本: V1.0
// 历史版本:
//  - V1.0: (2018-08-04)定时器中断初始化，定时器中断服务函数
// 调试工具: 凌智STM32F429+Cyclone IV电子系统设计开发板、LZE_ST_LINK2
// 说    明: 
//    
//-----------------------------------------------------------------
#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f429_winner.h"
//-----------------------------------------------------------------
// 声明
//-----------------------------------------------------------------
extern TIM_HandleTypeDef TIM3_Handler;      // 定时器句柄 
//-----------------------------------------------------------------
// 外部函数声明
//-----------------------------------------------------------------
extern void TIM3_Init(u16 arr,u16 psc);
#endif
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
