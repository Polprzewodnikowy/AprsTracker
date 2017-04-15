/*
 * tracker.c
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#include <stdlib.h>
#include <string.h>
#include "stm32f0xx.h"
#include "tracker.h"
#include "aprs.h"
#include "gps.h"
#include "config.h"
#include "lcd.h"

static Config *config;
static struct minmea_sentence_rmc *rmc;
static struct minmea_sentence_gga *gga;
static int beaconRate, beaconTimer, lastCourse, statusTimer;
static uint8_t miceMessage;
static char symbol, symbolTable;

static void TrackerDisplayUpdate(void);

void TrackerInit(void) {
    config = ConfigGet();
    rmc = GpsGetRMC();
    gga = GpsGetGGA();
    beaconRate = config->rate.slow;
    beaconTimer = config->rate.slow;
    statusTimer = config->status.rate;
    lastCourse = 0;
}

void TrackerUpdate(void) {
    if (GpsIsValid() && gga->satellites_tracked >= 3) {
        int speed = minmea_rescale(&rmc->speed, 1);
        int course = minmea_rescale(&rmc->course, 1);

        if (speed < config->speed.low) {
            beaconRate = config->rate.slow;
            symbol = config->symbol.park;
            symbolTable = config->symbolTable.park;
            miceMessage = config->MicE.park;
        } else {
            if (speed > config->speed.high) {
                beaconRate = config->rate.fast;
            } else {
                beaconRate = (config->rate.fast * config->speed.high) / speed;
            }

            int turnThreshold = config->turn.angle + (config->turn.slope / speed);
            int courseDifference = abs(course - lastCourse) % 360;
            courseDifference = courseDifference > 180 ? 360 - courseDifference : courseDifference;

            if (courseDifference >= turnThreshold && beaconTimer >= config->turn.delay) {
                beaconTimer = beaconRate;
            }

            symbol = config->symbol.drive;
            symbolTable = config->symbolTable.drive;
            miceMessage = config->MicE.drive;
        }

        if (beaconTimer >= beaconRate) {
            AX25Call call, path1, path2;
            AX25SetCall(&call, config->call.str, config->call.ssid);
            AX25SetCall(&path1, config->path[0].str, config->path[0].ssid);
            AX25SetCall(&path2, config->path[1].str, config->path[1].ssid);
            AprsSendMicEPosition(rmc, gga, &call, &path1, &path2, miceMessage, 0, symbol, symbolTable);
            beaconTimer = 0;
            lastCourse = course;
        }
    }
    if (statusTimer >= config->status.rate && config->status.length) {
        AX25Msg status;
        AX25InitFrame(&status);
        AX25SetCall(&status.source, config->call.str, config->call.ssid);
        AX25SetDestination(&status, "APZMF2", 0);
        AX25AddPath(&status, config->path[0].str, config->path[0].ssid);
        AX25AddPath(&status, config->path[1].str, config->path[1].ssid);
        AX25SetInfo(&status, config->status.str, config->status.length);
        AprsSendFrame(&status);
        statusTimer = 0;
    }
    ++beaconTimer;
    ++statusTimer;
    TrackerDisplayUpdate();
}

static void TrackerDisplayUpdate(void) {
    int lat = abs(minmea_rescale(&rmc->latitude, 100));
    int lon = abs(minmea_rescale(&rmc->longitude, 100));
    char ns = rmc->latitude.value >= 0 ? 'N' : 'S';
    char we = rmc->longitude.value >= 0 ? 'E' : 'W';
    int speed = minmea_rescale(&rmc->speed, 1);
    int course = minmea_rescale(&rmc->course, 1);

    char text[2][17];
    memset(text[0], ' ', 16);
    memset(text[1], ' ', 16);
    text[0][16] = text[1][16] = 0;

    text[0][0] = ' ';
    text[0][1] = '0' + ((lat / 100000) % 10);
    text[0][2] = '0' + ((lat / 10000) % 10);
    text[0][3] = '0' + ((lat / 1000) % 10);
    text[0][4] = '0' + ((lat / 100) % 10);
    text[0][5] = '.';
    text[0][6] = '0' + ((lat / 10) % 10);
    text[0][7] = '0' + ((lat / 1) % 10);
    text[0][8] = ns;
    text[0][9] = ' ';
    text[0][10] = '0' + ((speed / 100) % 10);
    text[0][11] = '0' + ((speed / 10) % 10);
    text[0][12] = '0' + ((speed / 1) % 10);
    text[0][13] = ' ';
    text[0][14] = rmc->valid ? 'A' : 'V';

    text[1][0] = '0' + ((lon / 1000000) % 10);
    text[1][1] = '0' + ((lon / 100000) % 10);
    text[1][2] = '0' + ((lon / 10000) % 10);
    text[1][3] = '0' + ((lon / 1000) % 10);
    text[1][4] = '0' + ((lon / 100) % 10);
    text[1][5] = '.';
    text[1][6] = '0' + ((lon / 10) % 10);
    text[1][7] = '0' + ((lon / 1) % 10);
    text[1][8] = we;
    text[1][9] = ' ';
    text[1][10] = '0' + ((course / 100) % 10);
    text[1][11] = '0' + ((course / 10) % 10);
    text[1][12] = '0' + ((course / 1) % 10);
    text[1][13] = ' ';
    text[1][14] = '0' + ((gga->satellites_tracked / 10) % 10);
    text[1][15] = '0' + ((gga->satellites_tracked / 1) % 10);

    LcdCursor(0, 0);
    LcdWriteText(text[0]);
    LcdCursor(0, 1);
    LcdWriteText(text[1]);
}
