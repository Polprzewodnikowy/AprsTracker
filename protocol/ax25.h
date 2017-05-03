/*
 * ax25.h
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#ifndef APRS_AX25_H_
#define APRS_AX25_H_

#include "misc/def.h"

#define AX25_CTRL_UI        0x03
#define AX25_PID_NOLAYER3   0xF0

typedef struct {
    char *call;
    uint8_t ssid;
} AX25Call;

void AX25Init(void);
void AX25SendRaw(char *buffer, size_t length);
void AX25SendVia(AX25Call *path, size_t pathLength, uint8_t *buffer, size_t bufferLength);

#endif /* APRS_AX25_H_ */
