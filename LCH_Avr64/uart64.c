#include <avr/io.h>
#include "uart64.h"
#include "delay64.h"
#include <interrupt.h>

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：UART0初始化函数
*****************************************************************/
void Uart_Init(void)
{
	   UCSR0B = 0x00;
	   UCSR0A = 0x00;                 //指示相关状态
	   UCSR0C = 0x06;                 //（UCSZ1：UCSZ0）Bit2-1 字符长度8位，结合UCSRB中的UCSZ2=0（Bit2）
	   UBRR0L = 0x4D;                 //12M晶振，波特率=9600bps
	   UBRR0H = 0x00;
	   UCSR0B = 0x98;                 //（RXCIE）Bit7=1接收中断使能，（RXEN）Bit4=1接收使能，（TXEN）Bit3=1发送使能
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：系统初始化函数
*****************************************************************/
void Init_Devices(void)
{
       cli();
	   XDIV  = 0x00;
	   XMCRA = 0x00;
	   MCUCR = 0x00;
	   EICRA = 0x00;
	   EICRB = 0x00;
	   EIMSK = 0x00;
	   TIMSK = 0x00;
	   ETIMSK= 0x00;
	   sei();
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：串口发送一个字节
*****************************************************************/
void Uart_SendData(unsigned char data)
{
	   while(!(UCSR0A&0x20));        //UDRE=1 数据寄存器已空
	   UDR0 = data;
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：串口发送一帧数据
*****************************************************************/
void Uart_SendFrame(unsigned char *s)
{
     unsigned char i=0,j;
     unsigned char crc=0;
     
     PORTE &= ~(1<<PE2);                     //切换为发送状态
     delay_xms(1);
     
     Uart_SendData(0xfe);                   //发送2个前导字节
     Uart_SendData(0xfe);
     Uart_SendData(0x68);                   //发送帧头
     crc+=0x68;
     //Uart_SendData(0xa0);                   //发送地址
     Uart_SendData(*s);
     crc+=*s;
     //Uart_SendData(0x01);                   //发送帧命令  01数码管显示
     Uart_SendData(*(s+1));
     crc+=*(s+1);
     //Uart_SendData(0x01);                   //发送数据长度
     j = *(s+2);
     Uart_SendData(j);
     crc+=*(s+2);
     //Uart_SendData(0x05);                   //发送数据 0x05
     for(i=0;i<=(j-1);i++)
     {
          Uart_SendData(*(s+3+i));
		  crc+=*(s+3+i);
     }
     
     Uart_SendData(crc);                      //发送校验字节
     Uart_SendData(0x16);                      //发送帧尾
     delay_xms(5);
     PORTE |= (1<<PE2);                     //切换为接收状态
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：串口接收一个数据
*****************************************************************/     
unsigned char Uart_ReceiveData(void)
{
	   while(!(UCSR0A&0x80));        //RXC=1 等待接收数据
	   return UDR0;                  //从缓冲区获取并返回数据
}   
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：串口接收一帧数据
*****************************************************************/ 
unsigned char Uart_ReceiveFrame(unsigned char *s) 
{
     unsigned char Serial_data;
     unsigned char i,j;
     unsigned char crc=0;
     
     Serial_data = UDR0;      //接收第一个字节
     while(Serial_data!=0xfe)
     return 0;
     
     do
     Serial_data = Uart_ReceiveData();  //是前导字节，继续查询
     while(Serial_data == 0xfe);         //是前导字节么？
     
     if(Serial_data!=0x68)              //是帧头么？
     return 0; 
     crc+=0x68;
     *s     = Uart_ReceiveData();            //接收地址
	 crc+=*s;
     *(s+1) = Uart_ReceiveData();            //接收命令
     crc+=*(s+1);
     j = *(s+2) = Uart_ReceiveData();        //接收数据长度
	 crc+=*(s+2);
     for(i=0;i<=(j-1);i++)                   //接收j个数据
     {
          *(s+3+i) = Uart_ReceiveData();
		  crc+=*(s+3+i); 
     }
     
     if(crc!=Uart_ReceiveData())             //校验正确么？
     return 0;
     if(Uart_ReceiveData()!=0x16)            //是帧尾么？
     return 0;
     return 1;                               //帧接收成功
}
