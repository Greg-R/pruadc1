//  pruSimData.c
//  Create simulated data and write to the ARM.

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
  uint32_t numSamples = 10;  // Number of samples

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

//  This section of code blocks until a message is received from ARM.
//  This is done to initialize the RPMSG communication.
//       if (__R31 & HOST_INT) {  // The interrupt from the ARM host. 
//         CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;  // Clear the event status.
//        }
         //  Receive all available messages, multiple messages can be sent per kick.
    while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) != PRU_RPMSG_SUCCESS) {}

   uint16_t data = 0x00000000;  // Incoming data stored here.
   uint8_t dataCounter = 0;  // Used to load data transmission buffer payloadOut;

for(uint16_t i = 0; i < numSamples; i = i + 1) {  //  Outer loop.  This determines # samples.

    data = i;

//  Send frames of 100 samples.   
    payload[dataCounter] = (uint16_t) data;
    dataCounter += 1;

if(dataCounter == 99){
   pru_rpmsg_send(&transport, dst, src, payload, 200);
//        while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) != PRU_RPMSG_SUCCESS) {}
   dataCounter = 0;
         CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
}
}//  End data acquisition loop.

//   __R31 = 35;                      // PRUEVENT_0 on PRU0_R31_VEC_VALID
   __halt();                        // halt the PRU
}
