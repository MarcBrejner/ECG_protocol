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
char buffer[FRAME_PAYLOAD_SIZE];

typedef struct {
	char preamble[0];
	char key[0];
	char PI;
	char str[0];
	char checksum[0];
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
    if (dst < 0 || dst > 65535 || len < 0 || len > 1025) {
    	return ERR_INVAL;
    }

    memset(buf.head.preamble, 170, sizeof(char)*10);
    memset(buf.head.key, 200, sizeof(char)*4);
    buf.head.PI = 201;
    memset(buf.head.str, 0, FRAME_PAYLOAD_SIZE+1);
    strcpy(buf.head.str, data);


    // Emulate transmission time
    //sleep((len*8)/19200);

    // Prepare address structure
	sa.sin_family = AF_INET;
    sa.sin_port = htons(dst);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Send the message
    if ( sendto(sock, buf.raw, len+1 , 0 , (struct sockaddr *) &sa, sizeof(sa)) < 0){
       	return ERR_FAILED;
    }

    memset(buffer, 0, FRAME_PAYLOAD_SIZE);
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

    int len,adrlen=sizeof(sa);            // Size of received packet (or error code)

    memset(fds, 0, sizeof(fds));

    fds[0].fd = sock;
    fds[0].events = POLLIN;

    if (poll(fds, 1, to_ms) == 0) {
    	return ERR_TIMEOUT;
    }
    // Receive packet/data
    if ((len = recvfrom(sock, buf.raw , FRAME_PAYLOAD_SIZE , 0, (struct sockaddr *) &sa, &adrlen)) == -1) {
    	return ERR_FAILED;
    }
    strcpy(data, buf.head.str);
    // Set source from address structure

    *src = ntohs(sa.sin_port);

    return len;
}


