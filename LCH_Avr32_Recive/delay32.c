#include "delay32.h"
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ����ʱ�Ӻ���
*****************************************************************/
void delay_us(unsigned int us)
{
     unsigned int i;
	 for(i=0;i<us;i++)
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
     }
}

void delay_ms(unsigned int ms)
{
     unsigned int i1;
	 unsigned int j1;
	 for(i1=0;i1<ms;i1++)
	 {
	    for(j1=0;j1<1000;j1++)
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
        }
     }
}
