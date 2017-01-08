/*
 * afsk.h
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#ifndef APRS_AFSK_H_
#define APRS_AFSK_H_

#include <stddef.h>

#define PTT_GPIO				GPIOA
#define PTT_PIN					5

#define AFSK_FIFO_SIZE			330

#define AFSK_BAUD				1200

#define AFSK_MARK_TONE			1200
#define AFSK_SPACE_TONE			2200

void AfskInit(void);
void AfskTxStart(void);
void AfskTxStop(void);
void AfskPutChar(char c);
void AfskTransmit(char *buffer, size_t size);

#endif /* APRS_AFSK_H_ */
