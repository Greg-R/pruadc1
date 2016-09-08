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

#define NUM_MESSAGES		1
#define DEVICE_NAME		"/dev/rpmsg_pru30"

int main(void)
{
//	struct pollfd pollfds[1];
        int fd1; // File descriptor for the character file.
	int i, dataIndex;
	int result = 0;
        uint16_t sendPayload[1] = {29};
        FILE* prudata = fopen("prudata.txt", "w");

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
       
         for(int i=0; i < 1000; i=i+1){
             result = read(fd1, readBuf, 200);
        fwrite(readBuf, 2, 100, prudata);
        }

        fclose(prudata);
	close(fd1);
	return 0;
}

