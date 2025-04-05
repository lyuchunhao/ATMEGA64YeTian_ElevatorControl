#include <avr/io.h>
#include "EleworkLogic.h"
#include "delay64.h"

//全局变量
unsigned int    TimerCnt = 0;                                //定时中断0时基（10ms）
unsigned long   EncoderCntHz=0;                              //T时间内的脉冲数
//局部变量
unsigned int    EncoderN=0;                                  //转速
unsigned long   EncoderCntHzCopy =0;
unsigned long   EncoderCntHzCopy1=0;
unsigned long   EncoderCntHzCopy2=0;

/***************************************************************
Copyright(C), 2014.01.13  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：MotorSpeed函数
*****************************************************************/
unsigned int MotorSpeed()
{
     unsigned long  temp=0;
	 	 
	 if(TimerCnt==0)
            EncoderCntHzCopy1  = EncoderCntHz;
     if(TimerCnt==2)
     {
            EncoderCntHzCopy2 =  EncoderCntHz;
            EncoderCntHzCopy = EncoderCntHzCopy2-EncoderCntHzCopy1;
        
	        //EncoderN = (3000*EncoderCntHzCopy)/1024;       //转速：EncoderN=60M/1024*T (T为采集时间20ms  M为采集脉冲数） 
            temp = 3000*EncoderCntHzCopy;
            EncoderN = (unsigned int)(temp/1024);
     }
     //Seg7_Led_display(EncoderN);
	 return EncoderN;
}

void LiftRunUp(void)
{
     NOP();
	 PORTB |=  (1<<PB7)|(1<<PB5);         //开Y1-Y3
	 PORTB &= ~(1<<PB6);                  //关Y2
}

void LiftRunDw(void)
{
     NOP();
	 PORTB |=  (1<<PB6)|(1<<PB5);         //开Y2-Y3
	 PORTB &= ~(1<<PB7);                  //关Y1
}
void LiftStop(void)
{
     NOP();
	 PORTB &= ~(1<<PB7);                  //关Y1
	 PORTB &= ~(1<<PB6);                  //关Y2
	 PORTB &= ~(1<<PB5);                  //关Y3
}
