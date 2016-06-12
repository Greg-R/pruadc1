#include <stdint.h>
#include <stdio.h>
#include <am335x/pru_cfg.h>
#include <am335x/pru_intc.h>
#include <rsc_types.h>
#include <pru_virtqueue.h>
#include <pru_rpmsg.h>
#include <am335x/sys_mailbox.h>
#include "resource_table_1.h"

volatile register uint32_t __R31;
volatile register uint32_t __R30;

/* PRU1 is mailbox module user 2 */
#define MB_USER						2
/* Mbox0 - mail_u2_irq (mailbox interrupt for PRU1) is Int Number 59 */
#define MB_INT_NUMBER				59

/* Host-1 Interrupt sets bit 31 in register R31 */
#define HOST_INT					0x80000000

/* The mailboxes used for RPMsg are defined in the Linux device tree
 * PRU0 uses mailboxes 2 (From ARM) and 3 (To ARM)
 * PRU1 uses mailboxes 4 (From ARM) and 5 (To ARM)
 */
#define MB_TO_ARM_HOST				5
#define MB_FROM_ARM_HOST			4

 // Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 // at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c

#define CHAN_NAME					"rpmsg-pru"
#define CHAN_DESC					"Channel 31"
#define CHAN_PORT					31

/* 
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
        #define VIRTIO_CONFIG_S_DRIVER_OK	4

        uint8_t payload[RPMSG_BUF_SIZE];
        uint8_t payloadOut[RPMSG_BUF_SIZE];

	struct pru_rpmsg_transport transport;
	uint16_t src, dst, len;
	volatile uint8_t *status;

//        char* test1 = "bullshit";
//        memcpy(payloadOut, test1, 8*sizeof(char));

	/* allow OCP master port access by the PRU so the PRU can read external memories */
//	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* clear the status of event MB_INT_NUMBER (the mailbox event) and enable the mailbox event */
//	CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;
//	CT_MBX.IRQ[MB_USER].ENABLE_SET |= 1 << (MB_FROM_ARM_HOST * 2);

	/* Make sure the Linux drivers are ready for RPMsg communication */
//	status = &resourceTable.rpmsg_vdev.status;
//	while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

	/* Initialize pru_virtqueue corresponding to vring0 (PRU to ARM Host direction) */
//	pru_virtqueue_init(&transport.virtqueue0, &resourceTable.rpmsg_vring0, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

	/* Initialize pru_virtqueue corresponding to vring1 (ARM Host to PRU direction) */
//	pru_virtqueue_init(&transport.virtqueue1, &resourceTable.rpmsg_vring1, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

  //      __R30 = 0;  //  Turn off the LED.

	/* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
//	while(pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
	
	//pru_rpmsg_send(&transport, dst, src, payload, len);
