#include "delay64.h"

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ����ʱ�Ӻ��� 1us 
*****************************************************************/
void delay_1us(void)
{

	    NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ����ʱ�Ӻ��� xus  
*****************************************************************/
void delay_xus(unsigned int x)
{
      unsigned int i;
	  for(i=0;i<x;i++)
	  {
	      delay_1us();
      }
}

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ����ʱ�Ӻ��� 1ms
*****************************************************************/
void delay_1ms(void)
{

	   delay_xus(1000);
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ����ʱ�Ӻ��� xms
*****************************************************************/
/*
void delay_xms(unsigned int x)
{
      unsigned int i;
	  for(i=0;i<x;i++)
	  {
	      delay_1ms();
      }
}
*/
void delay_xms(unsigned int x)
{
     unsigned int i1;
     unsigned int a1;
	 for(i1=0;i1<x;i1++)
	 {
	    //for(j1=0;j1<129;j1++)
		//{
		   //for(j2=0;j2<45;j2++);
        //}
           //delay_1ms();
          for(a1=0;a1<1000;a1++)
		  {
		         NOP();
		         NOP();
	           	 NOP();
		         NOP();
		         NOP();
		         NOP();
		         NOP();
		         NOP();
		         NOP();
		         NOP();
		         NOP();
		         //NOP();
		  }
     }
}
