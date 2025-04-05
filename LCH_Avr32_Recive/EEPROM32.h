/************************************************
文件：delay.h
用途：延时函数头文件
注意：系统时钟8M
************************************************/
#ifndef __EEPROM32_H__
#define __EEPROM32_H__

#include "delay32.h"

unsigned char EEPROM_Read(unsigned int EepAddress);
void EEPROM_Write(unsigned int EepAddress,unsigned char EeData);




#endif
