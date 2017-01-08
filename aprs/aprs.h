/*
 * aprs.h
 *
 *  Created on: 24.12.2016
 *      Author: korgeaux
 */

#ifndef APRS_APRS_H_
#define APRS_APRS_H_

#include <stddef.h>
#include "ax25.h"
#include "minmea.h"

#define MICE_A_MASK	0x04
#define MICE_B_MASK	0x02
#define MICE_C_MASK	0x01

enum MicEMessage {
	MICE_OFF_DUTY = 0b111,
	MICE_EN_ROUTE = 0b110,
	MICE_IN_SERVICE = 0b101,
	MICE_RETURNING = 0b100,
	MICE_COMMITTED = 0b011,
	MICE_SPECIAL = 0b010,
	MICE_PRIORITY = 0b001,
	MICE_EMERGENCY = 0b000,
};

void AprsInit(void);
void AprsSendFrame(AX25Msg *frame);
void AprsSendMicEPosition(struct minmea_sentence_rmc *rmc, struct minmea_sentence_gga *gga, AX25Call *call,
		AX25Call *path1, AX25Call *path2, enum MicEMessage message, char *info, char symbol, char symbolTable);

#endif /* APRS_APRS_H_ */
