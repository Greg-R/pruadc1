/*
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
//#include <sys/poll.h>

#define MAX_BUFFER_SIZE	1024	
uint16_t readBuf[MAX_BUFFER_SIZE];
//uint16_t readBuf;

#define NUM_MESSAGES		1
#define DEVICE_NAME		"/dev/rpmsg_pru30"

int main(void)
{
//	struct pollfd pollfds[1];
        int fd1; // File descriptor for the character file.
	int i;
	int result = 0;
        uint16_t sendPayload[1] = {29};

	/* Open the rpmsg_pru character device file */
	fd1 = open(DEVICE_NAME, O_RDWR);

	/*
	 * If the RPMsg channel doesn't exist yet the character device
	 * won't either.
	 * Make sure the PRU firmware is loaded and that the rpmsg_pru
	 * module is inserted.
	 */
	if (fd1 < 0) {
		printf("Failed to open %s\n", DEVICE_NAME);
		return -1;
	}

	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s\n\n", DEVICE_NAME);

        result = read(fd1, readBuf, MAX_BUFFER_SIZE);
        if (result > 0)
         for(int i=0; i < 1024/4; i=i+1){
	     printf("Message %d received from PRU: %" PRIu16 "\n", i, readBuf[i]);
        }

/*
        result = read(fd1, readBuf, MAX_BUFFER_SIZE);
        if (result > 0)
         for(int i=0; i < 1024/4; i=i+4){
	     printf("Message %d received from PRU: %" PRIu16 " %" PRIu16 " %" PRIu16 " %" PRIu16 "\n", i, readBuf[i], readBuf[i+1], readBuf[i+2], readBuf[i+3]);
        }
        result = read(fd1, readBuf, MAX_BUFFER_SIZE);
        if (result > 0)
         for(int i=0; i < 1024/4; i=i+4){
	     printf("Message %d received from PRU: %" PRIu16 " %" PRIu16 " %" PRIu16 " %" PRIu16 "\n", i, readBuf[i], readBuf[i+1], readBuf[i+2], readBuf[i+3]);
        }
*/

	close(fd1);
	return 0;
}

