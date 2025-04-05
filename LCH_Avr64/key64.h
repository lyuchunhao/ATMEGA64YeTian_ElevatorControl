/************************************************
�ļ���key63.h
��;����������ͷ�ļ�
ע�⣺ϵͳʱ��12M
************************************************/
#ifndef __KEY64_H__
#define __KEY64_H__


#define    INIT_VALUE                0xFF             //��ʼ��ֵ
#define    FUNCTION_SET              0x01             //F������
#define    DATA_SET                  0x02             //���ݵ���
#define    DATA_SAVE                 0x03             //���ݱ���
#define    TIME_1000MS               100              //100*10ms

#define    RUN_UP                    0x01  
#define    RUN_DOWN                  0x02

#define    FUNCTION_SET_MODE         0x31             //��������ģʽ
#define    ELEVATOR_CHECK_MODE       0x32             //���ݼ���ģʽ
#define    NORMAL_WORK_MODE          0x33             //��������ģʽ
#define    ERRO_DISPLAY_MODE         0x34             //������ʾģʽ


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
extern unsigned char  Elevator_Run_Mode; //��������ģʽ -����-����

extern void ScanKey(void);
extern void FuncitonSet(void);
	    

#endif
