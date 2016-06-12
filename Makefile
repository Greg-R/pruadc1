# Makefile for PRU ADC

INCLUDE_PATHS=-I$(PRU_CGT)/includeSupportPackage -I$(PRU_CGT)/include
CFLAGS=$(INCLUDE_PATHS) --hardware_mac=on --c99
LNK_CMDS1=-z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include -i$(PRU_CGT)/includeSupportPackage
LNK_CMDS2=-i$(PRU_CGT)/includeSupportPackage/am335x --reread_libs --stack_size=0x100
LNK_CMDS3=--heap_size=0x100

#SOURCES=$(wildcard *.c)
#OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

SOURCES=pru0adc.c

all: $(SOURCES)
	clpru --c99 $(SOURCES) $(LNK_CMDS1) $(LNK_CMDS2) $(LNK_CMDS3) ./AM335x_PRU.cmd -o am335x-pru0-fw 

clean:
	rm -rf $(OBJECTS)
