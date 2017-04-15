/*
 * config.h
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#ifndef MISC_CONFIG_H_
#define MISC_CONFIG_H_

#include "aprs.h"

#define CONFIG_VALID    0x55AA

typedef struct {
    union {
        uint16_t pd_combined;
        struct {
            char park;
            char drive;
        };
    } symbol;

    union {
        uint16_t pd_combined;
        struct {
            char park;
            char drive;
        };
    } symbolTable;

    union {
        uint16_t pd_combined;
        struct {
            char park;
            char drive;
        };
    } MicE;

    struct {
        uint16_t preamble;
        uint16_t tail;
    } afsk;

    union {
        uint16_t lh_combined;
        struct {
            uint8_t low;
            uint8_t high;
        };
    } speed;

    struct {
        uint16_t slow;
        uint16_t fast;
    } rate;

    struct {
        uint16_t slope;
        union {
            uint16_t ad_combined;
            struct {
                uint8_t angle;
                uint8_t delay;
            };
        };
    } turn;

    struct {
        uint16_t rate;
        char str[128];
        uint16_t length;
    } status;

    struct {
        union {
            uint32_t baud;
            struct {
                uint16_t baud_l;
                uint16_t baud_h;
            };
        };
    } gps;

    struct {
        union {
            char str[6];
            struct {
                uint16_t s1;
                uint16_t s2;
                uint16_t s3;
            };
        };
        uint16_t ssid;
    } call;

    struct {
        union {
            char str[6];
            struct {
                uint16_t s1;
                uint16_t s2;
                uint16_t s3;
            };
        };
        uint16_t ssid;
    } path[2];
} Config;

void ConfigInit(void);
Config *ConfigGet(void);

#endif /* MISC_CONFIG_H_ */
