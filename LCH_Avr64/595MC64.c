#include <avr/io.h> //包含MEGA64头文件
#include "595MC64.h"
#include "delay64.h"

#define WEI1()   {PORTD |= 0x07;PORTD &= ~(1<<PD2);}         //PD2=0
#define WEI2()   {PORTD |= 0x07;PORTD &= ~(1<<PD1);}         //PD1=0
#define WEI3()   {PORTD |= 0x07;PORTD &= ~(1<<PD0);}         //PD0=0

volatile unsigned char Seg7_Led_Buf[3]={16,16,16};                                //数码管要显示的数值
const unsigned char Display[]={0x28,0xee,0x32,0xa2,0xe4,0xa1,0x21,0xea,           //595-共阴数码管编码
	                           0x20,0xa0,0x60,0x25,0x39,0x26,0x31,0x71,0xff};
const unsigned char Discode[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,           //共阴数码管编码
                               0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：PD4-SDI
                 PD5-OE
                 PD6-存储RCK
                 PD7-移位RCK
*****************************************************************/
void MC595_Writedata(unsigned char data)
{
    unsigned char i;
  
    PORTD &= ~(1<<PD5);           //PD5-13   595输出使能端--低电平使能     
    for(i=0;i<8;i++)
    {
        NOP();
		PORTD &= ~(1<<PD7);       //PD7-11   595移位寄存器--低电平
        NOP();
        if((data&0x80)!=0)
        {
            PORTD |= (1<<PD4);   //PD4-14   串行数据输入端口--1
			NOP();
        }
        else
        {
            PORTD &= ~(1<<PD4);   //PD4-14   串行数据输入端口--0
			NOP();
        }

        data <<=1;                //数据data右移一位

        PORTD |= (1<<PD7);       //PD7-11   595移位寄存器--高电平
        NOP();
    }


}
/***************************************************************
Copyright(C), 2013.08.18  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
管脚说明       ：PC0-存储RCK

函数功能       ：完成并出数据功能
*****************************************************************/
void MC595_out(void)
{
     PORTD &= ~(1<<PD6);           //PC0-12   595存储RCK-- 低电平
	 NOP();
	 NOP();
     PORTD |= (1<<PD6);            //PC0-12   595存储RCK-- 高电平
}
/*************************************************************************
** 函数名称:HC595刷新显示
** 功能描述:
** 输　入:
** 输出	 :
** 全局变量:
** 调用模块: 
** 说明：
** 注意：
**************************************************************************/
void Seg7_Led_Update(void)
{

	 MC595_Writedata(Display[Seg7_Led_Buf[0]]);      //最低位
	 MC595_out();
	 WEI1();
     delay_xms(5);
	 PORTD |= (1<<PD2);
		      
	 MC595_Writedata(Display[Seg7_Led_Buf[1]]);
	 MC595_out();
	 WEI2();
     delay_xms(5);
	 PORTD |= (1<<PD1);
		     
	 MC595_Writedata(Display[Seg7_Led_Buf[2]]);      //最高位
	 MC595_out();
	 WEI3();
     delay_xms(5);
	 PORTD |= (1<<PD0);
}
/*************************************************************************
** 函数名称:Hc595显示整形数据
** 功能描述:
** 输　入:0~9999
** 输出	 :
** 全局变量:
** 调用模块: 
** 说明：
** 注意：
**************************************************************************/
void Seg7_Led_display(unsigned int data)
{
     if(data>999) //错误处理,超出显示范围则全亮
 	{
	     MC595_Writedata(Display[15]);
	     PORTD &= 0xf8;               //数码管位选全部打开
	 }
     else if(data>99)
 	{
	 Seg7_Led_Buf[2]= data/100;
 	 Seg7_Led_Buf[1]=(data%100)/10;
 	 Seg7_Led_Buf[0]= data%10;
	 Seg7_Led_Update();
	}
     else if(data>9)
 	{
	 Seg7_Led_Buf[2]=16;																//16什么意思？
 	 Seg7_Led_Buf[1]= data/10;
 	 Seg7_Led_Buf[0]= data%10;
	 Seg7_Led_Update();
	}
     else
 	{
 	 Seg7_Led_Buf[2]=16;
 	 Seg7_Led_Buf[1]=16;
 	 Seg7_Led_Buf[0]=data%10;
	 Seg7_Led_Update();
	}
}
//功能码显示
void Seg7_Function_display(unsigned char data)
{
     if(data>99) //错误处理,超出显示范围则全亮
 	{
	     MC595_Writedata(Display[15]);
	     PORTD &= 0xf8;               //数码管位选全部打开
	 }
     else if(data>9)
 	{
	 Seg7_Led_Buf[2]= 15;
 	 Seg7_Led_Buf[1]= data/10;
 	 Seg7_Led_Buf[0]= data%10;
	 Seg7_Led_Update();
	}
     else
 	{
 	 Seg7_Led_Buf[2]=15;
 	 Seg7_Led_Buf[1]=0;
 	 Seg7_Led_Buf[0]=data%10;
	 Seg7_Led_Update();
	}
}
