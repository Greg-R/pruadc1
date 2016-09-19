 // create_sinusoidal_pcm_file.c
 // Copyright 2016 Greg
 // Generate sinusoidal PCM data and write to a named pipe.
 // Play the data from the pipe using ALSA aplay via a
 // fork and execvp.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
//	int8_t makefifo;
	int soundfifo, pru_data;
	pid_t forkit;
	//  Open a file in write mode.
        int16_t sinebuf[256];
	
	//  Create a named pipe.
//	makefifo = mkfifo("soundfifo", O_RDWR);
//	if(makefifo < 0) printf("Named pipe soundfifo creation failed.");
	
	//  The sample rate is 44100 samples per second.
	//  Compute delta-t, a single sample interval.

        //  Split into two processes.
        //  The parent will consume the data.
        //  The child will generate data and write to a named pipe.
	forkit = fork();
	
	if(!forkit) {  //  This is the parent process.
		int result;
        char *arguments[] = {"aplay", "--format=S16_LE", "-Dplughw:1,0", "--rate=44100", "soundfifo", NULL};
        result = execvp("aplay", arguments);
        printf("The return value of execvp(aplay) is %d.\n", result);
	}
	else if (forkit > 0) {  //  This is the child process.
	
	double deltaT = 2.0 * M_PI/(44100.0/440.0);
	double sinOperand = 0.0;
	
        //  Now, in the child process, open the PRU character device.
        //  Read data from it in chunks and write to the named pipe.

        int count = 0;
	int16_t pcm_int_sample = 0;
	soundfifo = open("soundfifo", O_RDWR);  // Open named pipe.
	pru_data  = open("/dev/rpmsg_pru30", O_RDWR);  // Open char device.
		for(int i=0; i < 20000000; i++) {
                read(pru_data, sinebuf, 512);
		write(soundfifo, sinebuf, 512);
	}
	}

else if (forkit == -1) perror("fork error");
}


