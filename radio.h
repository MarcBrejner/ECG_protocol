/*
 * radio.h
 *
 * Interface to the radio device
 */

#ifndef _RADIO_H_
#define _RADIO_H_

#include "errors.h"

// Tags
#define CHECK 0

#define FRAME_PAYLOAD_SIZE 72
#define HEADER_SIZE 18
#define DATA_SIZE FRAME_PAYLOAD_SIZE - HEADER_SIZE

int radio_init(int addr);

int radio_send(int  dst, char* data, int len);

int radio_recv(int* src, char* data, int to_ms);

short checkSum (char * packet);

#endif // _RADIO_H_
