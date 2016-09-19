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
	int soundfifo;
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
	
        //  Next, create a raw data with signed integers instead of double.

        int count = 0;
	int16_t pcm_int_sample = 0;
	soundfifo = open("soundfifo", O_RDWR);  // Open named pipe.
	
		for(int i=0; i < 20000000; i++) {
		pcm_int_sample = sin(sinOperand) * pow(2, 16) * 0.2999;
		sinebuf[count] = pcm_int_sample;		
        if(count == 255) {
		write(soundfifo, sinebuf, 512);
//		fwrite(sinebuf, 2, 512, pcm_int_data); // Write file for testing.
		count = -1;
	}
		count += 1;  // Increment the sample counter.
		//  Increment the sine operand.
		sinOperand += deltaT;
	}
}
else if (forkit == -1) perror("fork error");
}


