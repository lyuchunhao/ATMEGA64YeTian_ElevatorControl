/************************************************
�ļ���delay.h
��;����ʱ����ͷ�ļ�
ע�⣺ϵͳʱ��8M
************************************************/
#ifndef __595MC32_H__
#define __595MC32_H__


extern void MC595_out(void);
extern void MC595_Writedata(unsigned char data);
extern unsigned char HC165Date_Read(void);
extern void Buzzer(void);
extern void DisplayAndUout(unsigned char Data,unsigned char Out);

#endif
