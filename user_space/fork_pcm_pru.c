// create_sinusoidal_pcm_file.c
// Copyright 2016 Greg
// Generate sinusoidal PCM data and write to a named pipe.
// Play the data from the pipe using ALSA aplay via a
// fork and execvp.
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

#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int soundfifo, pru_data, pru_clock; // file descriptors
  pid_t forkit;
  //  Open a file in write mode.
  uint8_t sinebuf[490];

  //  Split into two processes.
  //  The parent will consume the data.
  //  The child will generate data and write to a named pipe.
  forkit = fork();

  if (!forkit) { //  This is the child process.
    int result;
    char *arguments[] = {"aplay",       "--format=S16_LE", "-Dplughw:1,0",
                         "--rate=8000", "soundfifo",       NULL};
    result = execvp("aplay", arguments);
    printf("The return value of execvp(aplay) is %d.\n", result);
  } else if (forkit > 0) { //  This is the parent process.

    //  Now, open the PRU character device.
    //  Read data from it in chunks and write to the named pipe.
    ssize_t readpru, writepipe, prime_char, pru_clock_command;
    soundfifo = open("soundfifo", O_RDWR); // Open named pipe.
    if (soundfifo < 0)
      printf("Failed to open soundfifo.");
    //  Open the character device to PRU0.
    pru_data = open("/dev/rpmsg_pru30", O_RDWR);
    if (pru_data < 0)
      printf("Failed to open pru character device rpmsg_pru30.");
    //  The character device must be "primed".
    prime_char = write(pru_data, "prime", 6);
    if (prime_char < 0)
      printf("Failed to prime the PRU0 char device.");
    //  Now open the PRU1 clock control char device and start the clock.
    pru_clock = open("/dev/rpmsg_pru31", O_RDWR);
    pru_clock_command = write(pru_clock, "g", 2);
    if (pru_clock_command < 0)
      printf("The pru clock start command failed.");
    //  This is the main data transfer loop.
    //  Note that the number of transfers is finite.
    //  This can be changed to a while(1) to run forever.
    for (int i = 0; i < 20000000; i++) {
      readpru = read(pru_data, sinebuf, 490);
      writepipe = write(soundfifo, sinebuf, 490);
    }
    printf("The last value of readpru is %d and the last value of writepipe is "
           "%d.\n",
           readpru, writepipe);
  } else if (forkit == -1)
    perror("fork error");
}
