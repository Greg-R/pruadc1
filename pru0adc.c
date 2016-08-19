//  pru0adc.c
//  Attempt to duplicate Derek Molloy's
//  SPI ADC read program in C from assembly.
//  Chip Select:  P9.27 pr1_pru0_pru_r30_5
//  MOSI:         P9.29 pr1_pru0_pru_r30_1
//  MISO:         P9.28 pr1_pru0_pru_r31_3
//  SPI CLK:      P9.30 pr1_pru0_pru_r30_2
//  Sample Clock: P8.46 pr1_pru1_pru_r30_1  (testing only)

#include <stdint.h>
#include <stdio.h>
#include <am335x/pru_intc.h>
#include <am335x/pru_cfg.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "resource_table_0.h"

// Define remoteproc related variables.
#define HOST_INT ((uint32_t) 1 << 30)

//  The PRU-ICSS system events used for RPMsg are defined in the Linux device tree.
//  PRU0 uses system event 16 (to ARM) and 17 (from ARM)
//  PRU1 uses system event 18 (to ARM) and 19 (from ARM)
#define TO_ARM_HOST    16
#define FROM_ARM_HOST  17

//  Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
//  at linux-x.y.x/drivers/rpmsg_pru.c
#define CHAN_NAME    "rpmsg-pru"
#define CHAN_DESC    "Channel 30"
#define CHAN_PORT    30

//  Used to make sure the Linux drivers are ready for RPMsg communication
//  Found at linux-x.y.z/include/uapi/linux/virtio_config.h
#define VIRTIO_CONFIG_S_DRIVER_OK  4

//  Buffer used for PRU to ARM communication.
uint16_t payload[RPMSG_BUF_SIZE];

#define PRU_SHAREDMEM 0x00010000
  volatile register uint32_t __R30;
  volatile register uint32_t __R31;
  uint32_t spiCommand;
  uint32_t numSamples = 1000000;  // Number of samples

 int main(void){
    struct pru_rpmsg_transport transport;
    uint16_t src, dst, len;
    volatile uint8_t *status;
//    uint16_t payloadOut[100];

//  1.  Enable OCP Master Port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
//  Clear the status of PRU-ICSS system event that the armwill use to 'kick' us.
  CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

//  Make sure the drivers are ready for RPMsg communication:
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

//  Initialize pru_virtqueue corresponding to vring0 (PRU to ARM Host direction).
  pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

// Create the RPMsg channel between the PRU and the ARM user space using the transport structure.
  while(pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
//  The above code should cause an RPMsg character to device to appear in the directory /dev.  
//  The following is a test loop.  Comment this out for normal operation.

/*
while(1) {
       if (__R31 & HOST_INT) {  // Clear the event status.
         CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
         //  Receive all available messages, multiple messages can be sent per kick.
        while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS) {
    uint16_t payloadOut[1] = {123};
//    len = 2;  //  This is the length of payloadOut in bytes.
        //  Echo the message back to the same address from which we just received.
               pru_rpmsg_send(&transport, dst, src, payloadOut, len);
         }}}
*/
//  This section of code blocks until a message is received from ARM.
//  This is done to initialize the RPMSG communication.
//       if (__R31 & HOST_INT) {  // The interrupt from the ARM host. 
//         CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;  // Clear the event status.
//        }
         //  Receive all available messages, multiple messages can be sent per kick.
    while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) != PRU_RPMSG_SUCCESS) {}

//  2.  Initialization
//   uint32_t bitMask; //    = 0x000003FF;  //  Keep only 10 bits using this mask.

   uint32_t data = 0x00000000;  // Incoming data stored here.
//  The data out line is connected to R30 bit 1.
  __R30 = 0x00000000;  //  Clear the output pin.
//  The sample clock is located at shared memory address 0x00010000.
//  Need a pointer for this address.  This is found in the linker file.
//  The address 0x0001_000 is PRU_SHAREDMEM.
   uint32_t * clockPointer = (uint32_t *) 0x00010000;
   __R30 = __R30 | (1 << 5);  // Initialize chip select HIGH.
   __delay_cycles(100000000);  //  Allow chip to stabilize.
//  3.  SPI Data capture loop.  This captures numSamples data samples from the ADC.
   uint8_t dataCounter = 0;  // Used to load data transmission buffer payloadOut;

for(int i = 0; i < numSamples; i = i + 1) {  //  Outer loop.  This determines # samples.
 while(!(*clockPointer)){}  //  Hold until the Master clock from PRU1 goes high.

//  spiCommand is the MOSI preamble; must be reset for each sample.
   spiCommand = 0x80000000;  // Single-ended Channel 0
   data = 0x00000000;  //  Initialize data.
   __R30 = __R30 & 0xFFFFFFDF;  //  Chip select to LOW P9.27
   __R30 = __R30 & 0xFFFFFFFB;  //  Clock to LOW   P9.30
//  Start-bit is HIGH.  The Start-bit is manually clocked in here.
   __R30 = __R30 | (1 << 1);  //  Set a 0x10 on MOSI (the Start-bit)
   __delay_cycles(500);  //  Delay to allow settling.
   __R30 = __R30 | 0x00000004;  //  Clock goes high; latch in start bit.
   __delay_cycles(500);  //  Delay to allow settling.
   __R30 = __R30 & 0xFFFFFFFB;  //  Clock to LOW   P9.30
// The Start-bit cycle is completed.

//  Get 24 bits from the data line MISO R31.t3 (bit 3)
   for(int i = 0; i < 16; i = i + 1) {  //  Inner single sample loop

//  The first action will be to transmit on MOSI by shifting out spiCommand variable.
    if(spiCommand >> 31) //  If the MSB is 1 
    __R30 = __R30 | (1 << 1);  //  Set a 0x10 on MOSI
    else 
    __R30 = __R30 & (0xFFFFFFFD);  //  All 1s except bit 1
    spiCommand = spiCommand << 1;  // Shift left one bit (for next cycle).
   __delay_cycles(500);
   __R30 = __R30 | 0x00000004;  //  Clock goes high; bit set on MOSI.
   __delay_cycles(500);  //  Delay to allow settling.
    
//  The data needs to be "shifted" into the data variable.
    data = data << 1;  // Shift left; insert 0 at lsb.
   __R30 = __R30 & 0xFFFFFFFB;  //  Clock to LOW   P9.30
//   __delay_cycles(2500);  //  Delay to allow settling.

    if(__R31 & (1 << 3)) //  Probe MISO data from ADC.
    data = data | 1;
    else
    data = data & 0xFFFFFFFE;
  }  //  End of 24 cycle loop
   __R30 = __R30 | 1 << 5;  //  Chip select to HIGH

//  Send frames of 100 samples.   
    payload[dataCounter] = (int16_t) data - 512;
//      payload[dataCounter] = 299;
if(dataCounter == 199){
   pru_rpmsg_send(&transport, dst, src, payload, 400);
   dataCounter = 0;
}
   dataCounter = dataCounter + 1;
}//  End data acquisition loop.

//   __R31 = 35;                      // PRUEVENT_0 on PRU0_R31_VEC_VALID
   __halt();                        // halt the PRU
}
