/*
 * tracker.c
 *
 *  Created on: 01.01.2017
 *      Author: korgeaux
 */

#include <stdlib.h>
#include "stm32f0xx.h"
#include "tracker.h"
#include "aprs.h"
#include "gps.h"
#include "config.h"

static struct minmea_sentence_rmc *rmc;
static struct minmea_sentence_gga *gga;
static int beaconRate, beaconTimer, lastCourse;
static uint8_t miceMessage;
static char symbol, symbolTable;
static AX25Call call, path1, path2;
static int lowSpeed, highSpeed, slowRate, fastRate, turnAngle, turnSlope, turnDelay;
static char parkSymbol, driveSymbol, parkSymbolTable, driveSymbolTable;
static uint8_t parkMiceMessage, driveMiceMessage;

void TrackerInit(void)
{
	Config *c = ConfigGet();
	call = c->call;
	path1 = c->path[0];
	path2 = c->path[1];
	lowSpeed = c->lowSpeed;
	highSpeed = c->highSpeed;
	slowRate = c->slowRate;
	fastRate = c->fastRate;
	turnAngle = c->turnAngle;
	turnSlope = c->turnSlope;
	turnDelay = c->turnDelay;
	parkSymbol = c->parkSymbol;
	driveSymbol = c->driveSymbol;
	parkSymbolTable = c->parkSymbolTable;
	driveSymbolTable = c->driveSymbolTable;
	parkMiceMessage = c->parkMiceMessage;
	driveMiceMessage = c->driveMiceMessage;

	rmc = GpsGetRMC();
	gga = GpsGetGGA();
	beaconRate = slowRate;
	beaconTimer = slowRate;
	lastCourse = 0;
}

void TrackerUpdate(void)
{
	if(GpsIsValid() && gga->satellites_tracked >= 3) {
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
				beaconRate = fastRate * highSpeed / speed;
			}

			int turnThreshold = turnAngle + (turnSlope / speed);
			int courseDifference = abs(course - lastCourse);

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
}
