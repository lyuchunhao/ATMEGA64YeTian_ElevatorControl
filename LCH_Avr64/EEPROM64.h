/************************************************
文件：EEPROM64.h
用途：E2函数头文件
注意：系统时钟12M
************************************************/
#ifndef __EEPROM64_H__
#define __EEPROM64_H__



extern unsigned char EEPROM_Read(unsigned int EepAddress);
extern void EEPROM_Write(unsigned int EepAddress,unsigned char EeData);


#endif
