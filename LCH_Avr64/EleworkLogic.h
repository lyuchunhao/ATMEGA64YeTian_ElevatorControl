/************************************************
文件：EleworkLogic.h
用途：电梯运行逻辑函数头文件
注意：系统时钟12M
************************************************/
#ifndef __ELEWORKLOGIC_H__
#define __ELEWORKLOGIC_H__

extern unsigned int    TimerCnt;                                   //定时中断0时基（10ms）
extern unsigned long   EncoderCntHz;                              //T时间内的脉冲数


extern unsigned int MotorSpeed();
extern void LiftRunUp(void);
extern void LiftRunDw(void);
extern void LiftStop(void);


#endif
