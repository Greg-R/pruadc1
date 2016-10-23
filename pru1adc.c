//  This code runs in PRU1.
//  The clock output test pin is P8.30
//  The clock is started by writing a code
//  to a device driver opened by this file.
//  Copyright (C) 2016  Gregory Raven
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "resource_table_1.h"
#include <am335x/pru_cfg.h>
#include <am335x/pru_intc.h>
#include <pru_rpmsg.h>
#include <rsc_types.h>
#include <stdint.h>
#include <stdio.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Host-0 Interrupt sets bit 30 in register R31
#define HOST_INT ((uint32_t)1 << 30)

// The PRU-ICSS system events used for RPMsg are defined in the Linux device
// tree
// PRU0 uses system event 16 (To ARM) and 17 (From ARM)
// PRU1 uses system event 18 (To ARM) and 19 (From ARM)
#define TO_ARM_HOST 18
#define FROM_ARM_HOST 19

// Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
// at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
// Use Channel 31 for the device driver.
#define CHAN_NAME "rpmsg-pru"
#define CHAN_DESC "Channel 31"
#define CHAN_PORT 31

// Used to make sure the Linux drivers are ready for RPMsg communication
// Found at linux-x.y.z/include/uapi/linux/virtio_config.h
#define VIRTIO_CONFIG_S_DRIVER_OK 4

uint8_t payload[RPMSG_BUF_SIZE];

#define PRU_SHAREDMEM 0x00010000

int main(void) {
  __R30 = 0; //  Clear the output pin.
  struct pru_rpmsg_transport transport;
  uint16_t src, dst, len;
  volatile uint8_t *status;

  // Allow OCP master port access by the PRU so the PRU can read external
  // memories.
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Clear the status of the PRU-ICSS system event that the ARM will use to
  // 'kick' us.
  CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

  // Make sure the Linux drivers are ready for RPMsg communication.
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK))
    ;

  // Initialize the RPMsg transport structure.
  pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0,
                 &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);
  //  Block until the channel is established.
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC,
                           CHAN_PORT) != PRU_RPMSG_SUCCESS)
    ;
  //  The following code looks for a specific incoming message via RPMsg.
  //  This code blocks until the message is successfully received.
  //  If the correct message is received, the clock is allowed to begin.
  while (1) {
    /* Check bit 30 of register R31 to see if the ARM has kicked us */
    if (__R31 & HOST_INT) {
      /* Clear the event status */
      CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
      /* Receive all available messages, multiple messages can be sent per kick
       */
      if (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) ==
          PRU_RPMSG_SUCCESS) {
        if (payload[0] == 'g')
          break;
      }
    }
  }

  //  The following is the code for the clock which is used to drive the SPI in
  //  PRU0.  The clock rate is approximately 8 kHz.

  //  The sample clock is located at shared memory address 0x00010000.
  //  Need a pointer for this address.  This is found in the linker file.
  //  The address 0x0001_000 is PRU_SHAREDMEM.
  uint32_t *clockPointer = (uint32_t *)0x00010000;
  *clockPointer = 0; //  Clear this memory location.

  while (1) {
    __R30 = __R30 | (1 << 11); // P9.30
    *clockPointer = 7;
    __delay_cycles(1000); 
    *clockPointer = 0;
    __R30 = __R30 & (0 << 11);
    //  The following delay will set the clock rate.
    //  This delay was originally 24000 cycles; it was reduced due to ALSA underruns.
    __delay_cycles(23980); 
  }
}
