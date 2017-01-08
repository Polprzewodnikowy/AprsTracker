/*
 * dac.h
 *
 *  Created on: 22.12.2016
 *      Author: korgeaux
 */

#ifndef APRS_DAC_H_
#define APRS_DAC_H_

#define SINE_WAVETABLE_SIZE 64
#define SQUARE_WAVETABLE_SIZE 2

enum WaveTable {
	SINE_WAVE,
	SQUARE_WAVE,
};

void DacInit(void);
void DacSetWaveTableSource(const uint8_t *wt, uint32_t s);
void DacSetFrequency(uint32_t freq);
void DacUpdateSettings(void);
void DacChangeWaveTableSource(const uint8_t *wt, uint32_t s);
void DacChangeFrequency(uint32_t freq);
const uint8_t *DacGetPredefinedWaveTable(enum WaveTable table);
void DacStopWaveGeneration(void);

#endif /* APRS_DAC_H_ */
