/*
 * ax25.c
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#include "protocol/ax25.h"

#include "afsk/afsk.h"
#include "protocol/hdlc.h"
#include "protocol/crc.h"

static void AX25PutChar(char c);
static void AX25SendCall(AX25Call *addr, bool last);

void AX25Init(void) {
    CrcInit(0x1021, 0xFFFF, CRC_POLY_16, CRC_INV_BYTE, CRC_INV);
}

void AX25SendRaw(char *buffer, size_t length) {
    CrcReset();
    AfskPutChar(HDLC_FLAG);
    for (size_t i = 0; i < length; i++)
        AX25PutChar(buffer[i]);
    uint16_t crc = CrcGet() ^ 0xFFFF;
    AX25PutChar(crc & 0xFF);
    AX25PutChar(crc >> 8);
    AfskPutChar(HDLC_FLAG);
}

void AX25SendVia(AX25Call *path, size_t pathLength, uint8_t *buffer, size_t bufferLength) {
    CrcReset();
    AfskPutChar(HDLC_FLAG);
    for (size_t i = 0; i < pathLength; i++) {
        AX25SendCall(&path[i], (i == pathLength - 1));
    }
    AX25PutChar(AX25_CTRL_UI);
    AX25PutChar(AX25_PID_NOLAYER3);
    for (size_t i = 0; i < bufferLength; i++)
        AX25PutChar(buffer[i]);
    uint16_t crc = CrcGet() ^ 0xFFFF;
    AX25PutChar(crc & 0xFF);
    AX25PutChar(crc >> 8);
    AfskPutChar(HDLC_FLAG);
}

static void AX25PutChar(char c) {
    if (c == HDLC_FLAG || c == HDLC_RESET || c == AX25_ESC)
        AfskPutChar(AX25_ESC);
    CrcUpdate8(c);
    AfskPutChar(c);
}

static void AX25SendCall(AX25Call *addr, bool last) {
    for (int n = 0; n < 6; n++)
        AX25PutChar(addr->call[n] << 1);
    AX25PutChar(addr->ssid << 1 | 0x60 | (last ? 1 : 0));
}
