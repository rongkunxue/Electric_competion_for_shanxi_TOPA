#ifndef __DAW_H
#define __DAW_H	
#include "stm32f429_winner.h"

extern float Am_Fre[10][10];
extern void MY_Rectangle (void);
extern void drawstring_screen(void );
extern float findmax(float arry[],int n);
extern float  findmin(float arry[],int n);
extern float findpingjun(float arry[1000],int n);

#endif

