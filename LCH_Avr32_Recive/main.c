#include <avr/io.h> //包含MEGA32头文件
#include <util/delay.h>
#include <macros.h>
#include <string.h>
#include <stdio.h>
#include <interrupt.h>
#include "uart32.h"
#include "delay32.h"
#include "595MC32.h"
#include "EEPROM32.h"

#define   LED_UP()  {PORTB |= (1<<PB1); PORTB &= ~(1<<PB0);}    //LED-X 上亮下灭
#define   LED_DW()  {PORTB |= (1<<PB0); PORTB &= ~(1<<PB1);}    //LED-X 上灭下亮
#define   LED_QM()  {PORTB &= ~(1<<PB0);PORTB &= ~(1<<PB1);}    //LED-X 上下全灭
#define   LED_JT()   PORTB |= (1<<PB5)                          //LED-X 急停
#define   LED_KM()   PORTB |= (1<<PB6)                          //LED-X 开门
#define   SIGNAL_ES  PINA&0x02                                  //外呼板串行急停 无动作高电平 1
#define   SIGNAL_PKS PINA&0x04                                  //外呼板门梯锁   无动作高电平 1

#define   NOP()  asm("nop");

unsigned char CommandStatus=0;                                  //接收状态标志
unsigned char ReceiveBuf[16];                                   //接收缓冲区
unsigned char SendDatBuf[16];                                   //发送缓冲区
unsigned char DisplayData =0;                                   //测试显示数据
unsigned char FloorAddress=0;                                   //楼层外呼地址
unsigned char CurrentFloor=0;                                   //当前楼层
unsigned int  FlagCount   =0;                                   //循环计数 
unsigned char KeyDispState=0;                                   //外呼板按键显示状态

unsigned char AskFramBuf[4]={0xaa,0x00,0x01,0x0f};             //回复确认帧
/*主机地址：0xaa  回复命令：0x00   数据长度：0x01   回复内容： 0xff*/



/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：Main函数
*****************************************************************/	 
int main(void)
{

     unsigned char temp;
     cli();
	 DDRB   =  0xfe;                                                 //LED-X:PB0-高电平下亮、PB1高上亮、PB5高急停（JT）、PB6高开门（KM）
	 PORTB |=  (1<<PB2)|(1<<PB3);                                    //PB2-PB3自带LED  RSL-RUN(高电平关闭）
     PORTB &= ~(1<<PB4);                                             //PB4蜂鸣器     

	 DDRC  |=  (1<<PC0)|(1<<PC1)|(1<<PC6);                           //MC595: PC0 PC1 PC6    
	 
	 DDRD  |=  (1<<PD5)|(1<<PD6)|(1<<PD7)|(1<<PD2);                  //HC165控制线PD5-7   PD2 485 发送接收切换管脚
     DDRD  &= ~(1<<PD3);                                             //HC165并入串出移位寄存器  PD3 输出口
	 PORTD |=  (1<<PD3);                                             //输入上拉       
	 PORTD |=  (1<<PD2);                                             //允许485数据接收
 
	 DDRA  &= 0xf0;                                                  //PA0-3输入：PA0-IN1 X1、PA1-IN2 J10 PA2-IN3 J9
	 PORTA |= 0x0f;                                                  //PA3-IN4  编程模式：低电平、正常模式：高电平
	 
	 Uart_Init();
	 //EEPROM_Write(0,0x01);
	 FloorAddress = EEPROM_Read(0);                                 //读取外呼板地址
	 sei();                                                    
	 
	 while(1)
     {  
         
		 /********************编码开关SW1处理部分********************************/
         temp = HC165Date_Read();
		 FlagCount++;

		 if(PINA&0x08)                                                  //PE3-PRO 高电平-正常模式
		 {
		      switch(temp)
		     {
		         case 0xfe:                                             //IN1
			                  {
							       if(CurrentFloor==FloorAddress)       //如果当前楼层为=1
                                   {
								         KeyDispState  = 0x00;          //无按键亮
                                    }

                                   if(CurrentFloor>1)                   //如果当前楼层>1 
								   {
								         LED_DW();                      //下亮上灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x01;          //到达楼层

									     KeyDispState  = 0x01;          //按键1亮
										 Uart_SendFrame(SendDatBuf);    
                                   }

								   break;
                              }
                 case 0xfd:                                            //IN2
			                  {
							       if(CurrentFloor==FloorAddress)      //如果当前楼层为=2
                                   {
								         KeyDispState  = 0x00;         //无按键亮
                                    }

                                   if(CurrentFloor>2)                  //如果当前楼层>2
								   {
								         LED_DW();                     //下亮上灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x02;          //到达楼层

									     KeyDispState  = 0x02;          //按键2亮
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<2)                  //如果当前楼层<2
								   {
								         LED_UP();                      //上亮下灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x02;          //到达楼层

									     KeyDispState  = 0x02;          //按键2亮
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                  case 0xfb:                                           //IN3
			                  {
							       if(CurrentFloor==FloorAddress)      //如果当前楼层为=3
                                   {
								         KeyDispState  = 0x00;         //无按键亮
                                    }

                                   if(CurrentFloor>3)                  //如果当前楼层>3
								   {
								         LED_DW();                     //下亮上灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x03;          //到达楼层

									     KeyDispState  = 0x03;          //按键3亮
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<3)                  //如果当前楼层<3
								   {
								         LED_UP();                      //上亮下灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x03;          //到达楼层

									     KeyDispState  = 0x03;          //按键3亮
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                              }
                  case 0xf7:                                           //IN4
			                  {
							       if(CurrentFloor==FloorAddress)      //如果当前楼层为=4
                                   {
								         KeyDispState  = 0x00;         //无按键亮
                                    }

                                   if(CurrentFloor>4)                  //如果当前楼层>4
								   {
								         LED_DW();                     //下亮上灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x04;          //到达楼层

									     KeyDispState  = 0x04;          //按键4亮
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<4)                  //如果当前楼层<4
								   {
								         LED_UP();                      //上亮下灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x04;          //到达楼层

									     KeyDispState  = 0x04;          //按键4亮
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                  case 0xef:                                           //IN5
			                  {
							       if(CurrentFloor==FloorAddress)      //如果当前楼层为=5
                                   {
								         KeyDispState  = 0x00;         //无按键亮
                                    }

                                   if(CurrentFloor>5)                  //如果当前楼层>5
								   {
								         LED_DW();                     //下亮上灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x05;          //到达楼层

									     KeyDispState  = 0x05;          //按键5亮
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<2)                  //如果当前楼层<5
								   {
								         LED_UP();                      //上亮下灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x05;          //到达楼层

									     KeyDispState  = 0x05;          //按键5亮
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                              }
                  case 0xdf:                                           //IN6
			                  {
							       if(CurrentFloor==FloorAddress)      //如果当前楼层为=6
                                   {
								         KeyDispState  = 0x00;         //无按键亮
                                    }

                                   if(CurrentFloor>6)                  //如果当前楼层>6
								   {
								         LED_DW();                     //下亮上灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x06;          //到达楼层

									     KeyDispState  = 0x06;          //按键6亮
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<6)                  //如果当前楼层<6
								   {
								         LED_UP();                      //上亮下灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x06;          //到达楼层

									     KeyDispState  = 0x06;          //按键6亮
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                  case 0xbf:                                           //IN7
			                  {
							       if(CurrentFloor==FloorAddress)      //如果当前楼层为=7
                                   {
								         KeyDispState  = 0x00;         //无按键亮
                                    }

                                   if(CurrentFloor>7)                  //如果当前楼层>7
								   {
								         LED_DW();                     //下亮上灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x07;          //到达楼层

									     KeyDispState  = 0x07;          //按键7亮
										 Uart_SendFrame(SendDatBuf);
                                   }

								   if(CurrentFloor<7)                  //如果当前楼层<7
								   {
								         LED_UP();                      //上亮下灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x07;          //到达楼层

									     KeyDispState  = 0x07;          //按键7亮
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                              }
                  case 0x7f:                                           //IN8
			                  {
							       if(CurrentFloor==FloorAddress)      //如果当前楼层为=8
                                   {
								         KeyDispState  = 0x00;         //无按键亮
                                    }
                               
								   if(CurrentFloor<8)                  //如果当前楼层<8
								   {
								         LED_UP();                      //上亮下灭
									  
									     SendDatBuf[0] = FloorAddress;  //外呼地址
                                         SendDatBuf[1] = 0x01;          //命令 - 外呼呼叫楼层
								         SendDatBuf[2] = 0x01;          //数据长度
								         SendDatBuf[3] = 0x08;          //到达楼层

									     KeyDispState  = 0x08;          //按键8亮
										 Uart_SendFrame(SendDatBuf);
                                   }
								   break;
                               }
                    default:         
			                  {
							    break;
                               }
          
		      }//end switch

              DisplayAndUout(CurrentFloor,KeyDispState);               //显示当前楼层 按键无显示

         }
		 else                                                          //PE3-PRO 低电平-编程模式 
		 {
		      switch(temp)
		     {
		           case 0xfe:  	EEPROM_Write(0,0x01);                  //地址0写入外呼板地址 IN1
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
			  
			  FloorAddress = EEPROM_Read(0);                   //读取外呼板地址
			  DisplayAndUout(FloorAddress,0);                  //显示外呼板地址-楼层
          }
		 /***********************************************************************/
         if((PINA&0x01)==0)
		 {
		     CurrentFloor  = EEPROM_Read(0);                   //取当前楼层
			 SendDatBuf[0] = CurrentFloor;                     //外呼地址 CurrentFloor = FloorAddress
             SendDatBuf[1] = 0x00;                             //命令 - 外呼平层有信号
		     SendDatBuf[2] = 0x01;                             //数据长度
			 SendDatBuf[3] = CurrentFloor;                     //发送数据

			 //DisplayAndUout(CurrentFloor,0);                 //显示当前楼层 按键无输出显示
             if(CurrentFloor==KeyDispState)                    //如果当前楼层=到达楼层 按键熄灭
			 {
			      KeyDispState = 0;
				  SendDatBuf[4] = KeyDispState;

              }

             if(FlagCount>=20000)
			 {
			      Uart_SendFrame(SendDatBuf);                  //发送给主控板
				  FlagCount = 0;
              }
         }
		 
		 /********************外呼板8路按键输入检测********************************/
		 
		
		 /********************485通讯接收处理部分********************************/
		 if(CommandStatus)
		 {

			  if(ReceiveBuf[1]==0x00)                           //命令 - 外呼平层有信号
			  {
                     CurrentFloor = ReceiveBuf[3];
					 KeyDispState = SendDatBuf[4];              //门区有信号，接收按键显示状态

               }
			   if(ReceiveBuf[1]==0x01)                          //命令 - 外呼按键呼叫楼层
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
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：UART0接收中断子函数
*****************************************************************/	
SIGNAL (USART_RXC_vect)
{
       unsigned char temp;
	   
	   temp = Uart_ReceiveFrame(ReceiveBuf);
	   if(temp)                               //如果成功接收一帧数据
       CommandStatus = 1;  
       //Buzzer();
}
