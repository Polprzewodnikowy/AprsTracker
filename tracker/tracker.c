/*
 * tracker.c
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#include "tracker/tracker.h"

#include "aprs/aprs.h"
#include "gps/gps.h"
#include "misc/config.h"

static Config *config;
static struct minmea_sentence_rmc *rmc;
static struct minmea_sentence_gga *gga;
static int beaconRate, beaconTimer, lastCourse, statusTimer;
static uint8_t micEMessage;
static char symbol, symbolTable;
static AX25Call path[4];

void TrackerInit(Config *c) {
    config = c;
    rmc = GpsGetRMC();
    gga = GpsGetGGA();
    beaconRate = config->rate.slow;
    beaconTimer = config->rate.slow;
    statusTimer = config->status.rate;
    lastCourse = 0;
    path[0] = (AX25Call) { .call = VERSION_STRING, .ssid = 0 };
    path[1] = (AX25Call) { .call = config->call.str, .ssid = config->call.ssid };
    path[2] = (AX25Call) { .call = config->path[0].str, .ssid = config->path[0].ssid };
    path[3] = (AX25Call) { .call = config->path[1].str, .ssid = config->path[1].ssid };
}

void TrackerUpdate(void) {
    if (GpsIsValid() && gga->satellites_tracked >= 3) {
        int speed = minmea_rescale(&rmc->speed, 1);
        int course = minmea_rescale(&rmc->course, 1);

        if (speed < config->speed.low) {
            beaconRate = config->rate.slow;
            symbol = config->symbol.park;
            symbolTable = config->symbolTable.park;
            micEMessage = config->MicE.park;
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
            micEMessage = config->MicE.drive;
        }

        if (beaconTimer >= beaconRate) {
            AprsSendMicEPosition(rmc, gga, path, 4, micEMessage, symbol, symbolTable, NULL, 0);
            beaconTimer = 0;
            lastCourse = course;
        }

        GPIOC->BSRR = GPIO_BSRR_BS_9;
    } else {
        GPIOC->BSRR = GPIO_BSRR_BR_9;
    }

    if (statusTimer >= config->status.rate && config->status.rate != 0 && config->status.length > 0) {
        AprsSendStatus(path, 4, config->status.str, config->status.length);
        statusTimer = 0;
    }

    ++beaconTimer;
    ++statusTimer;
}
