/************************************************
文件：delay.h
用途：延时函数头文件
注意：系统时钟12M
************************************************/
#ifndef __DELAY64_H__
#define __DELAY64_H__

#define NOP()     asm("nop");

extern void delay_1us(void);
extern void delay_xus(unsigned int x);
extern void delay_1ms(void);
extern void delay_xms(unsigned int x);



#endif
