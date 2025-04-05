#include <avr/io.h>
#include "EEPROM64.h"
#include "delay64.h"

/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��EEPROMд����
*****************************************************************/
void EEPROM_Write(unsigned int EepAddress,unsigned char EeData)
{
      while(EECR&(1<<EEWE));             //�ȴ���һ��д��������
      EEAR = EepAddress;                //���õ�ַ�����ݼĴ���
	  EEDR = EeData;
	  EECR|= (1<<EEMWE);                //��λEEMWE
	  NOP();
	  EECR|= (1<<EEWE);                 //��λEEWE������д����
}
/***************************************************************
Copyright(C), 2013.08.11  Sunny Studio
����           ��LCH_shadow 
��Ƭ���ͺ�     ��ATmega64
���빤��       ��AVR Studio 4.18   WINAVR-20100110
ʱ��Ƶ��       ��12M
˵��           ��EEPROM������
*****************************************************************/
unsigned char EEPROM_Read(unsigned int EepAddress)
{
      while(EECR&(1<<EEWE));              //�ȴ���һ��д��������
      EEAR = EepAddress;                //���õ�ַ�����ݼĴ���

	  NOP();
	  EECR|= (1<<EERE);                 //��λEERE������������
      return EEDR;
}
