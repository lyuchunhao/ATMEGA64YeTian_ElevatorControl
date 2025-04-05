#include <avr/io.h>
#include "EEPROM64.h"
#include "delay64.h"

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：EEPROM写操作
*****************************************************************/
void EEPROM_Write(unsigned int EepAddress,unsigned char EeData)
{
      while(EECR&(1<<EEWE));             //等待上一次写操作结束
      EEAR = EepAddress;                //设置地址和数据寄存器
	  EEDR = EeData;
	  EECR|= (1<<EEMWE);                //置位EEMWE
	  NOP();
	  EECR|= (1<<EEWE);                 //置位EEWE以启动写操作
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
作者           ：LCH_shadow 
单片机型号     ：ATmega64
编译工具       ：AVR Studio 4.18   WINAVR-20100110
时钟频率       ：12M
说明           ：EEPROM读操作
*****************************************************************/
unsigned char EEPROM_Read(unsigned int EepAddress)
{
      while(EECR&(1<<EEWE));              //等待上一次写操作结束
      EEAR = EepAddress;                //设置地址和数据寄存器

	  NOP();
	  EECR|= (1<<EERE);                 //置位EERE以启动读操作
      return EEDR;
}
