#include <avr/io.h>
#include "key64.h"
#include "delay64.h"
#include "EEPROM64.h"
#include "595MC64.h" 

//ȫ�ֱ���
unsigned char  EscFunction_Mode  = INIT_VALUE;
unsigned char  FunctionNum=0;
unsigned char  DataSetNum =0;
unsigned char  Cycle_WithOperate = INIT_VALUE;                      //����ѭ��״̬��
unsigned int   InsTestCnt = 0;
unsigned char  Elevator_Run_Mode = INIT_VALUE;                      //��������ģʽ -����-����

KeyScan KeyScanResult;				// the result of key-scaning
unsigned char KeyPortState0;		// the last last key-port-state
unsigned char KeyPortState1;		// the last key-port-state	
unsigned long SysTimebase;
unsigned long SysTimebase_Wait;

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��PE4-AUTO/INS
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
	

	 //=================   ����״̬��PE4-INS������)--�͵�ƽ=����״̬======================================
	 if((PINE&0x10)==0)                                            //PE4-AUTO/INS  �͵�ƽ-INS
     {  
		 
	 //=================   PE3-PRO �͵�ƽ-���ģʽ===========================================	 
		 if((PINE&0x08)==0)                                   //PE3-PRO       �͵�ƽ-���ģʽ
         {
               
               Cycle_WithOperate = FUNCTION_SET_MODE;               

	           if(KeyScanResult.KeyUp&0x20)                  //ESC���ܼ����� PE5-UP/ESC
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
							                        EEPROM_Write(FunctionNum,DataSetNum);   //���湦�����Ӧ����
													break;													
							default:                NOP();
							                        break;
                      } 
               }
	
	
	           if(KeyScanResult.KeyUp&0x40)                 // PE6-RUN/UP   ����
	          {
	                  if(EscFunction_Mode == FUNCTION_SET)
					            FunctionNum++;
					  if(EscFunction_Mode == DATA_SET)
					            DataSetNum++;
		              //SysTimebase_Wait = SysTimebase + TIME_1000MS;
               }
			   if(KeyScanResult.KeyDown &0x40)                // PE6-RUN/UP   ���¼�ʱ
	          {
		              SysTimebase_Wait = SysTimebase + TIME_1000MS;
               }
	           if(KeyScanResult.KeyPress&0x40)               //����
	          {
	                 if(SysTimebase>SysTimebase_Wait)
		            {
		                        if(EscFunction_Mode == FUNCTION_SET)
					                                FunctionNum++;
					            if(EscFunction_Mode == DATA_SET)
					                                DataSetNum++;
                    }
               }
		 
		 } //end--PE3-PRO       �͵�ƽ-���ģʽ
		 //=================   PE3-PRO �͵�ƽ-���ģʽ===========================================

         //=================   PE3-PRO �ߵ�ƽ-�Ǳ��ģʽ==========================================
		 if(PINE&0x08)                                    //PE3-PRO       �ߵ�ƽ-�Ǳ��ģʽ
         {
               Cycle_WithOperate = ELEVATOR_CHECK_MODE;
			   //�����Ӧ���ܴ���
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
		 //=================   PE3-PRO �ߵ�ƽ-�Ǳ��ģʽ==========================================
     }
    //=================   ����״̬��PE4-INS������)--�͵�ƽ=����״̬======================================
     

	//=================   ����������PE4-AUTO������)--�ߵ�ƽ-����ģʽ   ==================================
	 if(PINE&0x10)                                        //PE4-AUTO/INS  �ߵ�ƽ-AUTO ����ģʽ
    {  
		 
		 //=================   PE3-PRO �ߵ�ƽ-�Ǳ��ģʽ==========================================
		 if(PINE&0x08)                                         //PE3-PRO       �ߵ�ƽ-�Ǳ��ģʽ
         {
               Cycle_WithOperate = NORMAL_WORK_MODE;
			   //�����Ӧ���ܴ���
			   NOP();
         }
		 //=================   PE3-PRO �ߵ�ƽ-�Ǳ��ģʽ==========================================

         
		 //=================   PE3-PRO �͵�ƽ-���ģʽ ===========================================
		 if((PINE&0x08)==0)                                   //PE3-PRO       �͵�ƽ-���ģʽ
         {
               
			   //��ʾ������
			   Cycle_WithOperate = ERRO_DISPLAY_MODE;
			   NOP();
         }
		 //=================   PE3-PRO �͵�ƽ-���ģʽ ===========================================

     }
    //=================   ����������PE4-AUTO������)--�ߵ�ƽ-����ģʽ   ==================================


}
/***************************************************************
Copyright(C), 2014.01.13  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��FuncitonSet����
*****************************************************************/
void FuncitonSet(void)
{
                     switch(EscFunction_Mode)
				     {
				                case INIT_VALUE:                                 //��ʼ��ֵ
					            {
					                     break;
							    }
                   
					            case FUNCTION_SET:                               //����ģʽ
					            {
                                 
			                             if(FunctionNum>28)
								                  FunctionNum =1;   
                                             
										 DataSetNum=EEPROM_Read(FunctionNum);   //��ȡ������F��Ӧ������ 
										 Seg7_Function_display(FunctionNum);    //��ʾ������  
									     break;
							    }
                   
					            case DATA_SET:                                  //����ģʽ
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
													 //F15--F20�� 
													  
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
                   
				            case DATA_SAVE:                                    //����ģʽ
				            {
                                   
								     break;
						    }
                            default:
		
								     break;
                                         
                   }
}	

