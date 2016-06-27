/*
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>

#define MAX_BUFFER_SIZE		512
uint16_t readBuf[MAX_BUFFER_SIZE];
//uint16_t readBuf;

#define NUM_MESSAGES		1
#define DEVICE_NAME		"/dev/rpmsg_pru30"

int main(void)
{
	struct pollfd pollfds[1];
	int i;
	int result = 0;
        uint16_t sendPayload[1] = {29};

	/* Open the rpmsg_pru character device file */
	pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

	/*
	 * If the RPMsg channel doesn't exist yet the character device
	 * won't either.
	 * Make sure the PRU firmware is loaded and that the rpmsg_pru
	 * module is inserted.
	 */
	if (pollfds[0].fd < 0) {
		printf("Failed to open %s\n", DEVICE_NAME);
		return -1;
	}

	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s, sending %d messages\n\n", DEVICE_NAME, NUM_MESSAGES);

	for (i = 0; i < NUM_MESSAGES; i++) {
		/* Send 'hello world!' to the PRU through the RPMsg channel */
	//	result = write(pollfds[0].fd, "hello world!", 13);
		result = write(pollfds[0].fd, sendPayload, 2);
		if (result > 0)
			printf("Message %d: Sent to PRU\n", i);

		/* Poll until we receive a message from the PRU and then print it */
		result = read(pollfds[0].fd, readBuf, MAX_BUFFER_SIZE);
		if (result > 0)
                   for(int i=0; i < 100; i++){
			printf("Message %d received from PRU:%" PRIu16 "\n", i, readBuf[i]);
                       }
	}

	/* Received all the messages the example is complete */
	printf("Received %d messages, closing %s\n", NUM_MESSAGES, DEVICE_NAME);

	/* Close the rpmsg_pru character device file */
	close(pollfds[0].fd);

	return 0;
}

