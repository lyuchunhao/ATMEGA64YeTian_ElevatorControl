###############################################################################
# Makefile for the project Ledtest
###############################################################################

## General Flags
PROJECT = Ledtest
MCU = atmega32
TARGET = Ledtest.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                                                     -DF_CPU=12000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"F:\AVR_Soft\AVR_20130909V5.0\LCH_Avr32_Recive\Ledtest\..\..\avr\include" -I"F:\AVR_Soft\AVR_20130909V5.0\LCH_Avr32_Recive\Ledtest\..\..\avr\include\util" -I"F:\AVR_Soft\AVR_20130909V5.0\LCH_Avr32_Recive\Ledtest\..\..\lib\gcc\avr32\4.3.2\include\c++\debug" -I"F:\AVR_Soft\AVR_20130909V5.0\LCH_Avr32_Recive\Ledtest\..\..\include" 

## Library Directories
LIBDIRS = -L"C:\Program Files\Atmel\AVR Tools\avr\lib\avr5" 

## Objects that must be built in order to link
OBJECTS = main.o 595MC32.o delay32.o EEPROM32.o uart32.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Ledtest.hex Ledtest.eep size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

595MC32.o: ../595MC32.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

delay32.o: ../delay32.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

EEPROM32.o: ../EEPROM32.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart32.o: ../uart32.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) Ledtest.elf dep/* Ledtest.hex Ledtest.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

