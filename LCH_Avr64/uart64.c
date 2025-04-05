#include <avr/io.h>
#include "uart64.h"
#include "delay64.h"
#include <interrupt.h>

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��UART0��ʼ������
*****************************************************************/
void Uart_Init(void)
{
	   UCSR0B = 0x00;
	   UCSR0A = 0x00;                 //ָʾ���״̬
	   UCSR0C = 0x06;                 //��UCSZ1��UCSZ0��Bit2-1 �ַ�����8λ�����UCSRB�е�UCSZ2=0��Bit2��
	   UBRR0L = 0x4D;                 //12M���񣬲�����=9600bps
	   UBRR0H = 0x00;
	   UCSR0B = 0x98;                 //��RXCIE��Bit7=1�����ж�ʹ�ܣ���RXEN��Bit4=1����ʹ�ܣ���TXEN��Bit3=1����ʹ��
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��ϵͳ��ʼ������
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
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           �����ڷ���һ���ֽ�
*****************************************************************/
void Uart_SendData(unsigned char data)
{
	   while(!(UCSR0A&0x20));        //UDRE=1 ���ݼĴ����ѿ�
	   UDR0 = data;
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           �����ڷ���һ֡����
*****************************************************************/
void Uart_SendFrame(unsigned char *s)
{
     unsigned char i=0,j;
     unsigned char crc=0;
     
     PORTE &= ~(1<<PE2);                     //�л�Ϊ����״̬
     delay_xms(1);
     
     Uart_SendData(0xfe);                   //����2��ǰ���ֽ�
     Uart_SendData(0xfe);
     Uart_SendData(0x68);                   //����֡ͷ
     crc+=0x68;
     //Uart_SendData(0xa0);                   //���͵�ַ
     Uart_SendData(*s);
     crc+=*s;
     //Uart_SendData(0x01);                   //����֡����  01�������ʾ
     Uart_SendData(*(s+1));
     crc+=*(s+1);
     //Uart_SendData(0x01);                   //�������ݳ���
     j = *(s+2);
     Uart_SendData(j);
     crc+=*(s+2);
     //Uart_SendData(0x05);                   //�������� 0x05
     for(i=0;i<=(j-1);i++)
     {
          Uart_SendData(*(s+3+i));
		  crc+=*(s+3+i);
     }
     
     Uart_SendData(crc);                      //����У���ֽ�
     Uart_SendData(0x16);                      //����֡β
     delay_xms(5);
     PORTE |= (1<<PE2);                     //�л�Ϊ����״̬
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           �����ڽ���һ������
*****************************************************************/     
unsigned char Uart_ReceiveData(void)
{
	   while(!(UCSR0A&0x80));        //RXC=1 �ȴ���������
	   return UDR0;                  //�ӻ�������ȡ����������
}   
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           �����ڽ���һ֡����
*****************************************************************/ 
unsigned char Uart_ReceiveFrame(unsigned char *s) 
{
     unsigned char Serial_data;
     unsigned char i,j;
     unsigned char crc=0;
     
     Serial_data = UDR0;      //���յ�һ���ֽ�
     while(Serial_data!=0xfe)
     return 0;
     
     do
     Serial_data = Uart_ReceiveData();  //��ǰ���ֽڣ�������ѯ
     while(Serial_data == 0xfe);         //��ǰ���ֽ�ô��
     
     if(Serial_data!=0x68)              //��֡ͷô��
     return 0; 
     crc+=0x68;
     *s     = Uart_ReceiveData();            //���յ�ַ
	 crc+=*s;
     *(s+1) = Uart_ReceiveData();            //��������
     crc+=*(s+1);
     j = *(s+2) = Uart_ReceiveData();        //�������ݳ���
	 crc+=*(s+2);
     for(i=0;i<=(j-1);i++)                   //����j������
     {
          *(s+3+i) = Uart_ReceiveData();
		  crc+=*(s+3+i); 
     }
     
     if(crc!=Uart_ReceiveData())             //У����ȷô��
     return 0;
     if(Uart_ReceiveData()!=0x16)            //��֡βô��
     return 0;
     return 1;                               //֡���ճɹ�
}
