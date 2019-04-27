TARGET=main
MCU = attiny85

CC=avr-gcc
LD=avr-ld 
AR=avr-ar
AS=avr-as
CP=avr-objcopy
OD=avr-objdump

BIN=$(TARGET).hex
EEP=$(TARGET).eep
EXECUTABLE=$(TARGET).elf

SRC = $(TARGET).c \
      
INC = -I./ \

   
#  C source files
CFILES = $(filter %.c, $(SRC))
#  Assembly source files
ASMFILES = $(filter %.s, $(SRC))

# Object files
COBJ = $(CFILES:.c=.o)
SOBJ = $(ASMFILES:.s=.o)
OBJ  = $(COBJ) $(SOBJ)


MCFLAGS = -mmcu=$(MCU)
OPTIMIZE=-O3 -ffunction-sections
DEBUG   = -g3
CFLAGS  =$(MCFLAGS) $(DEBUG)  $(OPTIMIZE) -Wall -MP -MMD

ASFLAGS = $(DEBUG)

# Defines to be passed to the compiler


all: $(BIN)

$(EXECUTABLE): $(OBJ) $(LDSCRIPT)
	$(CC) $(MCFLAGS) $(sort $(OBJ)) -o $@

$(COBJ): %.o: %.c
	$(CC) -c $(DEFINES) $(INC) $(CFLAGS) $< -o $@

$(SOBJ): %.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@
	
$(BIN): $(EXECUTABLE)
	$(CP) -O ihex -R .eeprom $^ $@

$(EEP): $(EXECUTABLE)
	$(CP) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $^ $@

clean:
	rm -f $(OBJ) $(BIN) $(EXECUTABLE) $(COBJ:.o=.d)

program: $(BIN)
	avrdude -c avrispmkII -P usb -p t85 -U flash:w:$<

program-eep: $(EEP)
	avrdude -c avrispmkII -P usb -p t85 -U eeprom:w:$<

reset :
	avrdude -c avrispmkII -P usb -p t85 

-include  $(COBJ:.o=.d)

.PHONY: clean program program-eep reset
