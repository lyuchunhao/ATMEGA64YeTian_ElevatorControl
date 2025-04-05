#include "delay64.h"

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：延时子函数 1us 
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
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：延时子函数 xus  
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
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：延时子函数 1ms
*****************************************************************/
void delay_1ms(void)
{

	   delay_xus(1000);
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：延时子函数 xms
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
