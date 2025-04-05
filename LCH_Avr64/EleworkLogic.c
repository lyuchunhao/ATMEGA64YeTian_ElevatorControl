#include <avr/io.h>
#include "EleworkLogic.h"
#include "delay64.h"

//ȫ�ֱ���
unsigned int    TimerCnt = 0;                                //��ʱ�ж�0ʱ����10ms��
unsigned long   EncoderCntHz=0;                              //Tʱ���ڵ�������
//�ֲ�����
unsigned int    EncoderN=0;                                  //ת��
unsigned long   EncoderCntHzCopy =0;
unsigned long   EncoderCntHzCopy1=0;
unsigned long   EncoderCntHzCopy2=0;

/***************************************************************
Copyright(C), 2014.01.13  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��MotorSpeed����
*****************************************************************/
unsigned int MotorSpeed()
{
     unsigned long  temp=0;
	 	 
	 if(TimerCnt==0)
            EncoderCntHzCopy1  = EncoderCntHz;
     if(TimerCnt==2)
     {
            EncoderCntHzCopy2 =  EncoderCntHz;
            EncoderCntHzCopy = EncoderCntHzCopy2-EncoderCntHzCopy1;
        
	        //EncoderN = (3000*EncoderCntHzCopy)/1024;       //ת�٣�EncoderN=60M/1024*T (TΪ�ɼ�ʱ��20ms  MΪ�ɼ��������� 
            temp = 3000*EncoderCntHzCopy;
            EncoderN = (unsigned int)(temp/1024);
     }
     //Seg7_Led_display(EncoderN);
	 return EncoderN;
}

void LiftRunUp(void)
{
     NOP();
	 PORTB |=  (1<<PB7)|(1<<PB5);         //��Y1-Y3
	 PORTB &= ~(1<<PB6);                  //��Y2
}

void LiftRunDw(void)
{
     NOP();
	 PORTB |=  (1<<PB6)|(1<<PB5);         //��Y2-Y3
	 PORTB &= ~(1<<PB7);                  //��Y1
}
void LiftStop(void)
{
     NOP();
	 PORTB &= ~(1<<PB7);                  //��Y1
	 PORTB &= ~(1<<PB6);                  //��Y2
	 PORTB &= ~(1<<PB5);                  //��Y3
}
