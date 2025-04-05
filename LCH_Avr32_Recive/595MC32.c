#include <avr/io.h>
#include "595MC32.h"
#include "delay32.h"

const unsigned char Display[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07, 
                               0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};  

const unsigned char ULN2803[]={0x00,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}; //OUT1-OUT8

/***************************************************************
Copyright(C), 2013.08.18  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
管脚说明       ：PC6-SDI
                 PC1-移位RCK
函数功能       ：完成一个8字位数据的输入
*****************************************************************/
void MC595_Writedata(unsigned char data)  
{
     unsigned char i;
	 //unsigned char data;

	 //data = Display[num];

	 for(i=0;i<8;i++)
	 {
         NOP();
		 PORTC &= ~(1<<PC1);         //PC1-11   595移位寄存器--低电平
		 NOP();
		 if((data&0x80)!=0)
        {
            NOP();
			PORTC |= (1<<PC6);       //PC6-14   串行数据输入端口--1
        }
        else
        {
            NOP();
			PORTC &= ~(1<<PC6);      //PC6-14   串行数据输入端口--0
        }

        data <<=1;                   //数据data右移一位

        PORTC |= (1<<PC1);           //PC1-11   595移位寄存器--高电平
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
     PORTC &= ~(1<<PC0);           //PC0-12   595存储RCK-- 低电平
	 NOP();
	 NOP();
     PORTC |= (1<<PC0);            //PC0-12   595存储RCK-- 高电平
}	
/***************************************************************
Copyright(C), 2013.08.25  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
管脚说明       ：PD5----CLK，上升沿移位
                 PD6----SH/LD,“0”并行数据Load，"1"允许移位
                 PD7----CLK INH “0”允许移位
                 PD3----QH，串行输出

函数功能       ：完成并行输入串行输出功能
*****************************************************************/
unsigned char HC165Date_Read(void)
{
    unsigned char i;
    unsigned char data=0;

    PORTD &= ~(1<<PD6);              //SH/LD,“0”并行数据Load
    NOP();
    NOP();
    NOP();
    PORTD |=  (1<<PD6);              //SH/LD,"1"允许移位
    NOP();
    NOP();
    NOP();
    PORTD &= ~(1<<PD7);              //CLK INH “0”允许移位
    for(i=0;i<8;i++)
    {
         
		 data<<=1;                   //数据data左移一位

		 PORTD &= ~(1<<PD5);         //CLK，上升沿移位
         NOP();
         NOP();

         if((PIND&0x08)!=0)           //先低位A---后高位H
         {
         	   data |=  1;
         }
         else
         {   
         	   data &= 0xfe;
         }

		 PORTD |=  (1<<PD5);
         NOP();
         NOP();
         PORTD &= ~(1<<PD5); 
    }
	NOP();
	NOP();
    PORTD |=  (1<<PD7);              //CLK INH “1”禁止移位
    return data;
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：蜂鸣器
*****************************************************************/	
void Buzzer(void)
{

	  PORTB |=  (1<<PB4);               //蜂鸣器
	  delay_ms(150);
      PORTB &= ~(1<<PB4);               //蜂鸣器
	  delay_ms(70);
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega32
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：数码管显示+ULN2803输出
*****************************************************************/
void DisplayAndUout(unsigned char Data,unsigned char Out)
{

	 NOP();
	 MC595_Writedata(Display[Data]);
	 MC595_Writedata(ULN2803[Out]);
	 MC595_out();
	 NOP();
}
