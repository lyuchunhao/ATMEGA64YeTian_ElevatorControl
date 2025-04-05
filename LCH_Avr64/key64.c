#include <avr/io.h>
#include "key64.h"
#include "delay64.h"
#include "EEPROM64.h"
#include "595MC64.h" 

//全局变量
unsigned char  EscFunction_Mode  = INIT_VALUE;
unsigned char  FunctionNum=0;
unsigned char  DataSetNum =0;
unsigned char  Cycle_WithOperate = INIT_VALUE;                      //操作循环状态字
unsigned int   InsTestCnt = 0;
unsigned char  Elevator_Run_Mode = INIT_VALUE;                      //电梯运行模式 -上行-下行

KeyScan KeyScanResult;				// the result of key-scaning
unsigned char KeyPortState0;		// the last last key-port-state
unsigned char KeyPortState1;		// the last key-port-state	
unsigned long SysTimebase;
unsigned long SysTimebase_Wait;

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：PE4-AUTO/INS
                 PE5-UP/ESC
                 PE6-RUN/UP
                 PE7-DN/ENTER
*****************************************************************/
void ScanKey(void)
{
	unsigned char KeyPortState;		// current key port state
	
	//ClearKeyScanResult();
	KeyScanResult.KeyDown			= 0;
	//KeyScanResult.KeyDownBackup	= 0;
	KeyScanResult.KeyUp				= 0;
	KeyScanResult.KeyPress			= 0;
	KeyScanResult.KeyShort			= 0;
	KeyScanResult.KeyLong			= 0;
	
	// get the current hardware port state
	KeyPortState=PINE;
	//KeyPortState=~KeyPortState;
	
	KeyScanResult.KeyDown  = ((KeyPortState0 ^ KeyPortState1) & KeyPortState0)
								&(KeyPortState0 ^ KeyPortState);
	KeyScanResult.KeyUp	   = ((KeyPortState0 ^ KeyPortState1) & KeyPortState1)
								&(KeyPortState0 ^ KeyPortState);
	KeyScanResult.KeyPress = ((KeyScanResult.KeyDown | KeyScanResult.KeyUp
								|KeyPortState | KeyPortState1) ^ 0xff) & 0xf0;
	
	KeyPortState0 = KeyPortState1;		// update the key-port-state
	KeyPortState1 = KeyPortState;
	

	 //=================   检修状态：PE4-INS（检修)--低电平=检修状态======================================
	 if((PINE&0x10)==0)                                            //PE4-AUTO/INS  低电平-INS
     {  
		 
	 //=================   PE3-PRO 低电平-编程模式===========================================	 
		 if((PINE&0x08)==0)                                   //PE3-PRO       低电平-编程模式
         {
               
               Cycle_WithOperate = FUNCTION_SET_MODE;               

	           if(KeyScanResult.KeyUp&0x20)                  //ESC功能键设置 PE5-UP/ESC
	          {	
	                  switch(EscFunction_Mode)
					  {
					        case  INIT_VALUE  :     EscFunction_Mode = FUNCTION_SET;
							                        FunctionNum =1;
							                        break;
                            case  FUNCTION_SET:     EscFunction_Mode = INIT_VALUE;
							                        break;
                            case  DATA_SET    :     EscFunction_Mode = FUNCTION_SET;
							                        break;
                            default:                NOP();
							                        break;
                      }
               }

	           if(KeyScanResult.KeyUp&0x80)                  //PE7-DN/ENTER
	          {
                      switch(EscFunction_Mode)
					  {
                            case  FUNCTION_SET:     EscFunction_Mode = DATA_SET;
							                        break;
                            case  DATA_SET   :      EscFunction_Mode = FUNCTION_SET;							                        
							                        EEPROM_Write(FunctionNum,DataSetNum);   //保存功能码对应数据
													break;													
							default:                NOP();
							                        break;
                      } 
               }
	
	
	           if(KeyScanResult.KeyUp&0x40)                 // PE6-RUN/UP   单击
	          {
	                  if(EscFunction_Mode == FUNCTION_SET)
					            FunctionNum++;
					  if(EscFunction_Mode == DATA_SET)
					            DataSetNum++;
		              //SysTimebase_Wait = SysTimebase + TIME_1000MS;
               }
			   if(KeyScanResult.KeyDown &0x40)                // PE6-RUN/UP   按下计时
	          {
		              SysTimebase_Wait = SysTimebase + TIME_1000MS;
               }
	           if(KeyScanResult.KeyPress&0x40)               //长按
	          {
	                 if(SysTimebase>SysTimebase_Wait)
		            {
		                        if(EscFunction_Mode == FUNCTION_SET)
					                                FunctionNum++;
					            if(EscFunction_Mode == DATA_SET)
					                                DataSetNum++;
                    }
               }
		 
		 } //end--PE3-PRO       低电平-编程模式
		 //=================   PE3-PRO 低电平-编程模式===========================================

         //=================   PE3-PRO 高电平-非编程模式==========================================
		 if(PINE&0x08)                                    //PE3-PRO       高电平-非编程模式
         {
               Cycle_WithOperate = ELEVATOR_CHECK_MODE;
			   //添加相应功能代码
			   NOP();
               Elevator_Run_Mode = INIT_VALUE;

			   if((KeyScanResult.KeyPress&0x40)&&(KeyScanResult.KeyPress&0x20))            //RUN+UP
			  {
			         Elevator_Run_Mode = RUN_UP;
			   }
			   if((KeyScanResult.KeyPress&0x40)&&(KeyScanResult.KeyPress&0x80))            //RUN+UP
			  {
			         Elevator_Run_Mode = RUN_DOWN;

			   }
			         
         }
		 //=================   PE3-PRO 高电平-非编程模式==========================================
     }
    //=================   检修状态：PE4-INS（检修)--低电平=检修状态======================================
     

	//=================   正常工作：PE4-AUTO（正常)--高电平-正常模式   ==================================
	 if(PINE&0x10)                                        //PE4-AUTO/INS  高电平-AUTO 正常模式
    {  
		 
		 //=================   PE3-PRO 高电平-非编程模式==========================================
		 if(PINE&0x08)                                         //PE3-PRO       高电平-非编程模式
         {
               Cycle_WithOperate = NORMAL_WORK_MODE;
			   //添加相应功能代码
			   NOP();
         }
		 //=================   PE3-PRO 高电平-非编程模式==========================================

         
		 //=================   PE3-PRO 低电平-编程模式 ===========================================
		 if((PINE&0x08)==0)                                   //PE3-PRO       低电平-编程模式
         {
               
			   //显示故障码
			   Cycle_WithOperate = ERRO_DISPLAY_MODE;
			   NOP();
         }
		 //=================   PE3-PRO 低电平-编程模式 ===========================================

     }
    //=================   正常工作：PE4-AUTO（正常)--高电平-正常模式   ==================================


}
/***************************************************************
Copyright(C), 2014.01.13  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：FuncitonSet函数
*****************************************************************/
void FuncitonSet(void)
{
                     switch(EscFunction_Mode)
				     {
				                case INIT_VALUE:                                 //初始化值
					            {
					                     break;
							    }
                   
					            case FUNCTION_SET:                               //功能模式
					            {
                                 
			                             if(FunctionNum>28)
								                  FunctionNum =1;   
                                             
										 DataSetNum=EEPROM_Read(FunctionNum);   //读取功能码F对应的数据 
										 Seg7_Function_display(FunctionNum);    //显示功能码  
									     break;
							    }
                   
					            case DATA_SET:                                  //数据模式
					            {
                                     

										 switch(FunctionNum)
										{
										                  case 1: 
												     {
												              if(DataSetNum>8)
														               DataSetNum=2;
	                                                              break;
												     }
														  case 2: 
											         {
											                  if(DataSetNum>1)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 3: 
											         {
											                  if(DataSetNum>8)
													                   DataSetNum=1;
                                                              break;
											         }
													  case 4: 
											         {
											                  if(DataSetNum>200)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 5: 
											         {
											                  if(DataSetNum>2)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 6: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 7: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 8: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 9: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 10: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 11: 
											         {
											                  if(DataSetNum>1)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 12: 
											         {
											                  if(DataSetNum>1)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 13: 
											         {
											                  if(DataSetNum>1)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 14: 
											         {
											                  if(DataSetNum>1)
													                   DataSetNum=0;
                                                              break;
											         }
													 //F15--F20空 
													  
													  case 21: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 22: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 23: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 24: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 25: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 26: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 27: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  case 28: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
													  default: 
											         {
											                  if(DataSetNum>255)
													                   DataSetNum=0;
                                                              break;
											         }
											         
                                            
                                    }
									Seg7_Led_display(DataSetNum);
									break;
						    }
                   
				            case DATA_SAVE:                                    //保存模式
				            {
                                   
								     break;
						    }
                            default:
		
								     break;
                                         
                   }
}	

