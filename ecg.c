#include "ecg.h"
#include "radio.h"
#include "alarm.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

// Tags
#define DATA 0
#define ACK 1

typedef struct {char tag; } tag_t;

typedef struct {
	tag_t type;	
	char str[0];
} data_pdu_t;

typedef struct {
	tag_t type;
} ack_pdu_t;

typedef union {
	char raw[FRAME_PAYLOAD_SIZE];

	tag_t pdu_type;
	data_pdu_t data;
	ack_pdu_t ack;
} pdu_frame_t;


int ecg_init(int addr) {
	return radio_init(addr);
}


int ecg_send(int  dst, char* packet, int len, int to_ms) {

	pdu_frame_t buf;
	int err, src, errs;

	memset(buf.raw, 0, FRAME_PAYLOAD_SIZE);
	buf.data.type.tag = DATA;
	strcpy(buf.data.str, packet);
	printf("packet len: %d\n", strlen(packet));

	if ((err = radio_send(dst, buf.raw, strlen(packet))) != ERR_OK) {
		printf("radio_send failed with: %d\n", err);
	}

	if ((errs = radio_recv(&src, buf.raw, to_ms)) >= ERR_OK){
		if (buf.ack.type.tag == ACK) {
			printf("ACK\n");
		}
	}

	return err;
}


int ecg_recv(int* src, char* packet, int len, int to_ms) {
	int err,errs;
	pdu_frame_t buf;

	memset(buf.raw, 0, FRAME_PAYLOAD_SIZE);

	err = radio_recv(src, buf.raw, to_ms);
	if (buf.data.type.tag == DATA) {

		printf("DATA RECEIVED\n");
		strcpy(packet,buf.data.str);

		buf.ack.type.tag = ACK;

		if ((errs = radio_send(*src, buf.raw, strlen(packet))) != ERR_OK) {
				printf("Our radio_send failed with: %d\n", errs);
		}
	}

	return err;
}

