/************************************************
文件：uart64.h
用途：串口通讯函数头文件
注意：系统时钟12M
************************************************/
#ifndef __UART64_H__
#define __UART64_H__


extern void Init_Devices(void);
extern void Uart_Init(void);
extern void Uart_SendData(unsigned char data);
extern void Uart_SendFrame(unsigned char *s);
extern unsigned char Uart_ReceiveData(void);
extern unsigned char Uart_ReceiveFrame(unsigned char *s);

#endif
