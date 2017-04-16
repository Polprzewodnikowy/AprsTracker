/*
 * config.h
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#ifndef MISC_CONFIG_H_
#define MISC_CONFIG_H_

#include "aprs.h"

#define CONFIG_VALID                0x55AA

#define DEFAULT_PARK_SYMBOL         ('P')
#define DEFAULT_PARK_SYMBOL_TABLE   ('\\')
#define DEFAULT_PARK_MICE_MESSAGE   (MICE_OFF_DUTY)
#define DEFAULT_DRIVE_SYMBOL        ('>')
#define DEFAULT_DRIVE_SYMBOL_TABLE  ('5')
#define DEFAULT_DRIVE_MICE_MESSAGE  (MICE_EN_ROUTE)
#define DEFAULT_PREAMBLE_LENGTH     (500)
#define DEFAULT_TAIL_LENGTH         (150)
#define DEFAULT_LOW_SPEED           (10)
#define DEFAULT_HIGH_SPEED          (60)
#define DEFAULT_SLOW_RATE           (600)
#define DEFAULT_FAST_RATE           (30)
#define DEFAULT_TURN_SLOPE          (400)
#define DEFAULT_TURN_ANGLE          (10)
#define DEFAULT_TURN_DELAY          (5)
#define DEFAULT_STATUS_RATE         (3600)
#define DEFAULT_STATUS_TEXT         (">Default status text")
#define DEFAULT_GPS_BAUD            (9600)
#define DEFAULT_CALL                ("NOCALL")
#define DEFAULT_CALL_SSID           (9)
#define DEFAULT_PATH_1              ("WIDE1 ")
#define DEFAULT_PATH_1_SSID         (1)
#define DEFAULT_PATH_2              ("WIDE2 ")
#define DEFAULT_PATH_2_SSID         (1)

#define VERSION_STRING              ("APZMF2")

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

    struct {
        uint16_t rate;
        char str[128];
        uint16_t length;
    } status;
} Config;

void ConfigInit(void);
Config *ConfigGet(void);
void ConfigUpdate(void);

#endif /* MISC_CONFIG_H_ */
