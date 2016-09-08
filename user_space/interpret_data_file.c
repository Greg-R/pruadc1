#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUFFER_SIZE		512
uint16_t readBuf[MAX_BUFFER_SIZE];

int main(void)
{
	uint16_t result = 0;
        FILE* rawdatafile;

        rawdatafile = fopen("prudata", "r");


	for (int i = 0; i < 500; i++) {
           fread(&result, 2, 1, rawdatafile);
         printf("Message %d received from PRU:%" PRIu16 "\n", i, result);
	}

	/* Received all the messages the example is complete */
	printf("All data is printed.\n");

	/* Close the pru data file */
	fclose(rawdatafile);

	return 0;
}

