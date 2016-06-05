//  This code runs in PRU1.
//  The clock output test pin is P8.44
#include "resource_table_empty.h"
#include <stdint.h>

#define PRU_SHAREDMEM 0x00010000

volatile register uint32_t __R30;

 int main(void){

 __R30 = 0;  //  Clear the output pin.

//  The sample clock is located at shared memory address 0x00010000.
//  Need a pointer for this address.  This is found in the linker file.
//  The address 0x0001_000 is PRU_SHAREDMEM.
 uint32_t * clockPointer = (uint32_t *) 0x00010000;
 *clockPointer = 0;  //  Clear this memory location.

//  The LED is connected to P8.44 which is bit 3 of R30.
//  Wait a bunch of clock cycles, then write 1 to clockPointer and blink LED:
 for(int i = 0; i < 20000; i = i + 0 ) {
  __R30 = __R30 | (1 << 3);
  *clockPointer = 1;
  __delay_cycles(10000);  //  Human perceivable delay.
  *clockPointer = 0;
  __R30 = __R30 & (0 << 3);
  __delay_cycles(140000);  //  Human perceivable delay.
  }
/*
  *clockPointer = 0;
  if(*clockPointer == 0) {
  __R30 = 0;
  }
*/
   __halt();                        // halt the PRU
 }
