#include <avr/io.h>                  //����MEGA64ͷ�ļ�
#include <util/delay.h>
#include <macros.h>
#include <string.h>
#include <stdio.h>
#include <interrupt.h>
#include <wdt.h>
#include "595MC64.h"                 //��ʾͷ�ļ�
#include "delay64.h"                 //��ʱͷ�ļ�
#include "key64.h"                   //��������ͷ�ļ�
#include "uart64.h"                  //uart���ڴ���ͷ�ļ�
#include "EEPROM64.h"                //E2��дͷ�ļ�
#include "EleworkLogic.h"            //���������߼�ͷ�ļ�

#define   ENCODER_A     (PIND&0x08)
#define   ENCODER_B     (PING&0x08)

unsigned char  CommandStatus=0;                        //485ͨѶ����״̬��־
unsigned char  CurrentFloor=0;                         //��ǰ¥��
unsigned char  ReceiveBuf[16];                         //485ͨѶ���ջ�����
unsigned char  SendDatBuf[16];                         //485���ͻ�����
unsigned char  TargetFloor = 0;                        //Ŀ��¥��

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��Board�˿ڳ�ʼ������IO
*****************************************************************/
void Port_Init(void)
{
     DDRA  = 0x00;                          //����IO����PA0-7��X1=X8
	 PORTA = 0xff;
	 DDRC  = 0x00;                          //X9-X14:PC7-2   PC0-1:A8(MS)-A9(JT)                     
	 PORTC = 0xff;
	 	 
	 DDRB   = 0xff;                         //�̵���IO��ʼ�� 1-�򿪼̵�����0-�ر�

	 DDRE  &= 0x0f;                         //����IO PE4-PE7����Ϊ����
     PORTE |= 0xf0;                         //����PE4-PE7����״̬����������
     

	 DDRE  &= 0xfc;                         //485ͨѶ�ܽ�PE0��PE1����Ϊ����
     PORTE &= 0xfc;                         //����������
	 DDRE  |=  (1<<PE2);                    //485���ͽ����л��ܽţ�0 ����  1 ����
     PORTE |=  (1<<PE2);                    //�������
	 
	 DDRE  &= ~(1<<PE3);                    //���ģʽ�ܽ�PE3-PRO����Ϊ����
     PORTE |=  (1<<PE3);                    //������������
	 
	 DDRD  |=  0xf0;                        //HC595:PD4-PD7����Ϊ���  
     DDRD  |=  (1<<PD0);                    //�����λѡ,����Ϊ���
     DDRD  |=  (1<<PD1);
     DDRD  |=  (1<<PD2);
     
	 DDRD  &= ~(1<<PD3);                    //�ⲿ�ж�3 ������A
     PORTE |=  (1<<PE3);                    //����������

	 DDRG  |=  (1<<PG0)|(1<<PG1);
     DDRG  &= ~(1<<PG3);                    //������B
     PORTG |=  (1<<PG3);                    //����������
}

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��Main����
*****************************************************************/	 
int main(void)
{

	 Port_Init();                           //Board�˿ڳ�ʼ������
	 Init_Devices();                        //485ͨѶ�˿����ó�ʼ�� 
     Uart_Init();                           
	 
	 TIMSK = 0x01;                          //8λ��ʱ���ж�0/����ж�ʹ��
	 TCCR0 = 0x07;                          //64��Ƶ
	 TCNT0 = 138;                           //(255-138)*(1024/12M)=9984us=10ms
 
     EIMSK = 0x08;                          //ʹ���ⲿ�ж�3
	 EICRA = 0xC0;                          //�ⲿ�ж�3�����ز����ж�
	 
	 TimerCnt = 0;
     wdt_enable(WDTO_2S);

	 while(1)
     {   
         
		 ScanKey();
		 wdt_reset();                       //ι��

		 /******************************�������ô�����***********************************/
	     if(Cycle_WithOperate == FUNCTION_SET_MODE)                          //��������ģʽ
		 {
		          
				   FuncitonSet();
								 
          }
		 /*******************************�������ô�����******************************/
		 

		 /******************************����״̬������***********************************/
		 if(Cycle_WithOperate == ELEVATOR_CHECK_MODE)                          //���޴���ģʽ
		 {

                  
				  if(((PINC&0x02)==0)&&((PINC&0x01)==0))                                 //PC1-JT PC0-MS ���ߵ�ƽ=�޼�ͣ���������ź�  
				  {
				      switch(Elevator_Run_Mode)
				     {
				         case RUN_UP    : PORTB |=  (1<<PB7)|(1<<PB5);         //��Y1-Y3
				                          PORTB &= ~(1<<PB6);                  //��Y2
						                  break;
                         case RUN_DOWN  : PORTB |=  (1<<PB6)|(1<<PB5);         //��Y2-Y3
				                          PORTB &= ~(1<<PB7);                  //��Y1
						                  break;
                         case INIT_VALUE: PORTB &= ~(1<<PB7);                  //��Y1
						                  PORTB &= ~(1<<PB6);                  //��Y2
										  PORTB &= ~(1<<PB5);                  //��Y3
						                  break;
                         default        : NOP();
						                  break;
                      }
				  }
				  else
				  {     
				         //ֹͣ����-code                                       //������һ��Ϊ�͵�ƽ=ͣ��
						 PORTB &= ~(1<<PB7);                  //��Y1
						 PORTB &= ~(1<<PB6);                  //��Y2
				         PORTB &= ~(1<<PB5);                  //��Y3
                   }
 	              Seg7_Led_display(CurrentFloor);                                  //������ʾ
				   
		 }
	     /******************************����״̬������***********************************/
		
		 
		 
		 
		 /******************************��������������***********************************/
		 if(Cycle_WithOperate == NORMAL_WORK_MODE)                            //��������ģʽ
		 {
                  
                  Seg7_Led_display(CurrentFloor);                             //������ʾ

				  if((PINA&0x40)==0)                                          //�����������λ����
                 {
							LiftStop(); 
								   
                  }
				  
				  if(CurrentFloor == 0)                                       //�����ϵ�󣬵���ƽ��
                  {
				            LiftRunDw();                                      //����������ƽ��
	
                  }   
                 

		 }
	     /******************************��������������***********************************/
		
		
		
		
		/********************485ͨѶ���մ�����***************************************/
         if(CommandStatus)
		 {			  
			       if(ReceiveBuf[1]==0x00)                                //��ַ+���� ¥��
			       {
                            CurrentFloor = ReceiveBuf[3];                 //������͹�����¥��
                            
                    }
					if(ReceiveBuf[1]==0x01)                                //���� - �������¥��
			       {
                            TargetFloor  = ReceiveBuf[3];                 //������͹����Ľ�Ҫ�����¥��
                            
                    }

			       CommandStatus = 0;
		 }   
		 

		 //Seg7_Led_display(CurrentFloor);                                  //������ʾ
        /**********************************************************************************/
       
     }//end while
			        

}//end main


/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��UART0�����ж��Ӻ���
*****************************************************************/	
SIGNAL(USART0_RX_vect)
{
	   unsigned char temp;
	   
	   temp = Uart_ReceiveFrame(ReceiveBuf);
	   if(temp)                                                            //����ɹ�����һ֡����
       CommandStatus = 1;

	   if(ReceiveBuf[1]==0x00)                                            //��ַ+���� ¥��
	  {
              CurrentFloor = ReceiveBuf[3];                 //������͹�����¥��
                            
      }


	   
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��Timer0����ж�  ��1M ��ʱ9920usԼ10ms��
*****************************************************************/	
SIGNAL(TIMER0_OVF_vect)
{
	   TCNT0 = 100;
	   
	   SysTimebase++;
	   if(TimerCnt>65000)
	          TimerCnt=0;

       TimerCnt++;                 //���ת�ٲɼ�����
	   if(TimerCnt>5)
	   {
	          TimerCnt=0;
			  EncoderCntHz = 0;    //��������
       }
      
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ���ⲿ�ж�3   �������ж�
*****************************************************************/	
SIGNAL(INT3_vect)
{
	  if(ENCODER_B)                 //B��ߵ�ƽ  ��ת
	  {
	          PORTG &= ~(1<<PG0);    //��תָʾ��RUN
			  PORTG |=  (1<<PG1);
	  }
	  else
	  {
	          PORTG &= ~(1<<PG1);    //��תָʾ��RSL
			  PORTG |=  (1<<PG0);
	   }	  
	  EncoderCntHz++;  
      
}






	   

