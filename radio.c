/*
 * radio.c
 *
 * Emulation of radio node using UDP (skeleton)
 */

// Implements
#include "radio.h"

// Uses
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#define BUFLEN 1024
int sock;    // UDP Socket used by this node
char buffer[BUFLEN];



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

	socklen_t slen;

    // Check that port and len are valid
    if (dst < 0 || dst > 65535 || len < 0 || len > 255) {
    	return ERR_INVAL;
    }

    // Emulate transmission time
    //sleep((len*8)/19200);

    // Prepare address structure
	sa.sin_family = AF_INET;
    sa.sin_port = dst;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Send the message
    if ( sendto(sock, (char *)data , len , 0 , (struct sockaddr *) &sa, sizeof(sa)) < 0){
       	return ERR_FAILED;
    }

    memset(buffer,'\0', BUFLEN);

    // Check if fully sent
    if ( recvfrom(sock, (char *)buffer , BUFLEN , 0 , (struct sockaddr *) &sa, &slen) < -1){
      	return ERR_FAILED;
    }

    return ERR_OK;
}

int radio_recv(int* src, char* data, int to_ms) {

    struct sockaddr_in sa;   // Structure to receive source address

    int len,adrlen=sizeof(sa);            // Size of received packet (or error code)

    // First poll/select with timeout (may be skipped at first)


    // Receive packet/data

    if ((len = recvfrom(sock, data , sizeof(data) , 0, (struct sockaddr *) &sa, &adrlen)) == -1) {
    	return ERR_FAILED;
    }


    /*if ( sendto(sock, (char *)buffer, BUFLEN, 0, (struct sockaddr *) &sa, adrlen) < 0){
    	return ERR_FAILED;
    }*/

    // Set source from address structure


    *src = ntohs(sa.sin_port);

    return len;
}

