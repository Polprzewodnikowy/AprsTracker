/*
 * config.c
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#include <string.h>
#include "stm32f0xx.h"
#include "config.h"
#include "aprs.h"
#include "eeprom.h"

static Config config;

void ConfigInit(void) {
    uint16_t valid;
    EepromRead(0, &valid);
    if (valid != CONFIG_VALID) {
        EepromWrite(0, CONFIG_VALID);
        EepromWrite(1, DEFAULT_PARK_SYMBOL | (DEFAULT_DRIVE_SYMBOL << 8));
        EepromWrite(2, DEFAULT_PARK_SYMBOL_TABLE | (DEFAULT_DRIVE_SYMBOL_TABLE << 8));
        EepromWrite(3, DEFAULT_PREAMBLE_LENGTH);
        EepromWrite(4, DEFAULT_TAIL_LENGTH);
        EepromWrite(5, DEFAULT_LOW_SPEED | (DEFAULT_HIGH_SPEED << 8));
        EepromWrite(6, DEFAULT_SLOW_RATE);
        EepromWrite(7, DEFAULT_FAST_RATE);
        EepromWrite(8, DEFAULT_TURN_SLOPE);
        EepromWrite(9, DEFAULT_TURN_ANGLE | (DEFAULT_TURN_DELAY << 8));
        EepromWrite(10, DEFAULT_STATUS_RATE);
        EepromWrite(11, DEFAULT_GPS_BAUD & 0xFFFF);
        EepromWrite(12, DEFAULT_GPS_BAUD >> 16);
        EepromWrite(13, DEFAULT_CALL[0] | (DEFAULT_CALL[1] << 8));
        EepromWrite(14, DEFAULT_CALL[2] | (DEFAULT_CALL[3] << 8));
        EepromWrite(15, DEFAULT_CALL[4] | (DEFAULT_CALL[5] << 8));
        EepromWrite(16, DEFAULT_CALL_SSID);
        EepromWrite(17, DEFAULT_PATH_1[0] | (DEFAULT_PATH_1[1] << 8));
        EepromWrite(18, DEFAULT_PATH_1[2] | (DEFAULT_PATH_1[3] << 8));
        EepromWrite(19, DEFAULT_PATH_1[4] | (DEFAULT_PATH_1[5] << 8));
        EepromWrite(20, DEFAULT_PATH_1_SSID);
        EepromWrite(21, DEFAULT_PATH_2[0] | (DEFAULT_PATH_2[1] << 8));
        EepromWrite(22, DEFAULT_PATH_2[2] | (DEFAULT_PATH_2[3] << 8));
        EepromWrite(23, DEFAULT_PATH_2[4] | (DEFAULT_PATH_2[5] << 8));
        EepromWrite(24, DEFAULT_PATH_2_SSID);
        EepromWrite(25, DEFAULT_PARK_MICE_MESSAGE | (DEFAULT_DRIVE_MICE_MESSAGE << 8));
        char *info = DEFAULT_STATUS_TEXT;
        int len = strlen(info);
        EepromWrite(26, len);
        for (int i = 0; i < len - 1; i += 2) {
            EepromWrite(100 + (i / 2), info[i] | (info[i + 1] << 8));
        }
        if (len % 2) {
            EepromWrite(100 + (len / 2), info[len - 1]);
        }
    }

    EepromRead(1, &config.symbol.pd_combined);
    EepromRead(2, &config.symbolTable.pd_combined);
    EepromRead(3, &config.afsk.preamble);
    EepromRead(4, &config.afsk.tail);
    EepromRead(5, &config.speed.lh_combined);
    EepromRead(6, &config.rate.slow);
    EepromRead(7, &config.rate.fast);
    EepromRead(8, &config.turn.slope);
    EepromRead(9, &config.turn.ad_combined);
    EepromRead(10, &config.status.rate);
    EepromRead(11, &config.gps.baud_l);
    EepromRead(12, &config.gps.baud_h);
    EepromRead(13, &config.call.s1);
    EepromRead(14, &config.call.s2);
    EepromRead(15, &config.call.s3);
    EepromRead(16, &config.call.ssid);
    EepromRead(17, &config.path[0].s1);
    EepromRead(18, &config.path[0].s2);
    EepromRead(19, &config.path[0].s3);
    EepromRead(20, &config.path[0].ssid);
    EepromRead(21, &config.path[1].s1);
    EepromRead(22, &config.path[1].s2);
    EepromRead(23, &config.path[1].s3);
    EepromRead(24, &config.path[1].ssid);
    EepromRead(25, &config.MicE.pd_combined);

    uint8_t status = EepromRead(26, &config.status.length);
    if (status == EE_OK && config.status.length != 0) {
        for (int i = 0; i < config.status.length; i += 2) {
            uint16_t c;
            status = EepromRead(100 + (i / 2), &c);
            if (status == EE_OK) {
                config.status.str[i] = (char) (c & 0xFF);
                config.status.str[i + 1] = (char) (c >> 8);
            } else {
                break;
            }
        }
    }
}

Config *ConfigGet(void) {
    return &config;
}
