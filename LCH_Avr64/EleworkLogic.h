/************************************************
�ļ���EleworkLogic.h
��;�����������߼�����ͷ�ļ�
ע�⣺ϵͳʱ��12M
************************************************/
#ifndef __ELEWORKLOGIC_H__
#define __ELEWORKLOGIC_H__

extern unsigned int    TimerCnt;                                   //��ʱ�ж�0ʱ����10ms��
extern unsigned long   EncoderCntHz;                              //Tʱ���ڵ�������


extern unsigned int MotorSpeed();
extern void LiftRunUp(void);
extern void LiftRunDw(void);
extern void LiftStop(void);


#endif
