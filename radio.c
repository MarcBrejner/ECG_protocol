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
	char tag;
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

    // Check that port and len are valid
    if (dst < 0 || dst > 65535 || len < 0 || len > DATA_SIZE) {
    	return ERR_INVAL;
    }

    // Initialize the header and clean the packet
    memset(buf.head.str, 0, sizeof(char)*len);
    memset(buf.head.preamble, 170, sizeof(char)*10);
    memset(buf.head.key, 199, sizeof(char)*4);
    buf.head.PI = 201;
    memset(buf.head.checksum, 0, sizeof(char)*2);
    buf.head.tag = data[0];

    //memset(buf.head.checksum, 0, sizeof(char)*2);
    if (buf.head.tag == CHECK) {
    	short p = checkSum(data+sizeof(char));

    	buf.head.checksum[0] = p;
    	p = p>>8;
    	buf.head.checksum[1] = p;

    }

    memcpy(buf.head.str, data+sizeof(char), DATA_SIZE);


    // Emulate transmission time
    sleep(((HEADER_SIZE+len)*8)/19200);
    //sleep(1);

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
    char calculatedChecksum[2];
    char receivedChecksum[2];

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

    receivedChecksum[0] = buf.head.checksum[0];
    receivedChecksum[1] = buf.head.checksum[1];
    short p = checkSum(buf.head.str);
    calculatedChecksum[0] = p;
    p = p>>8;
    calculatedChecksum[1] = p;


    if(buf.head.tag == CHECK) {
    	if (calculatedChecksum[0] != receivedChecksum[0] || calculatedChecksum[1] != receivedChecksum[1]) {
    	    printf("Packet integrety lost\n");
    	    return ERR_CORR;
    	}
    }

    data[0] = buf.head.tag;
    memcpy(data+sizeof(char), buf.head.str, DATA_SIZE);

    // Set source from address structure
    *src = ntohs(sa.sin_port);

    return len;
}

short checkSum (char * packet) {
	short sum = 0;
	char * s;
	for (s = packet ; *s != 0; s++ ){ sum ^= *s; }
	return ((short)sum);
}


