/*
 * gps.h
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#ifndef GPS_GPS_H_
#define GPS_GPS_H_

#include "misc/def.h"

#include "gps/minmea.h"
#include "misc/config.h"

#define GPS_BUFFER_SIZE 128

void GpsInit(Config *c);
void GpsProcess(void);
int GpsIsValid(void);
struct minmea_sentence_rmc *GpsGetRMC(void);
struct minmea_sentence_gga *GpsGetGGA(void);

#endif /* GPS_GPS_H_ */
