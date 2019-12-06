/*
 * ecg_test.c
 *
 * Simple test of ECG protocol
 * Parameters:
 *   -r     Runs as receiver (default)
 *   -s     Runs as sender
 *   -ra p  Set recevier address to p (port)
 *   -sa p  Set sender address to p (port)
 */


// Uses
#include "ecg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEND_TIMEOUT_SEC       	5
#define RECV_TIMEOUT_SEC       10
#define SEND_BUF_SIZE        1024
#define RECV_BUF_SIZE         256

int is_sender = 0;
int snd_addr = 2132;
int rcv_addr = 2135;

void read_args(int argc, char * argv[]) {
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) { is_sender = 0;  continue; }
        if (strcmp(argv[i], "-s") == 0) { is_sender = 1;  continue; }
        if (strcmp(argv[i], "-sa") == 0 && i < argc - 1) {
            snd_addr = atoi(argv[++i]);
            continue;
        }
        if (strcmp(argv[i], "-ra") == 0 && i < argc - 1) {
            rcv_addr = atoi(argv[++i]);
            continue;
        }
        printf("Unknown option %s\n", argv[i]);
    }
}


int sendFile(){
	FILE *file = fopen("Rpeak.txt","r");
	char    *buffer;

	int err;

	fseek(file, 0, SEEK_END);
	int length = ftell(file);

	fseek(file, 0, SEEK_SET);
	buffer = (char*)calloc(length, sizeof(char));

	fread(buffer, sizeof(char), length, file);

	fclose(file);

	printf("Acting as sender with address %d\n", snd_addr);

	if ((err=ecg_init(snd_addr)) != ERR_OK) {
		printf("Protocol could not be initialized: %d\n", err);
	    return 1;
	}

	printf("Protocol init initialized\n");
	err = ecg_send(rcv_addr, buffer, strlen(buffer), SEND_TIMEOUT_SEC * 1000);
	/*
	while (1) {

    if (err == ERR_TIMEOUT) {
           printf("ecg_recv timed out\n");
           continue;
       }
       printf("ecg_recv failed with %d\n", err);
       continue;
	}

		buffer[err] = '\0';
   		printf("Received %d bytes from address %d: %s\n", len, source, buf);
	}
	*/

	free(buffer);
	return 1;
}

int writeFile(){
	FILE *file = fopen("netFile.txt","w");
	char    *buffer = malloc(2000);
	int err, source;

	printf("Acting as receiver with address %d\n", rcv_addr);

	if ((err=ecg_init(rcv_addr)) != ERR_OK) {
		printf("Protocol could not be initialized: %d\n", err);
		return 1;
	}

	 printf("Protocol init initialized\n");

	 while((err = ecg_recv(&source, buffer, sizeof(buffer), RECV_TIMEOUT_SEC * 1000)) != -3){}

	fputs(buffer,file);

	fclose(file);
	free(buffer);
	return 1;
}


int main(int argc, char * argv[]) {

    read_args(argc, argv);

    return is_sender ? sendFile() :  writeFile();

}
