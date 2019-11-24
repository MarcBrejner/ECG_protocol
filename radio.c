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
int sock;    // UDP Socket used by this node

int radio_init(int addr) {

    struct sockaddr_in sa;   // Structure to set own address

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
    sa.sin_port = addr;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to port
    if (bind(sock, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
    	return ERR_FAILED;
    }

    return ERR_OK;
}

int radio_send(int  dst, char* data, int len) {

    struct sockaddr_in sa;   // Structure to hold destination address

    // Check that port and len are valid


    // Emulate transmission time

    // Prepare address structure

    // Send the message

    // Check if fully sent

    return ERR_OK;
}

int radio_recv(int* src, char* data, int to_ms) {

    struct sockaddr_in sa;   // Structure to receive source address

    int len = -1;            // Size of received packet (or error code)


    // First poll/select with timeout (may be skipped at first)

    // Receive packet/data

    // Set source from address structure
    // *src = ntohs(sa.sin_port);

    return len;
}

