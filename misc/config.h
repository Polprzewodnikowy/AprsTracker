/*
 * config.h
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#ifndef MISC_CONFIG_H_
#define MISC_CONFIG_H_

#include "aprs.h"

typedef struct {
	AX25Call call, path[2];
	char parkSymbol, driveSymbol, parkSymbolTable, driveSymbolTable;
	uint8_t parkMiceMessage, driveMiceMessage;
	int preambleLength, tailLength;
	int lowSpeed, highSpeed, slowRate, fastRate;
	int turnAngle, turnSlope, turnDelay;
	uint32_t gpsBaud;
	uint32_t crc;
} Config;

void ConfigInit(void);
Config *ConfigGet(void);
void ConfigUpdate(Config *c);

#endif /* MISC_CONFIG_H_ */
