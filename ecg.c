#include "ecg.h"
#include "radio.h"

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
	int seal;
	char str[0];
} data_pdu_t;

typedef struct {
	tag_t type;
	int seal;
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
	int done = 0;
	/*
	if ((err = radio_recv(&src, buf, 0)) >= ERR_OK) {
		printf("Flushing buffer of size: %d\n", err);
	}

	if (err != ERR_TIMEOUT) {
		printf("Failed with %d\n", err);
		return ERR_FAILED;
	}
	*/
	buf.data.type.tag = DATA;
	strcpy(buf.data.str, packet);
	if ((err = radio_send(dst, packet, len)) != ERR_OK) {
		printf("radio_send failed with: %d\n", err);
	}

	if ((errs = radio_recv(&src, buf.raw, to_ms)) >= ERR_OK){
		printf("ACK\n");
	}
	/*
	while(!done) {
		while(1) {

			if (err >= ERR_OK) {
				if (src != dst) {
					printf("wrong sender: %d\n", src);
					continue;
				}

				done = 1;
				break;
			}

			if (err != ERR_TIMEOUT) {
				return err;
			}
			break;
		}
	}
	return done ? ERR_OK : ERR_TIMEOUT;
	*/
	return err;
}

int ecg_recv(int* src, char* packet, int len, int to_ms) {
	int err, errs;
	pdu_frame_t buf;

	err = radio_recv(src, packet, to_ms);

	buf.ack.type.tag = ACK;

	if ((errs = radio_send(*src, packet, sizeof(buf))) != ERR_OK) {
		printf("Our radio_send failed with: %d\n", err);
	}

	/*
	while(1) {
		while(1) {

			if (err >= ERR_OK) {
				break;
			}
			printf("Our radio_recv failed with: %d\n", err);
		}


			printf("src: %d\n", src);
			printf("&src: %d\n", &src);
			printf("*src: %d\n", *src);

			return err;
		}
	}
	*/
	return err;
}

