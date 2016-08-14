//  pru0singleChar.c
//  This file sends a single Char to the ARM.
//  This is part of a series of experiments to
//  better understand PRU<->ARM communication
//  with RPMsg.

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
uint16_t payload1[30];
//uint8_t payload2[30];
uint8_t payload[20];

#define PRU_SHAREDMEM 0x00010000
  volatile register uint32_t __R30;
  volatile register uint32_t __R31;

 int main(void){
    struct pru_rpmsg_transport transport;
    uint16_t src, dst, len, data;
    volatile uint8_t *status;

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

while(pru_rpmsg_receive(&transport, &src, &dst, payload, &len) != PRU_RPMSG_SUCCESS){};

data = 0;
for(uint16_t i=0; i < 10; i = i + 1) {
   __delay_cycles(100000000);
//  Pack data into 10 sample chunks and send.
   for(int i=0; i < 10; i = i + 1) {
   payload1[i] = i + data;
}
   data = data + 10;
   pru_rpmsg_send(&transport, dst, src, payload1, 20);
}
//   __halt();                        // halt the PRU
}
