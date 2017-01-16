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

static struct minmea_sentence_rmc *rmc;
static struct minmea_sentence_gga *gga;
static int beaconRate, beaconTimer, lastCourse;
static uint8_t miceMessage;
static char symbol, symbolTable;
static AX25Call call, path1, path2;
static int lowSpeed, highSpeed, slowRate, fastRate, turnAngle, turnSlope, turnDelay;
static char parkSymbol, driveSymbol, parkSymbolTable, driveSymbolTable;
static uint8_t parkMiceMessage, driveMiceMessage;

void TrackerInit(void) {
    Config *config = ConfigGet();
    call = config->call;
    path1 = config->path[0];
    path2 = config->path[1];
    lowSpeed = config->lowSpeed;
    highSpeed = config->highSpeed;
    slowRate = config->slowRate;
    fastRate = config->fastRate;
    turnAngle = config->turnAngle;
    turnSlope = config->turnSlope;
    turnDelay = config->turnDelay;
    parkSymbol = config->parkSymbol;
    driveSymbol = config->driveSymbol;
    parkSymbolTable = config->parkSymbolTable;
    driveSymbolTable = config->driveSymbolTable;
    parkMiceMessage = config->parkMiceMessage;
    driveMiceMessage = config->driveMiceMessage;

    rmc = GpsGetRMC();
    gga = GpsGetGGA();
    beaconRate = slowRate;
    beaconTimer = slowRate;
    lastCourse = 0;
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

void TrackerUpdate(void) {
    if (GpsIsValid() && gga->satellites_tracked >= 3) {
        int speed = minmea_rescale(&rmc->speed, 1);
        int course = minmea_rescale(&rmc->course, 1);

        if (speed < lowSpeed) {
            beaconRate = slowRate;
            symbol = parkSymbol;
            symbolTable = parkSymbolTable;
            miceMessage = parkMiceMessage;
        } else {
            if (speed > highSpeed) {
                beaconRate = fastRate;
            } else {
                beaconRate = (fastRate * highSpeed) / speed;
            }

            int turnThreshold = turnAngle + (turnSlope / speed);
            int courseDifference = abs(course - lastCourse) % 360;
            courseDifference = courseDifference > 180 ? 360 - courseDifference : courseDifference;

            if (courseDifference >= turnThreshold && beaconTimer >= turnDelay) {
                beaconTimer = beaconRate;
            }

            symbol = driveSymbol;
            symbolTable = driveSymbolTable;
            miceMessage = driveMiceMessage;
        }

        if (beaconTimer >= beaconRate) {
            AprsSendMicEPosition(rmc, gga, &call, &path1, &path2, miceMessage, 0, symbol, symbolTable);
            beaconTimer = 0;
            lastCourse = course;
        }
    }
    ++beaconTimer;
    TrackerDisplayUpdate();
}
