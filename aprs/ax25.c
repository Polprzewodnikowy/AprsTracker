/*
 * ax25.c
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#include <string.h>
#include "stm32f0xx.h"
#include "ax25.h"
#include "afsk.h"
#include "hdlc.h"
#include "crc.h"

void AX25Init(void)
{
	CrcInit(0x1021, 0xFFFF, CRC_POLY_16, CRC_INV_BYTE, CRC_INV);
}

void AX25InitFrame(AX25Msg *msg)
{
	msg->controlField = AX25_CTRL_UI;
	msg->protocolId = AX25_PID_NOLAYER3;
	msg->pathLength = 0;
	msg->infoLength = 0;
}

void AX25SetCall(AX25Call *call, char *callsign, uint8_t ssid)
{
	memset(&call->callsign, ' ', 6);
	for (int i = 0; i < 6 && callsign[i] != 0; i++)
		call->callsign[i] = callsign[i];
	call->ssid = ssid;
}

void AX25SetSource(AX25Msg *msg, char *callsign, uint8_t ssid)
{
	AX25SetCall(&msg->source, callsign, ssid);
}

void AX25SetDestination(AX25Msg *msg, char *callsign, uint8_t ssid)
{
	AX25SetCall(&msg->destination, callsign, ssid);
}

void AX25AddPath(AX25Msg *msg, char *path, uint8_t ssid)
{
	uint8_t i = msg->pathLength;
	if (i < 8) {
		AX25SetCall(&msg->path[i], path, ssid);
		msg->pathLength += 1;
	}
}

void AX25SetPath(AX25Msg *msg, int iPath, char *path, uint8_t ssid)
{
	if (iPath < 8)
		AX25SetCall(&msg->path[iPath], path, ssid);
}

void AX25SetPathLength(AX25Msg *msg, uint8_t length)
{
	msg->pathLength = length;
}

void AX25ResetPath(AX25Msg *msg)
{
	msg->pathLength = 0;
}

void AX25SetInfo(AX25Msg *msg, char *info, size_t infoLength)
{
	msg->information = info;
	msg->infoLength = infoLength;
}

void AX25PutChar(char c)
{
	if (c == HDLC_FLAG || c == HDLC_RESET || c == AX25_ESC)
		AfskPutChar(AX25_ESC);
	CrcUpdate8(c);
	AfskPutChar(c);
}

void AX25SendRaw(char *buffer, size_t length)
{
	CrcReset();
	AfskPutChar(HDLC_FLAG);
	for (size_t i = 0; i < length; i++)
		AX25PutChar(buffer[i]);
	uint16_t crc = CrcGet() ^ 0xFF;
	AX25PutChar(crc & 0xFF);
	AX25PutChar(crc >> 8);
	AfskPutChar(HDLC_FLAG);
}
