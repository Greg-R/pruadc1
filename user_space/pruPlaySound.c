//  User space program to take audio data
//  emitted by the PRU and pipe into ALSA.

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>

//  The format of the audio data is signed 16 bit little endian.
//  The ALSA format code is S16_LE.
#define MAX_BUFFER_SIZE		256
uint16_t readBuf[MAX_BUFFER_SIZE];


//  The following are RPMsg PRU character devices.
#define PRU0_DEVICE	"/dev/rpmsg_pru30"
#define PRU1_DEVICE     "/dev/rpmsg_pru31"

int main(void)
{
	int pru0_fd, pru1_fd;
        int soundfifo;
	int result = 0;

	// Open the PRU0 character device file.
	pru0_fd = open(PRU0_DEVICE, O_RDWR);

	 // If the RPMsg channel doesn't exist yet the character device
	 // won't either.
	 // Make sure the PRU firmware is loaded and that the rpmsg_pru
	 // module is inserted.
	 // Add code here which uses prumodout prumodin scripts.
	if (pru0_fd < 0) {
		printf("Failed to open %s\n", PRU0_DEVICE);
		return -1;
	}

	// The RPMsg channel exists and the character device is opened.
	printf("Successfully opened %s RPMsg character device.\n\n", PRU0_DEVICE);

        // "Prime" the RPMsg channel by writing to the character device.
	result = write(pru0_fd, "prime", 6);
		if (result > 0)
			printf("PRU0 character device successfully primed.\n");
        
        //  Open the PRU1 character device file.
        pru1_fd = open(PRU1_DEVICE, O_RDWR);

	if (pru1_fd < 0) {
		printf("Failed to open %s\n", PRU0_DEVICE);
                return -1;
        }

        //  Start the clock pulses from PRU1.
        result = write(pru1_fd, "g", 2);
        if (result > 0)
            printf("PRU1 clock signal successfully written.\n");
        close(pru1_fd);  //  May need to leave this open if other control features added.

        //  Now create a named pipe.  This will be used
        //  to buffer the data between the PRU character
        //  device and ALSA.  It may be possible to operate
        //  without this pipe, but this is how the first
        //  successful sound was played via the command line.

        soundfifo = mkfifo("soundfifo", O_RDWR);        
        if(soundfifo < 0) printf("Creation of soundfifo named pipe failed.\n");
        
        //  Create a process running ALSA aplay reading data from soundfifo.
        //fork();
        char *arguments[] = {"aplay", "--format=S16_LE", "-Dplughw:1,0", "soundfifo", NULL};
        result = execv("/usr/bin/aplay", arguments);
        printf("The value returned by execve(aplay) is %d.\n", result);

	close(pru0_fd);
	return 0;
}

