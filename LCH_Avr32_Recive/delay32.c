#include "delay32.h"
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：延时子函数
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
