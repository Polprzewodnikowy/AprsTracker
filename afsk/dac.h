/*
 * dac.h
 *
 *  Created on: 22.12.2016
 *      Author: korgeaux
 */

#ifndef APRS_DAC_H_
#define APRS_DAC_H_

#include "misc/def.h"

typedef struct WaveTable {
    uint8_t *sampleData;
    uint16_t sampleSize;
} WaveTable;

enum PredefinedWaveTable {
    SINE_WAVE,
    IDLE_WAVE,
};

void DacInit(void);
void DacSetFrequency(uint32_t freq, uint8_t update);
void DacSetWaveTable(WaveTable *wt, uint8_t update);
void DacSetPredefinedWaveTable(enum PredefinedWaveTable table, uint8_t update);
void DacUpdateSettings(void);
void DacStopPlaying(void);

#endif /* APRS_DAC_H_ */
