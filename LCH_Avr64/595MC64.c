#include <avr/io.h> //����MEGA64ͷ�ļ�
#include "595MC64.h"
#include "delay64.h"

#define WEI1()   {PORTD |= 0x07;PORTD &= ~(1<<PD2);}         //PD2=0
#define WEI2()   {PORTD |= 0x07;PORTD &= ~(1<<PD1);}         //PD1=0
#define WEI3()   {PORTD |= 0x07;PORTD &= ~(1<<PD0);}         //PD0=0

volatile unsigned char Seg7_Led_Buf[3]={16,16,16};                                //�����Ҫ��ʾ����ֵ
const unsigned char Display[]={0x28,0xee,0x32,0xa2,0xe4,0xa1,0x21,0xea,           //595-��������ܱ���
	                           0x20,0xa0,0x60,0x25,0x39,0x26,0x31,0x71,0xff};
const unsigned char Discode[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,           //��������ܱ���
                               0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��PD4-SDI
                 PD5-OE
                 PD6-�洢RCK
                 PD7-��λRCK
*****************************************************************/
void MC595_Writedata(unsigned char data)
{
    unsigned char i;
  
    PORTD &= ~(1<<PD5);           //PD5-13   595���ʹ�ܶ�--�͵�ƽʹ��     
    for(i=0;i<8;i++)
    {
        NOP();
		PORTD &= ~(1<<PD7);       //PD7-11   595��λ�Ĵ���--�͵�ƽ
        NOP();
        if((data&0x80)!=0)
        {
            PORTD |= (1<<PD4);   //PD4-14   ������������˿�--1
			NOP();
        }
        else
        {
            PORTD &= ~(1<<PD4);   //PD4-14   ������������˿�--0
			NOP();
        }

        data <<=1;                //����data����һλ

        PORTD |= (1<<PD7);       //PD7-11   595��λ�Ĵ���--�ߵ�ƽ
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
     PORTD &= ~(1<<PD6);           //PC0-12   595�洢RCK-- �͵�ƽ
	 NOP();
	 NOP();
     PORTD |= (1<<PD6);            //PC0-12   595�洢RCK-- �ߵ�ƽ
}
/*************************************************************************
** ��������:HC595ˢ����ʾ
** ��������:
** �䡡��:
** ���	 :
** ȫ�ֱ���:
** ����ģ��: 
** ˵����
** ע�⣺
**************************************************************************/
void Seg7_Led_Update(void)
{

	 MC595_Writedata(Display[Seg7_Led_Buf[0]]);      //���λ
	 MC595_out();
	 WEI1();
     delay_xms(5);
	 PORTD |= (1<<PD2);
		      
	 MC595_Writedata(Display[Seg7_Led_Buf[1]]);
	 MC595_out();
	 WEI2();
     delay_xms(5);
	 PORTD |= (1<<PD1);
		     
	 MC595_Writedata(Display[Seg7_Led_Buf[2]]);      //���λ
	 MC595_out();
	 WEI3();
     delay_xms(5);
	 PORTD |= (1<<PD0);
}
/*************************************************************************
** ��������:Hc595��ʾ��������
** ��������:
** �䡡��:0~9999
** ���	 :
** ȫ�ֱ���:
** ����ģ��: 
** ˵����
** ע�⣺
**************************************************************************/
void Seg7_Led_display(unsigned int data)
{
     if(data>999) //������,������ʾ��Χ��ȫ��
 	{
	     MC595_Writedata(Display[15]);
	     PORTD &= 0xf8;               //�����λѡȫ����
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
	 Seg7_Led_Buf[2]=16;																//16ʲô��˼��
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
//��������ʾ
void Seg7_Function_display(unsigned char data)
{
     if(data>99) //������,������ʾ��Χ��ȫ��
 	{
	     MC595_Writedata(Display[15]);
	     PORTD &= 0xf8;               //�����λѡȫ����
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
