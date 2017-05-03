/*
 * dac.c
 *
 *  Created on: 22.12.2016
 *      Author: korgeaux
 */

#include "afsk/dac.h"

#include "misc/gpio.h"

static const uint8_t sineSamples[] = {
    0x80, 0x8C, 0x98, 0xA5, 0xB0, 0xBC, 0xC6, 0xD0, 0xDA, 0xE2, 0xEA, 0xF0, 0xF5, 0xFA, 0xFD, 0xFE,
    0xFF, 0xFE, 0xFD, 0xFA, 0xF5, 0xF0, 0xEA, 0xE2, 0xDA, 0xD0, 0xC6, 0xBC, 0xB0, 0xA5, 0x98, 0x8C,
    0x80, 0x73, 0x67, 0x5A, 0x4F, 0x43, 0x39, 0x2F, 0x25, 0x1D, 0x15, 0x0F, 0x0A, 0x05, 0x02, 0x01,
    0x00, 0x01, 0x02, 0x05, 0x0A, 0x0F, 0x15, 0x1D, 0x25, 0x2F, 0x39, 0x43, 0x4F, 0x5A, 0x67, 0x73
};

static const uint8_t idleSamples[] = {
    0x80
};  

static WaveTable sineWaveTable, idleWaveTable;
static WaveTable *waveTable, *currentWaveTable;
static uint32_t frequency, currentFrequency;

void DacInit(void) {
    sineWaveTable.sampleData = (uint8_t *) sineSamples;
    sineWaveTable.sampleSize = 64;
    idleWaveTable.sampleData = (uint8_t *) idleSamples;
    idleWaveTable.sampleSize = 1;

    waveTable = &idleWaveTable;
    frequency = 1000;

    //GPIO | PA4 Analog
    GpioConfig(GPIOA, 4, GPIO_ANALOG);

    //RCC | DAC and TIM6 clock enabled
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    //DAC | DMA enable, trigger source = TIM6, trigger enabled, buffer enabled, DAC enabled
    DAC->CR = DAC_CR_DMAEN1 | DAC_CR_TEN1 | DAC_CR_BOFF1 | DAC_CR_EN1;

    //DMA | Channel 3, peripheral = DAC, memory = waveTable, size = 1
    //priority high, memory increment, circular mode, direction to peripheral, DMA enabled
    DMA1_Channel3->CPAR = (uint32_t) &DAC->DHR8R1;
    DMA1_Channel3->CMAR = (uint32_t) waveTable->sampleData;
    DMA1_Channel3->CNDTR = waveTable->sampleSize;
    DMA1_Channel3->CCR = DMA_CCR_PL_1 | DMA_CCR_PL_0 | DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_DIR | DMA_CCR_EN;

    //TIM6 | trigger = update, auto-reload preload enabled, timer enabled
    TIM6->ARR = (FREQ / (frequency * waveTable->sampleSize)) - 1;
    TIM6->CR2 = TIM_CR2_MMS_1;
    TIM6->EGR = TIM_EGR_UG;
    TIM6->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}

void DacSetFrequency(uint32_t freq, uint8_t update) {
    frequency = freq;
    if (update)
        DacUpdateSettings();
}

void DacSetWaveTable(WaveTable *wt, uint8_t update) {
    waveTable = wt;
    if (update)
        DacUpdateSettings();
}

void DacSetPredefinedWaveTable(enum PredefinedWaveTable table, uint8_t update) {
    switch (table) {
    case SINE_WAVE:
        DacSetWaveTable(&sineWaveTable, update);
        break;
    case IDLE_WAVE:
        DacSetWaveTable(&idleWaveTable, update);
        break;
    }
}

void DacUpdateSettings(void) {
    if (currentWaveTable != waveTable) {
        DMA1_Channel3->CCR &= ~DMA_CCR_EN;
        DMA1_Channel3->CMAR = (uint32_t) waveTable->sampleData;
        DMA1_Channel3->CNDTR = waveTable->sampleSize;
        DMA1_Channel3->CCR |= DMA_CCR_EN;
    }

    if (currentFrequency != frequency || currentWaveTable->sampleSize != waveTable->sampleSize) {
        TIM6->ARR = (FREQ / (frequency * waveTable->sampleSize)) - 1;
    }

    currentWaveTable = waveTable;
    currentFrequency = frequency;
}

void DacStopPlaying(void) {
    DacSetWaveTable(&idleWaveTable, 0);
    DacSetFrequency(1000, 1);
}
