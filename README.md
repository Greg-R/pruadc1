# pruadc1
Beaglebone Green and PRU "Programmable Real-Time Unit" Interface to ADC using RemoteProc and RPMsg Framework

This project is based on the project described by Derek Molloy at his website "Exploring BeagleBone":

http://exploringbeaglebone.com/chapter13/

The particular project is not described in his book, but is supplementary material available on the website only.
The project title is "High-Speed Analog to Digital Conversion (ADC) using the PRU-ICSS
The MCP3008 8-Channel 10-bit ADC".

The goal of this project is to perform real-time digitization of an analog audio waveform, and then to make the digital
data-stream available for manipulation within the Linux user-space via the "Advanced Linux Sound Architecture".
The Analog-to-Digital Converter (ADC) is a type MCP3008 and is connected via SPI bus to one of the two "Programmable
Real-Time Units" included with the Beaglebone system.

The Beaglebone series of development boards with AM335X Sitara System-On-Chip contain two "Programmable Real-Time Units".
This is in addition to the primary 32-bit ARM processor on the AM335X which hosts the Debian GNU/Linux operating system.

These PRUs are intended targetted at "real-time" applications where predictable response is required.
Thus the ARM processor hosts the non-preemptive GNU/Linux OS while real-time tasks are farmed out to the PRUs.

Texas Instruments has published extensive information and documentation on these devices here:

http://processors.wiki.ti.com/index.php/PRU-ICSS

TI has published a "PRU Support Package" here:

https://git.ti.com/pru-software-support-package

The above software package plus Derek Molloy's project were the two primary sources of inspiration for this project.

The "RemoteProc" and "RemoteProc Messaging" frameworks were used for firmware management and user-space access of
the PRUs.  These frameworks are still in the experimental stage, however, TI's examples provided in the PRU
Support Package mentioned above were sufficient to complete implementation of the project.

Derek Molloy's project used the "PASM" assembler to code the SPI bus firmware for the PRUs.  This project uses a 
C compiler to implement the SPI bus.  The C code follows the general pattern of Derek's assembly code, but it is
probably not quite as efficient.  Even so, there were sufficient resources on the PRUs without resorting to
optimization.

The TI PRU C compiler documentation is located here:

http://www.ti.com/lit/ug/spruhv7a/spruhv7a.pdf

A quick video describing the project is at youtube:

https://youtu.be/i-ZcPAvkQVU

Here is a second video (covering the same, a little more formal):

https://youtu.be/FzmRYEzsuLA

Regards,
Greg
