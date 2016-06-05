#include "resource_table_empty.h"
#include <stdint.h>
#include <pru_cfg.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Global variables?; why?
unsigned int i;                  // the counter in the time delay
unsigned int delay = 9988260;     // the delay (manually determined)

int main(void){

CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  //  Include pru_cfg.h

// while(1){
 for(int i = 0; i < 1000; i = i + 1) {
 __R30 = __R30 | 1 << 3; // config-pin P8.44 pruout 
//     for(i=0; i< delay; i++) {}     // sleep for the delay
 __delay_cycles(200000000);
       __R30 = __R30 & 0 << 3;         // turn off the LED r30.5
      for(i=0; i < 1; i++) {     // sleep for the delay
 __delay_cycles(200000000);
 }
   }
}
