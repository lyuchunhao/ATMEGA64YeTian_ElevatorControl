/************************************************
�ļ���delay.h
��;����ʱ����ͷ�ļ�
ע�⣺ϵͳʱ��12M
************************************************/
#ifndef __DELAY64_H__
#define __DELAY64_H__

#define NOP()     asm("nop");

extern void delay_1us(void);
extern void delay_xus(unsigned int x);
extern void delay_1ms(void);
extern void delay_xms(unsigned int x);



#endif
