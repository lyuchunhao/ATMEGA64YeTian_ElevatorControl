/************************************************
�ļ���delay.h
��;����ʱ����ͷ�ļ�
ע�⣺ϵͳʱ��8M
************************************************/
#ifndef __EEPROM32_H__
#define __EEPROM32_H__

#include "delay32.h"

unsigned char EEPROM_Read(unsigned int EepAddress);
void EEPROM_Write(unsigned int EepAddress,unsigned char EeData);




#endif
