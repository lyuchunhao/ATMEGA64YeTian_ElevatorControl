#include <avr/io.h> //����MEGA32ͷ�ļ�
#include <util/delay.h>
#include <macros.h>
#include <string.h>
#include <stdio.h>
#include <interrupt.h>
#include "uart32.h"
#include "delay32.h"
#include "595MC32.h"
#include "EEPROM32.h"

#define   LED_UP()  {PORTB |= (1<<PB1); PORTB &= ~(1<<PB0);}    //LED-X ��������
#define   LED_DW()  {PORTB |= (1<<PB0); PORTB &= ~(1<<PB1);}    //LED-X ��������
#define   LED_QM()  {PORTB &= ~(1<<PB0);PORTB &= ~(1<<PB1);}    //LED-X ����ȫ��
#define   LED_JT()   PORTB |= (1<<PB5)                          //LED-X ��ͣ
#define   LED_KM()   PORTB |= (1<<PB6)                          //LED-X ����
#define   SIGNAL_ES  PINA&0x02                                  //����崮�м�ͣ �޶����ߵ�ƽ 1
#define   SIGNAL_PKS PINA&0x04                                  //�����������   �޶����ߵ�ƽ 1

#define   NOP()  asm("nop");

unsigned char CommandStatus=0;                                  //����״̬��־
unsigned char ReceiveBuf[16];                                   //���ջ�����
unsigned char SendDatBuf[16];                                   //���ͻ�����
unsigned char DisplayData =0;                                   //������ʾ����
unsigned char FloorAddress=0;                                   //¥�������ַ
unsigned char CurrentFloor=0;                                   //��ǰ¥��
unsigned int  FlagCount   =0;                                   //ѭ������ 
unsigned char KeyDispState=0;                                   //����尴����ʾ״̬

unsigned char AskFramBuf[4]={0xaa,0x00,0x01,0x0f};             //�ظ�ȷ��֡
/*������ַ��0xaa  �ظ����0x00   ���ݳ��ȣ�0x01   �ظ����ݣ� 0xff*/



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

     unsigned char temp;
     cli();
	 DDRB   =  0xfe;                                                 //LED-X:PB0-�ߵ�ƽ������PB1��������PB5�߼�ͣ��JT����PB6�߿��ţ�KM��
	 PORTB |=  (1<<PB2)|(1<<PB3);                                    //PB2-PB3�Դ�LED  RSL-RUN(�ߵ�ƽ�رգ�
     PORTB &= ~(1<<PB4);                                             //PB4������     

	 DDRC  |=  (1<<PC0)|(1<<PC1)|(1<<PC6);                           //MC595: PC0 PC1 PC6    
	 
	 DDRD  |=  (1<<PD5)|(1<<PD6)|(1<<PD7)|(1<<PD2);                  //HC165������PD5-7   PD2 485 ���ͽ����л��ܽ�
     DDRD  &= ~(1<<PD3);                                             //HC165���봮����λ�Ĵ���  PD3 �����
	 PORTD |=  (1<<PD3);                                             //��������       
	 PORTD |=  (1<<PD2);                                             //����485���ݽ���
 
	 DDRA  &= 0xf0;                                                  //PA0-3���룺PA0-IN1 X1��PA1-IN2 J10 PA2-IN3 J9
	 PORTA |= 0x0f;                                                  //PA3-IN4  ���ģʽ���͵�ƽ������ģʽ���ߵ�ƽ
	 
	 Uart_Init();
	 //EEPROM_Write(0,0x01);
	 FloorAddress = EEPROM_Read(0);                                 //��ȡ������ַ
	 sei();                                                    
	 
	 while(1)
     {  
         
		 /********************���뿪��SW1������********************************/
         temp = HC165Date_Read();
		 FlagCount++;

		 if(PINA&0x08)                                                  //PE3-PRO �ߵ�ƽ-����ģʽ
		 {
		      switch(temp)
		     {
		         case 0xfe:                                             //IN1
			                  {
							       if(CurrentFloor==FloorAddress)       //�����ǰ¥��Ϊ=1
                                   {
								         KeyDispState  = 0x00;          //�ް�����
                                    }

                                   if(CurrentFloor>1)                   //�����ǰ¥��>1 
								   {
								         LED_DW();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x01;          //����¥��

									     KeyDispState  = 0x01;          //����1��
										 Uart_SendFrame(SendDatBuf);    
                                   }

								   break;
                              }
                 case 0xfd:                                            //IN2
			                  {
							       if(CurrentFloor==FloorAddress)      //�����ǰ¥��Ϊ=2
                                   {
								         KeyDispState  = 0x00;         //�ް�����
                                    }

                                   if(CurrentFloor>2)                  //�����ǰ¥��>2
								   {
								         LED_DW();                     //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x02;          //����¥��

									     KeyDispState  = 0x02;          //����2��
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<2)                  //�����ǰ¥��<2
								   {
								         LED_UP();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x02;          //����¥��

									     KeyDispState  = 0x02;          //����2��
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                  case 0xfb:                                           //IN3
			                  {
							       if(CurrentFloor==FloorAddress)      //�����ǰ¥��Ϊ=3
                                   {
								         KeyDispState  = 0x00;         //�ް�����
                                    }

                                   if(CurrentFloor>3)                  //�����ǰ¥��>3
								   {
								         LED_DW();                     //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x03;          //����¥��

									     KeyDispState  = 0x03;          //����3��
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<3)                  //�����ǰ¥��<3
								   {
								         LED_UP();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x03;          //����¥��

									     KeyDispState  = 0x03;          //����3��
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                              }
                  case 0xf7:                                           //IN4
			                  {
							       if(CurrentFloor==FloorAddress)      //�����ǰ¥��Ϊ=4
                                   {
								         KeyDispState  = 0x00;         //�ް�����
                                    }

                                   if(CurrentFloor>4)                  //�����ǰ¥��>4
								   {
								         LED_DW();                     //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x04;          //����¥��

									     KeyDispState  = 0x04;          //����4��
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<4)                  //�����ǰ¥��<4
								   {
								         LED_UP();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x04;          //����¥��

									     KeyDispState  = 0x04;          //����4��
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                  case 0xef:                                           //IN5
			                  {
							       if(CurrentFloor==FloorAddress)      //�����ǰ¥��Ϊ=5
                                   {
								         KeyDispState  = 0x00;         //�ް�����
                                    }

                                   if(CurrentFloor>5)                  //�����ǰ¥��>5
								   {
								         LED_DW();                     //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x05;          //����¥��

									     KeyDispState  = 0x05;          //����5��
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<2)                  //�����ǰ¥��<5
								   {
								         LED_UP();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x05;          //����¥��

									     KeyDispState  = 0x05;          //����5��
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                              }
                  case 0xdf:                                           //IN6
			                  {
							       if(CurrentFloor==FloorAddress)      //�����ǰ¥��Ϊ=6
                                   {
								         KeyDispState  = 0x00;         //�ް�����
                                    }

                                   if(CurrentFloor>6)                  //�����ǰ¥��>6
								   {
								         LED_DW();                     //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x06;          //����¥��

									     KeyDispState  = 0x06;          //����6��
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<6)                  //�����ǰ¥��<6
								   {
								         LED_UP();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x06;          //����¥��

									     KeyDispState  = 0x06;          //����6��
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                  case 0xbf:                                           //IN7
			                  {
							       if(CurrentFloor==FloorAddress)      //�����ǰ¥��Ϊ=7
                                   {
								         KeyDispState  = 0x00;         //�ް�����
                                    }

                                   if(CurrentFloor>7)                  //�����ǰ¥��>7
								   {
								         LED_DW();                     //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x07;          //����¥��

									     KeyDispState  = 0x07;          //����7��
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<7)                  //�����ǰ¥��<7
								   {
								         LED_UP();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x07;          //����¥��

									     KeyDispState  = 0x07;          //����7��
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                              }
                  case 0x7f:                                           //IN8
			                  {
							       if(CurrentFloor==FloorAddress)      //�����ǰ¥��Ϊ=8
                                   {
								         KeyDispState  = 0x00;         //�ް�����
                                    }
                               
								   if(CurrentFloor<8)                  //�����ǰ¥��<8
								   {
								         LED_UP();                      //��������
									  
									     SendDatBuf[0] = FloorAddress;  //�����ַ
                                         SendDatBuf[1] = 0x01;          //���� - �������¥��
								         SendDatBuf[2] = 0x01;          //���ݳ���
								         SendDatBuf[3] = 0x08;          //����¥��

									     KeyDispState  = 0x08;          //����8��
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                    default:         
			                  {
							    break;
                               }
          
		      }//end switch

              DisplayAndUout(CurrentFloor,KeyDispState);               //��ʾ��ǰ¥�� ��������ʾ

         }
		 else                                                          //PE3-PRO �͵�ƽ-���ģʽ 
		 {
		      switch(temp)
		     {
		           case 0xfe:  	EEPROM_Write(0,0x01);                  //��ַ0д��������ַ IN1
								break;

                   case 0xfd:   EEPROM_Write(0,0x02);                  //IN2
								break;

                   case 0xfb:  	EEPROM_Write(0,0x03);
								break;

                   case 0xf7:   EEPROM_Write(0,0x04);
								break;

                   case 0xef:  	EEPROM_Write(0,0x05);
								break;

                   case 0xdf:   EEPROM_Write(0,0x06);
								break;

                   case 0xbf:   EEPROM_Write(0,0x07);
								break;

                   case 0x7f:   EEPROM_Write(0,0x08);                 //IN8
								break;

                    default:         
							    break;
          
		        }//end switch			  
			  
			  FloorAddress = EEPROM_Read(0);                   //��ȡ������ַ
			  DisplayAndUout(FloorAddress,0);                  //��ʾ������ַ-¥��
          }
		 /***********************************************************************/
         if((PINA&0x01)==0)
		 {
		     CurrentFloor  = EEPROM_Read(0);                   //ȡ��ǰ¥��
			 SendDatBuf[0] = CurrentFloor;                     //�����ַ CurrentFloor = FloorAddress
             SendDatBuf[1] = 0x00;                             //���� - ���ƽ�����ź�
		     SendDatBuf[2] = 0x01;                             //���ݳ���
			 SendDatBuf[3] = CurrentFloor;                     //��������

			 //DisplayAndUout(CurrentFloor,0);                 //��ʾ��ǰ¥�� �����������ʾ
             if(CurrentFloor==KeyDispState)                    //�����ǰ¥��=����¥�� ����Ϩ��
			 {
			      KeyDispState = 0;
				  SendDatBuf[4] = KeyDispState;

              }

             if(FlagCount>=20000)
			 {
			      Uart_SendFrame(SendDatBuf);                  //���͸����ذ�
				  FlagCount = 0;
              }
         }
		 
		 /********************�����8·����������********************************/
		 
		
		 /********************485ͨѶ���մ�����********************************/
		 if(CommandStatus)
		 {

			  if(ReceiveBuf[1]==0x00)                           //���� - ���ƽ�����ź�
			  {
                     CurrentFloor = ReceiveBuf[3];
					 KeyDispState = SendDatBuf[4];              //�������źţ����հ�����ʾ״̬

               }
			   if(ReceiveBuf[1]==0x01)                          //���� - �����������¥��
			  {
                     KeyDispState = ReceiveBuf[3];

               }
			   
			   CommandStatus = 0;
		  }    
       /***********************************************************************/

	 }//end while

     return 0;

}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��UART0�����ж��Ӻ���
*****************************************************************/	
SIGNAL (USART_RXC_vect)
{
       unsigned char temp;
	   
	   temp = Uart_ReceiveFrame(ReceiveBuf);
	   if(temp)                               //����ɹ�����һ֡����
       CommandStatus = 1;  
       //Buzzer();
}
