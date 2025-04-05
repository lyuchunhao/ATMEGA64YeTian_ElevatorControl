/************************************************
文件：key63.h
用途：按键函数头文件
注意：系统时钟12M
************************************************/
#ifndef __KEY64_H__
#define __KEY64_H__


#define    INIT_VALUE                0xFF             //初始化值
#define    FUNCTION_SET              0x01             //F功能码
#define    DATA_SET                  0x02             //数据调整
#define    DATA_SAVE                 0x03             //数据保存
#define    TIME_1000MS               100              //100*10ms

#define    RUN_UP                    0x01  
#define    RUN_DOWN                  0x02

#define    FUNCTION_SET_MODE         0x31             //功能设置模式
#define    ELEVATOR_CHECK_MODE       0x32             //电梯检修模式
#define    NORMAL_WORK_MODE          0x33             //正常工作模式
#define    ERRO_DISPLAY_MODE         0x34             //故障显示模式


typedef struct 
{
	unsigned char KeyDown;			// key-pressed
	unsigned char KeyDownBackup;	// the backup of KeyDown
	unsigned char KeyUp;			// key-release
	unsigned char KeyPress;			// key-pressed-and-hold
	unsigned char KeyShort;			// short-pressed
	unsigned char KeyLong;			// long-pressed
}KeyScan;

extern unsigned char  FunctionNum;
extern unsigned char  DataSetNum;
extern unsigned char  EscFunction_Mode;
extern KeyScan KeyScanResult;
extern unsigned char KeyPortState0;		// the last last key-port-state
extern unsigned char KeyPortState1;		// the last key-port-state
extern unsigned long SysTimebase;
extern unsigned long SysTimebase_Wati;
extern unsigned char  Cycle_WithOperate;
extern unsigned int   InsTestCnt;
extern unsigned char  Elevator_Run_Mode; //电梯运行模式 -上行-下行

extern void ScanKey(void);
extern void FuncitonSet(void);
	    

#endif
