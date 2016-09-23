#    Makefile for PRU ADC Project
#    Copyright (C) 2016  Gregory Raven
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

INCLUDE_PATHS=-I$(PRU_CGT)/includeSupportPackage -I$(PRU_CGT)/include -I$(PRU_CGT)/lib
CFLAGS=$(INCLUDE_PATHS) --hardware_mac=on --c99
LNK_CMDS1=-z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include -i$(PRU_CGT)/includeSupportPackage
LNK_CMDS2=-i$(PRU_CGT)/includeSupportPackage/am335x --reread_libs --stack_size=0x100
LNK_CMDS3=--heap_size=0x100 --library=$(PRU_CGT)/lib/rpmsg_lib.lib

all: $(SOURCES)
	clpru $(CFLAGS) pru0adc.c $(LNK_CMDS1) $(LNK_CMDS2) $(LNK_CMDS3) ./AM335x_PRU.cmd -o ./result/am335x-pru0-fw --library=libc.a 
	clpru $(CFLAGS) pru1adc.c $(LNK_CMDS1) $(LNK_CMDS2) $(LNK_CMDS3) ./AM335x_PRU.cmd -o ./result/am335x-pru1-fw --library=libc.a 
	gcc -std=c99 ./user_space/fork_pcm_pru.c -o ./user_space/fork_pcm_pru
	cd result
	cp ./result/am335x-pru0-fw /lib/firmware
	cp ./result/am335x-pru1-fw /lib/firmware
	mkfifo ./user_space/soundfifo
	prumodout
	prumodin

clean:
	rm ./user_space/soundfifo

