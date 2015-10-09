###############################################################################
# Makefile by 
###############################################################################

## General Flags
PROJECT =test
MCU = atmega328p

################


TARGET = test.elf
CC = avr-gcc


CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep2/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=wetterstation.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) $(PROJECT).hex  $(PROJECT).eep  $(PROJECT).lss size

## Compile
main.o:  main.c
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
	-rm -rf $(OBJECTS) $(PROJECT).elf dep2/*  $(PROJECT).hex  $(PROJECT).eep  $(PROJECT).lss  $(PROJECT).map

flash:   $(PROJECT).hex 
	stty hupcl < /dev/ttyACM2; \
	avrdude -p $(MCU) -c stk500v2 -e -U flash:w:$(PROJECT).hex -P  /dev/ttyACM2


flash1: $(PROJECT).hex 
	stty hupcl < /dev/ttyUSB0; \
	avrdude -c arduino -p $(MCU) -P /dev/ttyUSB0 \
	-b 57600 -U flash:w:$(PROJECT).hex

	
## Other dependencies
-include $(shell mkdir dep2 2>/dev/null) $(wildcard dep2/*)
