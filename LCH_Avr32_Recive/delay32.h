/************************************************
�ļ���delay.h
��;����ʱ����ͷ�ļ�
ע�⣺ϵͳʱ��8M
************************************************/
#ifndef __DELAY32_H__
#define __DELAY32_H__

#define NOP()     asm("nop");

void delay_us(unsigned int us);
void delay_ms(unsigned int ms);



#endif
