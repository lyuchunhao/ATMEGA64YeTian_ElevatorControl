/************************************************
文件：delay.h
用途：延时函数头文件
注意：系统时钟8M
************************************************/
#ifndef __595MC32_H__
#define __595MC32_H__


extern void MC595_out(void);
extern void MC595_Writedata(unsigned char data);
extern unsigned char HC165Date_Read(void);
extern void Buzzer(void);
extern void DisplayAndUout(unsigned char Data,unsigned char Out);

#endif
