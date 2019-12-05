/*
 * radio.c
 *
 * Emulation of radio node using UDP (skeleton)
 */

// Implements
#include "radio.h"

// Uses
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
int sock;    // UDP Socket used by this node

typedef struct {
	char preamble[10];
	char key[4];
	char PI;
	char checksum[2];
	char str[DATA_SIZE];
}frame_header_t;

typedef union {
	char raw[FRAME_PAYLOAD_SIZE];

	frame_header_t head;
}frame_packet_t;



int radio_init(int addr) {

    struct sockaddr_in sa;   // Structure to set own address
    memset(&sa, 0, sizeof(sa));

    // Check validity of address
    if (addr < 0 || addr > 65535) {
    	return ERR_INVAL;
    }

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    	return ERR_FAILED;
    }

    // Prepare address structure
    sa.sin_family = AF_INET;
    sa.sin_port = htons(addr);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to port
    if (bind(sock, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
    	return -4;
    }

    return ERR_OK;
}

int radio_send(int dst, char* data, int len) {

	struct sockaddr_in sa;   // Structure to hold destination address
	frame_packet_t buf;

	int slen;

    // Check that port and len are valid
    if (dst < 0 || dst > 65535 || len < 0 || len > DATA_SIZE) {
    	return ERR_INVAL;
    }


    memset(buf.head.str, 0, sizeof(char)*len);
    memset(buf.head.preamble, 170, sizeof(char)*10);
    memset(buf.head.key, 199, sizeof(char)*4);
    buf.head.PI = 201;
    memset(buf.head.checksum, 0, sizeof(char)*2);

    short p = checkSum(data);
    //check if checksum works
    //printf("checksum before split: %p\n", p);
    buf.head.checksum[0] = p;
    p = p>>8;
    buf.head.checksum[1] = p;
    //check this too when checking checksum
    /*
    printf("Checksum[0]: %p\n", buf.head.checksum[0]);
    printf("Checksum[1]: %p\n", buf.head.checksum[1]);
	*/

    //buf.head.checksum = checkSum(data);

    // Truncates data if bigger than payload size
    memcpy(buf.head.str, data, DATA_SIZE);

    /*
    printf("Preamble[0]: %p\n", *buf.head.preamble);
    printf("Preamble[1]: %p\n", buf.head.preamble[1]);
    printf("Preamble[2]: %p\n", buf.head.preamble[2]);
    printf("Preamble[3]: %p\n", buf.head.preamble[3]);
    printf("Preamble[4]: %p\n", buf.head.preamble[4]);
    printf("Preamble[5]: %p\n", buf.head.preamble[5]);
    printf("Preamble[6]: %p\n", buf.head.preamble[6]);
    printf("Preamble[7]: %p\n", buf.head.preamble[7]);
    printf("Preamble[8]: %p\n", buf.head.preamble[8]);
    printf("Preamble[9]: %p\n", buf.head.preamble[9]);
    printf("Key[0]: %p\n", buf.head.key[0]);
    printf("Key[1]: %p\n", buf.head.key[1]);
    printf("Key[2]: %p\n", buf.head.key[2]);
    printf("Key[3]: %p\n", buf.head.key[3]);
    printf("PI: %p\n", buf.head.PI);
    printf("Checksum[0]: %p\n", buf.head.checksum[0]);
    printf("Checksum[1]: %p\n", buf.head.checksum[1]);
    printf("First letter: %c\n", buf.head.str[0]);
	*/

    // Emulate transmission time
    //sleep((len*8)/19200);

    // Prepare address structure
	sa.sin_family = AF_INET;
    sa.sin_port = htons(dst);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Send the message
    if ( sendto(sock, buf.raw, HEADER_SIZE + len, 0 , (struct sockaddr *) &sa, sizeof(sa)) < 0){
       	return ERR_FAILED;
    }
    /*
    // Check if fully sent
    if ( recvfrom(sock, (char *)buffer , FRAME_PAYLOAD_SIZE, 0 , (struct sockaddr *) &sa, &slen) < -1){
      	return ERR_FAILED;
    }
    */

    return ERR_OK;
}

int radio_recv(int* src, char* data, int to_ms) {

    struct sockaddr_in sa;   // Structure to receive source address
    struct pollfd fds[1];
    frame_packet_t buf;
    short calculatedChecksum;
    short receivedChecksum;

    int len,adrlen=sizeof(sa);            // Size of received packet (or error code);

    memset(buf.raw, 0, FRAME_PAYLOAD_SIZE);
    memset(fds, 0, sizeof(fds));

    fds[0].fd = sock;
    fds[0].events = POLLIN;

    if (poll(fds, 1, to_ms) == 0) {
    	return ERR_TIMEOUT;
    }
    // Receive packet/data
    if ((len = recvfrom(sock, buf.raw, FRAME_PAYLOAD_SIZE, 0, (struct sockaddr *) &sa, &adrlen)) == -1) {
    	return ERR_FAILED;
    }


    receivedChecksum = buf.head.checksum[1];
    receivedChecksum = receivedChecksum << 8;
    receivedChecksum |= buf.head.checksum[0];
    calculatedChecksum = checkSum(buf.head.str);

    /*
    printf("Received checksum[0]: %p\n", buf.head.checksum[0]);
    printf("Received checksum[1]: %p\n", buf.head.checksum[1]);
    printf("Received checksum combined: %p\n", receivedChecksum);
    printf("Calculated checksum: %p\n", calculatedChecksum);
    */


    if (calculatedChecksum != receivedChecksum) {
    	printf("Packet integrety lost\n");
    	return ERR_CORR;
    }

    //check if checksum works



    // Set source from address structure

    /*
    printf("Preamble[0]: %p\n", *buf.head.preamble);
    printf("Preamble[1]: %p\n", buf.head.preamble[1]);
    printf("Preamble[2]: %p\n", buf.head.preamble[2]);
    printf("Preamble[3]: %p\n", buf.head.preamble[3]);
    printf("Preamble[4]: %p\n", buf.head.preamble[4]);
    printf("Preamble[5]: %p\n", buf.head.preamble[5]);
    printf("Preamble[6]: %p\n", buf.head.preamble[6]);
    printf("Preamble[7]: %p\n", buf.head.preamble[7]);
    printf("Preamble[8]: %p\n", buf.head.preamble[8]);
    printf("Preamble[9]: %p\n", buf.head.preamble[9]);
    printf("Key[0]: %p\n", buf.head.key[0]);
    printf("Key[1]: %p\n", buf.head.key[1]);
    printf("Key[2]: %p\n", buf.head.key[2]);
    printf("Key[3]: %p\n", buf.head.key[3]);
    printf("PI: %p\n", buf.head.PI);
    printf("Checksum[0]: %p\n", buf.head.checksum[0]);
    printf("Checksum[1]: %p\n", buf.head.checksum[1]);
    printf("First letter: %c\n", buf.head.str[0]);
    */

    memcpy(data, buf.head.str, DATA_SIZE);

    *src = ntohs(sa.sin_port);

    return len;
}

short checkSum (char * packet) {
	short sum = 0;
	char * s;
	for (s = packet ; *s != 0; s++ ){ sum ^= *s; }
	return ((short)sum);
}


