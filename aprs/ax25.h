/*
 * ax25.h
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#ifndef APRS_AX25_H_
#define APRS_AX25_H_

#include <stddef.h>

#define AX25_CTRL_UI		0x03
#define AX25_PID_NOLAYER3	0xF0

typedef struct {
	char callsign[6];
	uint8_t ssid;
} AX25Call;

typedef struct {
	AX25Call source;
	AX25Call destination;
	AX25Call path[8];
	uint8_t pathLength;
	uint8_t controlField;
	uint8_t protocolId;
	char *information;
	size_t infoLength;
} AX25Msg;

void AX25Init(void);
void AX25InitFrame(AX25Msg *frame);
void AX25SetCall(AX25Call *ax25call, char *callsign, uint8_t ssid);
void AX25SetSource(AX25Msg *frame, char *callsign, uint8_t ssid);
void AX25SetDestination(AX25Msg *frame, char *callsign, uint8_t ssid);
void AX25AddPath(AX25Msg *frame, char *path, uint8_t ssid);
void AX25SetPath(AX25Msg *frame, int npath, char *path, uint8_t ssid);
void AX25SetPathLength(AX25Msg *frame, uint8_t length);
void AX25ResetPath(AX25Msg *frame);
void AX25SetInfo(AX25Msg *frame, char *information, size_t infoLength);
void AX25PutChar(char c);
void AX25SendRaw(char *buffer, size_t length);

#endif /* APRS_AX25_H_ */
