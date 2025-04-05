/************************************************
文件：delay.h
用途：延时函数头文件
注意：系统时钟8M
************************************************/
#ifndef __DELAY32_H__
#define __DELAY32_H__

#define NOP()     asm("nop");

void delay_us(unsigned int us);
void delay_ms(unsigned int ms);



#endif
