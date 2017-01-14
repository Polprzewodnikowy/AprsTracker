/*
 * config.c
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "config.h"
#include "aprs.h"

static Config config;

void ConfigInit(void) {
    //TODO: read config from flash
    AX25SetCall(&config.call, "SP5XHJ", 9);
    AX25SetCall(&config.path[0], "WIDE1", 1);
    AX25SetCall(&config.path[1], "WIDE2", 1);
    config.parkSymbol = 'P';
    config.driveSymbol = '>';
    config.parkSymbolTable = '\\';
    config.driveSymbolTable = '5';
    config.parkMiceMessage = MICE_OFF_DUTY;
    config.driveMiceMessage = MICE_EN_ROUTE;
    config.preambleLength = 500;
    config.tailLength = 20;
    config.lowSpeed = 10;
    config.highSpeed = 60;
    config.slowRate = 1800;
    config.fastRate = 60;
    config.turnAngle = 10;
    config.turnSlope = 400;
    config.turnDelay = 5;
    config.gpsBaud = 9600;
}

Config *ConfigGet(void) {
    return &config;
}

void ConfigUpdate(Config *c) {
    config = *c;
    //TODO: write config to flash
}
