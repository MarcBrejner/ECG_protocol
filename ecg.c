#include "ecg.h"
#include "radio.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>


int ecg_init(int addr) {
	return radio_init(addr);
}

int ecg_send(int  dst, char* packet, int len, int to_ms) {
	int err, src, done;
	char buf[FRAME_PAYLOAD_SIZE];
	if ((err = radio_recv(&src, buf, 0)) >= ERR_OK) {
		printf("Flushing buffer of size: %d\n", err);
	}

	if (err != ERR_TIMEOUT) { return ERR_FAILED; }

	while(!done) {
		if ((err = radio_send(dst, packet, len)) != ERR_OK) {
				printf("radio_send failed with: %d\n", err);
			}

		while(1) {
			err = radio_recv(&src, buf, to_ms);
			if (err >= ERR_OK) {
				if (src != dst) {
					printf("wrong sender: %d\n", src);
					continue;
				}
				printf("ACK");
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
}

int ecg_recv(int* src, char* packet, int len, int to_ms) {
	int err;
	char buf[FRAME_PAYLOAD_SIZE];
	err = radio_recv(src, packet, to_ms);
	if (err <= 0) {
		printf("radio_recv failed with: %d\n", err);
	}

	if ((err = radio_send(src, buf, sizeof(buf))) != ERR_OK) {
		printf("radio_send failed with: %d\n", err);
	}
}

