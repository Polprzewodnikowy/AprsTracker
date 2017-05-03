/*
 * aprs.c
 *
 *  Created on: 24.12.2016
 *      Author: korgeaux
 */

#include "aprs/aprs.h"

#include "afsk/afsk.h"

void AprsSendMicEPosition(struct minmea_sentence_rmc *rmc, struct minmea_sentence_gga *gga, AX25Call *path, 
    size_t pathLength, enum MicEMessage message, char symbol, char symbolTable, char *info, size_t infoLength) {
    static const char micECharset[33] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
        'K', 'L', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
        'X', 'Y', 'Z',
    };

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

    AX25Call aprsPath[10] = {
        { .call = dest, .ssid = 0 },
    };

    for(int i = 1; i < pathLength; i++) {
        aprsPath[i] = path[i];
    }

    char aprsInfo[256] = {
        '`',
        lonDegrees,
        lonMinutes,
        lonHundredths,
        tmpSP,
        tmpDC,
        tmpSE,
        symbol,
        symbolTable,
        tmpAlt1,
        tmpAlt2,
        tmpAlt3,
        '}',
    };

    if (info != NULL) {
        if (infoLength + 13 > 256)
            infoLength = 256 - 13;

        for (int i = 0; i < infoLength; i++)
            aprsInfo[i + 13] = info[i];
    } else {
        infoLength = 0;
    }

    AX25SendVia(aprsPath, pathLength, (uint8_t *) aprsInfo, infoLength + 13);
}

void AprsSendStatus(AX25Call *path, size_t pathLength, char *info, size_t infoLength) {
    char aprsInfo[256] = { '>' };

    if (info != NULL) {
        if (infoLength + 1 > 256)
            infoLength = 256 - 1;
        for (int i = 0; i < infoLength; i++)
            aprsInfo[i + 1] = info[i];
    } else {
        infoLength = 0;
    }
    
    AX25SendVia(path, pathLength, (uint8_t *) aprsInfo, infoLength + 1);
}
