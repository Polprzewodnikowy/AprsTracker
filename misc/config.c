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
        EepromWrite(1, 'P' | ('>' << 8));
        EepromWrite(2, '\\' | ('5' << 8));
        EepromWrite(3, 500);
        EepromWrite(4, 150);
        EepromWrite(5, 10 | (60 << 8));
        EepromWrite(6, 600);
        EepromWrite(7, 30);
        EepromWrite(8, 400);
        EepromWrite(9, 10 | (5 << 8));
        EepromWrite(10, 1800);
        EepromWrite(11, 9600 & 0xFFFF);
        EepromWrite(12, 9600 >> 16);
        EepromWrite(13, 'N' | ('O' << 8));
        EepromWrite(14, 'C' | ('A' << 8));
        EepromWrite(15, 'L' | ('L' << 8));
        EepromWrite(16, 0);
        EepromWrite(17, 'W' | ('I' << 8));
        EepromWrite(18, 'D' | ('E' << 8));
        EepromWrite(19, '1' | (' ' << 8));
        EepromWrite(20, 1);
        EepromWrite(21, 'W' | ('I' << 8));
        EepromWrite(22, 'D' | ('E' << 8));
        EepromWrite(23, '2' | (' ' << 8));
        EepromWrite(24, 1);
        EepromWrite(25, MICE_OFF_DUTY | (MICE_EN_ROUTE << 8));
        char *info = "Insert your info here";
        int len = strlen(info);
        EepromWrite(100, len);
        for (int i = 0; i < len; i++) {
            EepromWrite(101 + i, info[i]);
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

    uint8_t status = EepromRead(100, &config.status.length);
    if (status == EE_OK && config.status.length != 0) {
        config.status.str[0] = '>';
        int i;
        for (i = 1; i < config.status.length; i++) {
            uint16_t c;
            status = EepromRead(i + 100, &c);
            if (status == EE_OK) {
                config.status.str[i] = (char) c;
            } else {
                break;
            }
        }
        config.status.str[i] = 0;
    }
}

Config *ConfigGet(void) {
    return &config;
}
