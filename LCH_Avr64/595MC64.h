/************************************************
�ļ���595MC64.h
��;����ʾ����ͷ�ļ�
ע�⣺ϵͳʱ��12M
************************************************/
#ifndef __595MC64_H__
#define __595MC64_H__


extern void MC595_out(void);
extern void MC595_Writedata(unsigned char data);
extern void Seg7_Led_Update(void);
extern void Seg7_Led_display(unsigned int data);
extern void Seg7_Function_display(unsigned char data);


#endif
