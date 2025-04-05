/************************************************
文件：595MC64.h
用途：显示函数头文件
注意：系统时钟12M
************************************************/
#ifndef __595MC64_H__
#define __595MC64_H__


extern void MC595_out(void);
extern void MC595_Writedata(unsigned char data);
extern void Seg7_Led_Update(void);
extern void Seg7_Led_display(unsigned int data);
extern void Seg7_Function_display(unsigned char data);


#endif
