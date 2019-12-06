#include "ecg.h"
#include "radio.h"
#include "alarm.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

typedef struct {char tag; } tag_t;

typedef struct {
	tag_t type;	
	char str[0];
} data_pdu_t;

typedef union {
	char raw[DATA_SIZE];

	tag_t pdu_type;
	data_pdu_t data;
} pdu_frame_t;

char localArray[MAX_SEND_SIZE];

int ecg_init(int addr) {
	return radio_init(addr);
}

int ecg_send(int  dst, char* packet, int len, int to_ms) {

	//Size of a single packet is DATA_SIZE - 1
	char singlePacket[DATA_SIZE-1];
	int err=0,counter=0;

	//Clear singlePacket
	memset(singlePacket, 0, DATA_SIZE -1);

	//Insert len in singlePacket
	printf("in Length %d\n",len);
	singlePacket[1] = (char) (len % 256);
	singlePacket[2] = (char) (len / 256);

	//Send start-packet that indicates a transfer is about to start always 3 bytes (tag and 2 bytes for length)
	ecg_sendPacket(dst, singlePacket, DATA_SIZE-1, to_ms, START);

	//While the remaining packets total length is bigger than the size of a single packet
	while(len >= DATA_SIZE -1){
		memcpy(singlePacket,packet+counter,DATA_SIZE - 1);
		err += ecg_sendPacket(dst, singlePacket, DATA_SIZE - 1, to_ms, DATA);
		counter += DATA_SIZE - 1;
		len -= DATA_SIZE - 1;
	}

	memset(singlePacket, 0, DATA_SIZE -1);
	memcpy(singlePacket,packet+counter,len);
	err += ecg_sendPacket(dst,singlePacket, len , to_ms,DATA);

	return err;
}


int ecg_sendPacket(int  dst, char* packet, int len, int to_ms, char tag) {

	pdu_frame_t buf;
	int err, src, errs;

	if (len > DATA_SIZE -1) {
		printf("ECG send invalid argument");
		return ERR_INVAL;
	}

	memset(buf.data.str, 0, DATA_SIZE);
	buf.data.type.tag = tag;
	memcpy(buf.data.str, packet, DATA_SIZE -1);

	if ((err = radio_send(dst, buf.raw, DATA_SIZE)) != ERR_OK) {
		printf("radio_send failed with: %d\n", err);
	}

	while(1) {

		errs = radio_recv(&src, buf.raw, to_ms);
		if (errs >= ERR_OK) {

			if (buf.data.type.tag != ACK && buf.data.type.tag != END && buf.data.type.tag != START) {
			printf("received a non-ACK packet with length %d\n",errs);
			continue;
			}

			if(buf.data.type.tag == START){
				printf("START-Acknowledgement received from %d\n",dst);
				break;
			}
			if(buf.data.type.tag == END){
				printf("END-acknowledgement received from %d\n",dst);
				break;
			}
			printf("Acknowledgement received from %d\n",dst);
			break;
		}

		if(errs != ERR_TIMEOUT){
		printf("Unknown error occured when receiving acknowledgement\n");
		return errs;
		}

		printf("Acknowledgement timed out\n");
		return errs;

	}
	return err;
}


int ecg_recv(int* src, char* packet, int len, int to_ms) {
	int err=0, errs, recLen,ogLen;
	int counter = 0;


	pdu_frame_t buf;

	//Receive packet
	memset(buf.raw,0,DATA_SIZE);
	radio_recv(src , buf.raw , to_ms);

	//Check if start of transfer packet
	if(buf.data.type.tag == START){

		//Set total length of data to be received

		unsigned char tempA, tempB;
		tempA = buf.data.str[1];
		tempB = buf.data.str[2];

		recLen = (int) tempA + (int) tempB*256;
		ogLen = recLen;

		printf("recLen %d\n",recLen);

		//Check if data is too big to receive
		if (recLen > MAX_SEND_SIZE){
			return ERR_INVAL;
		}

		//If OKAY, then send back acknowledgement
		memset(buf.raw,0,DATA_SIZE);
		buf.data.type.tag = START;
		if ((errs = radio_send(*src, buf.raw, 1)) != ERR_OK) {
				printf("recvACK failed with: %d\n", errs);
		}
	}

	while(recLen > DATA_SIZE - 1){

		memset(buf.raw, 0, DATA_SIZE);
		err += radio_recv(src, buf.raw, to_ms);

		if(buf.data.type.tag == DATA){

			printf("DATA RECEIVED\n");

			memcpy(localArray+counter,buf.data.str,DATA_SIZE -1);
			counter += DATA_SIZE - 1;
			recLen -= DATA_SIZE -1;

			memset(buf.raw,0,DATA_SIZE);
			buf.data.type.tag = ACK;
			if ((errs = radio_send(*src, buf.raw, 1)) != ERR_OK) {
					printf("whileACK failed with: %d\n", errs);
			}

		}

	}

	memset(buf.raw, 0, DATA_SIZE -1);
	err += radio_recv(src,buf.raw,to_ms);

	if(buf.data.type.tag == DATA){
		printf("DATA RECEIVED\n");
		memcpy(localArray+counter,buf.data.str,recLen);
		memset(buf.raw,0,DATA_SIZE);

		//send ACK
		buf.data.type.tag = END;
		if ((errs = radio_send(*src, buf.raw, 1)) != ERR_OK) {
				printf("endACK failed with: %d\n", errs);
		}
	}

	memcpy(packet,localArray,ogLen);

	return err;
}

