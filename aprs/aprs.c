/*
 * aprs.c
 *
 *  Created on: 24.12.2016
 *      Author: korgeaux
 */

#include <stdlib.h>
#include <string.h>
#include "stm32f0xx.h"
#include "aprs.h"
#include "ax25.h"
#include "afsk.h"
#include "hdlc.h"
#include "crc.h"

static const char micECharset[33] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
	'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
};

void AprsInit(void)
{
	AfskInit();
	AX25Init();
}

void AprsSendFrame(AX25Msg *frame)
{
	AfskPutChar(HDLC_FLAG);

	CrcReset();

	for(int i = 0; i < 6; i++)
		AX25PutChar(frame->destination.callsign[i] << 1);
	AX25PutChar(frame->destination.ssid << 1 | 0x60);

	for(int i = 0; i < 6; i++)
		AX25PutChar(frame->source.callsign[i] << 1);
	AX25PutChar(frame->source.ssid << 1 | 0x60  | (frame->pathLength ? 0 : 1));

	for(uint8_t i = 0; i < frame->pathLength; i++) {
		for(int n = 0; n < 6; n++)
			AX25PutChar(frame->path[i].callsign[n] << 1);
		AX25PutChar(frame->path[i].ssid << 1 | 0x60 | (frame->pathLength == (i + 1) ? 1 : 0));
	}

	AX25PutChar(frame->controlField);
	AX25PutChar(frame->protocolId);

	for(size_t i = 0; i < frame->infoLength; i++)
		AX25PutChar(frame->information[i]);

	uint16_t crc = CrcGet() ^ 0xFFFF;

	AX25PutChar(crc & 0xFF);
	AX25PutChar(crc >> 8);

	AfskPutChar(HDLC_FLAG);
}



void AprsSendMicEPosition(struct minmea_sentence_rmc *rmc, struct minmea_sentence_gga *gga, AX25Call *call, AX25Call *path1, AX25Call *path2, enum MicEMessage message, char *info, char symbol, char symbolTable)
{
	AX25Msg frame;
	AX25InitFrame(&frame);
	frame.source = *call;

	if(!info)
		info = "";

	int lat = abs(minmea_rescale(&rmc->latitude, 100));
	int lon = abs(minmea_rescale(&rmc->longitude, 100));

	uint8_t latDegrees = lat / 10000;
	uint8_t latMinutes = (lat % 10000) / 100;
	uint8_t latHundredths = (lat % 10000) % 100;

	uint8_t lonDegrees = lon / 10000;
	uint8_t lonMinutes = (lon % 10000) / 100;
	uint8_t lonHundredths = (lon % 10000) % 100;

	uint16_t speed = minmea_rescale(&rmc->speed, 1);
	uint16_t course = minmea_rescale(&rmc->course, 1);

	char dest[6];

	dest[0] = micECharset[(latDegrees / 10) + ((message & MICE_A_MASK) ? 22 : 0)];
	dest[1] = micECharset[(latDegrees % 10) + ((message & MICE_B_MASK) ? 22 : 0)];
	dest[2] = micECharset[(latMinutes / 10) + ((message & MICE_C_MASK) ? 22 : 0)];
	dest[3] = micECharset[(latMinutes % 10) + ((rmc->latitude.value >= 0) ? 22 : 0)];
	dest[4] = micECharset[(latHundredths / 10) + ((lonDegrees >= 100 || lonDegrees <= 9) ? 22 : 0)];
	dest[5] = micECharset[(latHundredths % 10) + ((rmc->longitude.value >= 0) ? 0 : 22)];

	AX25SetDestination(&frame, dest, 0);
	if(path1)
		AX25AddPath(&frame, path1->callsign, path1->ssid);
	if(path2)
		AX25AddPath(&frame, path2->callsign, path2->ssid);

	if (lonDegrees <= 9)
		lonDegrees += 118;
	else if (lonDegrees <= 99)
		lonDegrees += 28;
	else if (lonDegrees <= 109)
		lonDegrees += 8;
	else
		lonDegrees -= 72;

	if (lonMinutes <= 9)
		lonMinutes += 88;
	else
		lonMinutes += 28;

	lonHundredths += 28;

	uint8_t tmpSP = (speed / 10) + (speed <= 199 ? 'l' : '0');
	uint8_t tmpDC = ((speed % 10) * 10) + (course / 100) + 32;
	uint8_t tmpSE = (course % 100) + 28;

	int alt = minmea_rescale(&gga->altitude, 1) + 10000;
	uint8_t tmpAlt1 = (alt / 8281) + '!';
	uint8_t tmpAlt2 = ((alt % 8281) / 91) + '!';
	uint8_t tmpAlt3 = ((alt % 8281) % 91) + '!';

	char infoField[256];
	infoField[0] = '`';
	infoField[1] = lonDegrees;
	infoField[2] = lonMinutes;
	infoField[3] = lonHundredths;
	infoField[4] = tmpSP;
	infoField[5] = tmpDC;
	infoField[6] = tmpSE;
	infoField[7] = symbol;
	infoField[8] = symbolTable;
	infoField[9] = tmpAlt1;
	infoField[10] = tmpAlt2;
	infoField[11] = tmpAlt3;
	infoField[12] = '}';
	int i;
	for (i = 13; i < 256 && info[i - 13] != 0; i++)
		infoField[i] = info[i - 13];

	AX25SetInfo(&frame, infoField, i);

	AprsSendFrame(&frame);
}
