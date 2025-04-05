#ifndef __UART32_H__
#define __UART32_H__

#include "delay32.h"

void Uart_Init(void);
void Uart_SendData(unsigned char data);
void Uart_SendFrame(unsigned char *s);
unsigned char Uart_ReceiveData(void);
unsigned char Uart_ReceiveFrame(unsigned char *s);



#endif
