# Makefile for PRU ADC

INCLUDE_PATHS=-I$(PRU_CGT)/includeSupportPackage -I$(PRU_CGT)/include -I$(PRU_CGT)/lib
CFLAGS=$(INCLUDE_PATHS) --hardware_mac=on --c99
LNK_CMDS1=-z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include -i$(PRU_CGT)/includeSupportPackage
LNK_CMDS2=-i$(PRU_CGT)/includeSupportPackage/am335x --reread_libs --stack_size=0x100
LNK_CMDS3=--heap_size=0x100 --library=$(PRU_CGT)/lib/rpmsg_lib.lib

#SOURCES=$(wildcard *.c)
#OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

#SOURCES=pru0adc.c

all: $(SOURCES)
	clpru $(CFLAGS) pru0adc.c $(LNK_CMDS1) $(LNK_CMDS2) $(LNK_CMDS3) ./AM335x_PRU.cmd -o am335x-pru0-fw --library=libc.a 
	clpru $(CFLAGS) pru1adc.c $(LNK_CMDS1) $(LNK_CMDS2) $(LNK_CMDS3) ./AM335x_PRU.cmd -o am335x-pru1-fw --library=libc.a 
	cp am335x-pru0-fw ./result
	cp am335x-pru1-fw ./result
	cd result
	cp am335x-pru0-fw /lib/firmware
	cp am335x-pru1-fw /lib/firmware
	prumodout
	prumodin

clean:
	rm -rf $(OBJECTS)
