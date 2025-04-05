#include <avr/io.h>
#include "595MC32.h"
#include "delay32.h"

const unsigned char Display[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07, 
                               0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};  

const unsigned char ULN2803[]={0x00,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}; //OUT1-OUT8

/***************************************************************
Copyright(C), 2013.08.18  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
�ܽ�˵��       ��PC6-SDI
                 PC1-��λRCK
��������       �����һ��8��λ���ݵ�����
*****************************************************************/
void MC595_Writedata(unsigned char data)  
{
     unsigned char i;
	 //unsigned char data;

	 //data = Display[num];

	 for(i=0;i<8;i++)
	 {
         NOP();
		 PORTC &= ~(1<<PC1);         //PC1-11   595��λ�Ĵ���--�͵�ƽ
		 NOP();
		 if((data&0x80)!=0)
        {
            NOP();
			PORTC |= (1<<PC6);       //PC6-14   ������������˿�--1
        }
        else
        {
            NOP();
			PORTC &= ~(1<<PC6);      //PC6-14   ������������˿�--0
        }

        data <<=1;                   //����data����һλ

        PORTC |= (1<<PC1);           //PC1-11   595��λ�Ĵ���--�ߵ�ƽ
        NOP();
    }

}	     
/***************************************************************
Copyright(C), 2013.08.18  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
�ܽ�˵��       ��PC0-�洢RCK

��������       ����ɲ������ݹ���
*****************************************************************/
void MC595_out(void)
{
     PORTC &= ~(1<<PC0);           //PC0-12   595�洢RCK-- �͵�ƽ
	 NOP();
	 NOP();
     PORTC |= (1<<PC0);            //PC0-12   595�洢RCK-- �ߵ�ƽ
}	
/***************************************************************
Copyright(C), 2013.08.25  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
�ܽ�˵��       ��PD5----CLK����������λ
                 PD6----SH/LD,��0����������Load��"1"������λ
                 PD7----CLK INH ��0��������λ
                 PD3----QH���������

��������       ����ɲ������봮���������
*****************************************************************/
unsigned char HC165Date_Read(void)
{
    unsigned char i;
    unsigned char data=0;

    PORTD &= ~(1<<PD6);              //SH/LD,��0����������Load
    NOP();
    NOP();
    NOP();
    PORTD |=  (1<<PD6);              //SH/LD,"1"������λ
    NOP();
    NOP();
    NOP();
    PORTD &= ~(1<<PD7);              //CLK INH ��0��������λ
    for(i=0;i<8;i++)
    {
         
		 data<<=1;                   //����data����һλ

		 PORTD &= ~(1<<PD5);         //CLK����������λ
         NOP();
         NOP();

         if((PIND&0x08)!=0)           //�ȵ�λA---���λH
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
    PORTD |=  (1<<PD7);              //CLK INH ��1����ֹ��λ
    return data;
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��������
*****************************************************************/	
void Buzzer(void)
{

	  PORTB |=  (1<<PB4);               //������
	  delay_ms(150);
      PORTB &= ~(1<<PB4);               //������
	  delay_ms(70);
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega32
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ���������ʾ+ULN2803���
*****************************************************************/
void DisplayAndUout(unsigned char Data,unsigned char Out)
{

	 NOP();
	 MC595_Writedata(Display[Data]);
	 MC595_Writedata(ULN2803[Out]);
	 MC595_out();
	 NOP();
}
