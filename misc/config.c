/*
 * config.c
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#include "misc/config.h"

#include "misc/eeprom.h"

#define TABLE_SIZE(x)   (sizeof(x) / sizeof(x[0]))

static void readStatusText(char *str, uint16_t *len);
static void writeStatusText(char *str, uint16_t len);

static Config config;
static uint16_t *configPointers[] = {
    &config.symbol.pd_combined,
    &config.symbolTable.pd_combined,
    &config.afsk.preamble,
    &config.afsk.tail,
    &config.speed.lh_combined,
    &config.rate.slow,
    &config.rate.fast,
    &config.turn.slope,
    &config.turn.ad_combined,
    &config.status.rate,
    &config.gps.baud_l,
    &config.gps.baud_h,
    &config.call.s1,
    &config.call.s2,
    &config.call.s3,
    &config.call.ssid,
    &config.path[0].s1,
    &config.path[0].s2,
    &config.path[0].s3,
    &config.path[0].ssid,
    &config.path[1].s1,
    &config.path[1].s2,
    &config.path[1].s3,
    &config.path[1].ssid,
    &config.MicE.pd_combined,
};

void ConfigInit(void) {
    uint16_t defaultConfig[] = {
        DEFAULT_PARK_SYMBOL | (DEFAULT_DRIVE_SYMBOL << 8),
        DEFAULT_PARK_SYMBOL_TABLE | (DEFAULT_DRIVE_SYMBOL_TABLE << 8),
        DEFAULT_PREAMBLE_LENGTH,
        DEFAULT_TAIL_LENGTH,
        DEFAULT_LOW_SPEED | (DEFAULT_HIGH_SPEED << 8),
        DEFAULT_SLOW_RATE,
        DEFAULT_FAST_RATE,
        DEFAULT_TURN_SLOPE,
        DEFAULT_TURN_ANGLE | (DEFAULT_TURN_DELAY << 8),
        DEFAULT_STATUS_RATE,
        DEFAULT_GPS_BAUD & 0xFFFF,
        DEFAULT_GPS_BAUD >> 16,
        DEFAULT_CALL[0] | (DEFAULT_CALL[1] << 8),
        DEFAULT_CALL[2] | (DEFAULT_CALL[3] << 8),
        DEFAULT_CALL[4] | (DEFAULT_CALL[5] << 8),
        DEFAULT_CALL_SSID,
        DEFAULT_PATH_1[0] | (DEFAULT_PATH_1[1] << 8),
        DEFAULT_PATH_1[2] | (DEFAULT_PATH_1[3] << 8),
        DEFAULT_PATH_1[4] | (DEFAULT_PATH_1[5] << 8),
        DEFAULT_PATH_1_SSID,
        DEFAULT_PATH_2[0] | (DEFAULT_PATH_2[1] << 8),
        DEFAULT_PATH_2[2] | (DEFAULT_PATH_2[3] << 8),
        DEFAULT_PATH_2[4] | (DEFAULT_PATH_2[5] << 8),
        DEFAULT_PATH_2_SSID,
        DEFAULT_PARK_MICE_MESSAGE | (DEFAULT_DRIVE_MICE_MESSAGE << 8),
    };

    EepromInit();

    uint16_t valid;
    EepromRead(0, &valid);
    if (valid != CONFIG_VALID) {
        EepromWrite(0, CONFIG_VALID);
        for (int i = 0; i < TABLE_SIZE(defaultConfig); i++) {
            EepromWrite(i + 1, defaultConfig[i]);
        }
    }

    for (int i = 0; i < TABLE_SIZE(configPointers); i++) {
        uint8_t status = EepromRead(i + 1, configPointers[i]);
        if (status != EE_OK) {
            *configPointers[i] = defaultConfig[i];
        }
    }

    readStatusText(config.status.str, &config.status.length);
}

Config *ConfigGet(void) {
    return &config;
}

void ConfigUpdate(void) {
    for (int i = 0; i < TABLE_SIZE(configPointers); i++) {
        uint16_t tmp;
        uint8_t status = EepromRead(i + 1, &tmp);
        if (status != EE_OK || tmp != *configPointers[i]) {
            EepromWrite(i + 1, *configPointers[i]);
        }
    }

    uint16_t len;
    char str[128];
    memset(str, 0, 128);
    readStatusText(str, &len);
    if (len != config.status.length || strcmp(str, config.status.str)) {
        writeStatusText(config.status.str, config.status.length);
    }
}

static void readStatusText(char *str, uint16_t *len) {
    uint8_t status = EepromRead(26, len);
    if (status == EE_OK && *len != 0) {
        for (int i = 0; i < *len; i += 2) {
            uint16_t c;
            status = EepromRead(100 + (i / 2), &c);
            if (status == EE_OK) {
                str[i] = (char) (c & 0xFF);
                str[i + 1] = (char) (c >> 8);
            } else {
                break;
            }
        }
    } else {
        len = 0;
    }
}

static void writeStatusText(char *str, uint16_t len) {
    EepromWrite(26, len);
    for (int i = 0; i < len - 1; i += 2) {
        EepromWrite(100 + (i / 2), str[i] | (str[i + 1] << 8));
    }
    if (len % 2) {
        EepromWrite(100 + (len / 2), str[len - 1]);
    }
}
