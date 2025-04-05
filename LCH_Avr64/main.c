#include <avr/io.h>                  //包含MEGA64头文件
#include <util/delay.h>
#include <macros.h>
#include <string.h>
#include <stdio.h>
#include <interrupt.h>
#include <wdt.h>
#include "595MC64.h"                 //显示头文件
#include "delay64.h"                 //延时头文件
#include "key64.h"                   //按键处理头文件
#include "uart64.h"                  //uart串口处理头文件
#include "EEPROM64.h"                //E2读写头文件
#include "EleworkLogic.h"            //电梯运行逻辑头文件

#define   ENCODER_A     (PIND&0x08)
#define   ENCODER_B     (PING&0x08)

unsigned char  CommandStatus=0;                        //485通讯接收状态标志
unsigned char  CurrentFloor=0;                         //当前楼层
unsigned char  ReceiveBuf[16];                         //485通讯接收缓冲器
unsigned char  SendDatBuf[16];                         //485发送缓冲器
unsigned char  TargetFloor = 0;                        //目标楼层

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：Board端口初始化函数IO
*****************************************************************/
void Port_Init(void)
{
     DDRA  = 0x00;                          //输入IO配置PA0-7：X1=X8
	 PORTA = 0xff;
	 DDRC  = 0x00;                          //X9-X14:PC7-2   PC0-1:A8(MS)-A9(JT)                     
	 PORTC = 0xff;
	 	 
	 DDRB   = 0xff;                         //继电器IO初始化 1-打开继电器；0-关闭

	 DDRE  &= 0x0f;                         //键盘IO PE4-PE7设置为输入
     PORTE |= 0xf0;                         //键盘PE4-PE7输入状态，上拉电阻
     

	 DDRE  &= 0xfc;                         //485通讯管脚PE0、PE1设置为输入
     PORTE &= 0xfc;                         //不设置上拉
	 DDRE  |=  (1<<PE2);                    //485发送接收切换管脚，0 接收  1 发送
     PORTE |=  (1<<PE2);                    //允许接收
	 
	 DDRE  &= ~(1<<PE3);                    //编程模式管脚PE3-PRO设置为输入
     PORTE |=  (1<<PE3);                    //设置上拉电阻
	 
	 DDRD  |=  0xf0;                        //HC595:PD4-PD7设置为输出  
     DDRD  |=  (1<<PD0);                    //数码管位选,设置为输出
     DDRD  |=  (1<<PD1);
     DDRD  |=  (1<<PD2);
     
	 DDRD  &= ~(1<<PD3);                    //外部中断3 编码器A
     PORTE |=  (1<<PE3);                    //设置上拉电

	 DDRG  |=  (1<<PG0)|(1<<PG1);
     DDRG  &= ~(1<<PG3);                    //编码器B
     PORTG |=  (1<<PG3);                    //设置上拉电
}

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

	 Port_Init();                           //Board端口初始化配置
	 Init_Devices();                        //485通讯端口配置初始化 
     Uart_Init();                           
	 
	 TIMSK = 0x01;                          //8位定时器中断0/溢出中断使能
	 TCCR0 = 0x07;                          //64分频
	 TCNT0 = 138;                           //(255-138)*(1024/12M)=9984us=10ms
 
     EIMSK = 0x08;                          //使能外部中断3
	 EICRA = 0xC0;                          //外部中断3上升沿产生中断
	 
	 TimerCnt = 0;
     wdt_enable(WDTO_2S);

	 while(1)
     {   
         
		 ScanKey();
		 wdt_reset();                       //喂狗

		 /******************************功能设置处理部分***********************************/
	     if(Cycle_WithOperate == FUNCTION_SET_MODE)                          //功能设置模式
		 {
		          
				   FuncitonSet();
								 
          }
		 /*******************************功能设置处理部分******************************/
		 

		 /******************************检修状态处理部分***********************************/
		 if(Cycle_WithOperate == ELEVATOR_CHECK_MODE)                          //检修处理模式
		 {

                  
				  if(((PINC&0x02)==0)&&((PINC&0x01)==0))                                 //PC1-JT PC0-MS 都高电平=无急停或者门锁信号  
				  {
				      switch(Elevator_Run_Mode)
				     {
				         case RUN_UP    : PORTB |=  (1<<PB7)|(1<<PB5);         //开Y1-Y3
				                          PORTB &= ~(1<<PB6);                  //关Y2
						                  break;
                         case RUN_DOWN  : PORTB |=  (1<<PB6)|(1<<PB5);         //开Y2-Y3
				                          PORTB &= ~(1<<PB7);                  //关Y1
						                  break;
                         case INIT_VALUE: PORTB &= ~(1<<PB7);                  //关Y1
						                  PORTB &= ~(1<<PB6);                  //关Y2
										  PORTB &= ~(1<<PB5);                  //关Y3
						                  break;
                         default        : NOP();
						                  break;
                      }
				  }
				  else
				  {     
				         //停止运行-code                                       //有其中一个为低电平=停车
						 PORTB &= ~(1<<PB7);                  //关Y1
						 PORTB &= ~(1<<PB6);                  //关Y2
				         PORTB &= ~(1<<PB5);                  //关Y3
                   }
 	              Seg7_Led_display(CurrentFloor);                                  //总体显示
				   
		 }
	     /******************************检修状态处理部分***********************************/
		
		 
		 
		 
		 /******************************正常工作处理部分***********************************/
		 if(Cycle_WithOperate == NORMAL_WORK_MODE)                            //正常工作模式
		 {
                  
                  Seg7_Led_display(CurrentFloor);                             //总体显示

				  if((PINA&0x40)==0)                                          //如果有上下限位开关
                 {
							LiftStop(); 
								   
                  }
				  
				  if(CurrentFloor == 0)                                       //重新上电后，电梯平层
                  {
				            LiftRunDw();                                      //电梯下行找平层
	
                  }   
                 

		 }
	     /******************************正常工作处理部分***********************************/
		
		
		
		
		/********************485通讯接收处理部分***************************************/
         if(CommandStatus)
		 {			  
			       if(ReceiveBuf[1]==0x00)                                //地址+命令 楼层
			       {
                            CurrentFloor = ReceiveBuf[3];                 //外呼发送过来的楼层
                            
                    }
					if(ReceiveBuf[1]==0x01)                                //命令 - 外呼呼叫楼层
			       {
                            TargetFloor  = ReceiveBuf[3];                 //外呼发送过来的将要到达的楼层
                            
                    }

			       CommandStatus = 0;
		 }   
		 

		 //Seg7_Led_display(CurrentFloor);                                  //总体显示
        /**********************************************************************************/
       
     }//end while
			        

}//end main


/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：UART0接收中断子函数
*****************************************************************/	
SIGNAL(USART0_RX_vect)
{
	   unsigned char temp;
	   
	   temp = Uart_ReceiveFrame(ReceiveBuf);
	   if(temp)                                                            //如果成功接收一帧数据
       CommandStatus = 1;

	   if(ReceiveBuf[1]==0x00)                                            //地址+命令 楼层
	  {
              CurrentFloor = ReceiveBuf[3];                 //外呼发送过来的楼层
                            
      }


	   
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：Timer0溢出中断  （1M 定时9920us约10ms）
*****************************************************************/	
SIGNAL(TIMER0_OVF_vect)
{
	   TCNT0 = 100;
	   
	   SysTimebase++;
	   if(TimerCnt>65000)
	          TimerCnt=0;

       TimerCnt++;                 //电机转速采集计数
	   if(TimerCnt>5)
	   {
	          TimerCnt=0;
			  EncoderCntHz = 0;    //参数清零
       }
      
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：外部中断3   上升沿中断
*****************************************************************/	
SIGNAL(INT3_vect)
{
	  if(ENCODER_B)                 //B相高电平  反转
	  {
	          PORTG &= ~(1<<PG0);    //反转指示灯RUN
			  PORTG |=  (1<<PG1);
	  }
	  else
	  {
	          PORTG &= ~(1<<PG1);    //正转指示灯RSL
			  PORTG |=  (1<<PG0);
	   }	  
	  EncoderCntHz++;  
      
}






	   

