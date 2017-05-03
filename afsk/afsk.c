/*
 * afsk.c
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#include "afsk/afsk.h"

#include "afsk/dac.h"
#include "misc/fifo.h"
#include "misc/gpio.h"
#include "protocol/hdlc.h"

static Config *config;
static FifoBuffer txFifo;
static uint8_t txBuffer[AFSK_FIFO_SIZE];
static volatile uint32_t preambleRemaining, tailRemaining, bitStuffCount, lastTone;
static uint32_t preambleLength, tailLength;
static volatile uint8_t currentByte, currentBit;
static volatile bool bitStuffAllowed, sending;

static void txStart(void);
static void txStop(void);
static void switchTone(void);

void AfskInit(Config *c) {
    config = c;

    DacInit();
    
    FifoInit(&txFifo, txBuffer, AFSK_FIFO_SIZE);

    GpioConfig(PTT_GPIO, PTT_PIN, GPIO_OUT_PP_HIGH);

    //TIM14 | 1200Hz interrupt
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->ARR = (FREQ / AFSK_BAUD) - 1;
    TIM14->DIER = TIM_DIER_UIE;
    TIM14->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;
    NVIC_SetPriority(TIM14_IRQn, 1);
    NVIC_EnableIRQ(TIM14_IRQn);

    preambleRemaining = 0;
    tailRemaining = 0;
    bitStuffCount = 0;
    bitStuffAllowed = false;
    currentByte = 0;
    currentBit = 0;
    sending = false;
    AfskSetPreambleLength(config->afsk.preamble);
    AfskSetTailLength(config->afsk.tail);
}

void AfskSetPreambleLength(uint32_t length) {
    preambleLength = (length * 100) / (100000 / (AFSK_BAUD / 8));
}

void AfskSetTailLength(uint32_t length) {
    tailLength = (length * 100) / (100000 / (AFSK_BAUD / 8));
}

void AfskPutChar(char c) {
    txStart();
    while (FifoIsFull(&txFifo));
        ;
    FifoPush(&txFifo, c);
}

void AfskTransmit(char *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        AfskPutChar(buffer[i]);
    }
}

static void txStart(void) {
    if (!sending) {
        sending = true;
        preambleRemaining = preambleLength;
        tailRemaining = tailLength;
        DacSetPredefinedWaveTable(SINE_WAVE, 0);
        DacSetFrequency(AFSK_MARK_TONE, 1);
        lastTone = AFSK_MARK_TONE;
        PTT_GPIO->BSRR = 1 << PTT_PIN;
        TIM14->CR1 |= TIM_CR1_CEN;
        GPIOC->BSRR = GPIO_BSRR_BS_7;
    }
}

static void txStop(void) {
    if (sending) {
        GPIOC->BSRR = GPIO_BSRR_BR_7;
        TIM14->CR1 &= ~TIM_CR1_CEN;
        DacStopPlaying();
        PTT_GPIO->BRR = 1 << PTT_PIN;
        sending = false;
    }
}

static void switchTone(void) {
    if (lastTone == AFSK_MARK_TONE)
        lastTone = AFSK_SPACE_TONE;
    else if (lastTone == AFSK_SPACE_TONE)
        lastTone = AFSK_MARK_TONE;
    DacSetFrequency(lastTone, 1);
}

void TIM14_IRQHandler(void) {
    TIM14->SR = ~TIM_SR_UIF;

    if (currentBit == 0) {
        if (FifoIsEmpty(&txFifo) && !tailRemaining) {
            txStop();
            return;
        } else {
            if (!bitStuffAllowed)
                bitStuffCount = 0;
            bitStuffAllowed = true;
            if (!preambleRemaining) {
                if (FifoIsEmpty(&txFifo)) {
                    --tailRemaining;
                    currentByte = HDLC_FLAG;
                } else {
                    tailRemaining = tailLength;
                    currentByte = FifoPop(&txFifo);
                }
            } else {
                --preambleRemaining;
                currentByte = HDLC_FLAG;
            }
            if (currentByte == AX25_ESC) {
                if (FifoIsEmpty(&txFifo)) {
                    txStop();
                } else {
                    currentByte = FifoPop(&txFifo);
                }
            } else if (currentByte == HDLC_FLAG || currentByte == HDLC_RESET) {
                bitStuffAllowed = false;
            }
        }
        currentBit = 1;
    }

    if (bitStuffAllowed && bitStuffCount >= 5) {
        switchTone();
        bitStuffCount = 0;
    } else {
        if (currentByte & currentBit) {
            ++bitStuffCount;
        } else {
            switchTone();
            bitStuffCount = 0;
        }
        currentBit <<= 1;
    }
}
