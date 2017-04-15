/*
 * afsk.c
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "afsk.h"
#include "gpio.h"
#include "dac.h"
#include "hdlc.h"
#include "config.h"

static volatile char fifo[AFSK_FIFO_SIZE], *fifoIn, *fifoOut;
static volatile uint32_t preambleRemaining, tailRemaining, bitStuffCount, lastTone;
static volatile uint8_t currentByte, currentBit, bitStuffAllowed, sending;
static volatile uint32_t preambleLength, tailLength;

static void AfskSwitchTone(void);
static void AfskStartTone(uint32_t tone);
static void AfskStopTone(void);
static void AfskSetPTT(uint8_t state);
static void AfskFifoPush(char c);
static char AfskFifoPop(void);
static uint8_t AfskFifoIsEmpty(void);
static uint8_t AfskFifoIsFull(void);

void AfskInit(void) {
    Config *config = ConfigGet();

    DacInit();

    GpioConfig(PTT_GPIO, PTT_PIN, GPIO_OUT_PP_HIGH);

    //TIM14 | 1200Hz interrupt
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->ARR = (FREQ / AFSK_BAUD) - 1;
    TIM14->DIER = TIM_DIER_UIE;
    TIM14->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;
    NVIC_SetPriority(TIM14_IRQn, 0);
    NVIC_EnableIRQ(TIM14_IRQn);

    preambleRemaining = 0;
    tailRemaining = 0;
    bitStuffCount = 0;
    bitStuffAllowed = 0;
    currentByte = 0;
    currentBit = 0;
    preambleLength = (config->afsk.preamble * 100) / (100000 / (AFSK_BAUD / 8));
    tailLength = (config->afsk.tail * 100) / (100000 / (AFSK_BAUD / 8));
    fifoIn = fifoOut = fifo;
}

void AfskSetPreambleLength(uint32_t length) {
    preambleLength = length;
}

void AfskSetTailLength(uint32_t length) {
    tailLength = length;
}

void AfskTxStart(void) {
    if (!sending) {
        sending = 1;
        preambleRemaining = preambleLength;
        tailRemaining = tailLength;
        AfskStartTone(AFSK_MARK_TONE);
        AfskSetPTT(1);
        TIM14->CR1 |= TIM_CR1_CEN;
        GPIOC->BSRR = GPIO_BSRR_BS_7;
    }
}

void AfskTxStop(void) {
    if (sending) {
        GPIOC->BSRR = GPIO_BSRR_BR_7;
        TIM14->CR1 &= ~TIM_CR1_CEN;
        AfskStopTone();
        AfskSetPTT(0);
        sending = 0;
    }
}

void AfskPutChar(char c) {
    AfskTxStart();
    while (AfskFifoIsFull())
        ;
    AfskFifoPush(c);
}

void AfskTransmit(char *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        AfskPutChar(buffer[i]);
    }
}

void TIM14_IRQHandler(void) {
    TIM14->SR = ~TIM_SR_UIF;

    if (currentBit == 0) {
        if (AfskFifoIsEmpty() && !tailRemaining) {
            AfskTxStop();
            return;
        } else {
            if (!bitStuffAllowed)
                bitStuffCount = 0;
            bitStuffAllowed = 1;
            if (!preambleRemaining) {
                if (AfskFifoIsEmpty()) {
                    --tailRemaining;
                    currentByte = HDLC_FLAG;
                } else {
                    tailRemaining = tailLength;
                    currentByte = AfskFifoPop();
                }
            } else {
                --preambleRemaining;
                currentByte = HDLC_FLAG;
            }
            if (currentByte == AX25_ESC) {
                if (AfskFifoIsEmpty()) {
                    AfskTxStop();
                } else {
                    currentByte = AfskFifoPop();
                }
            } else if (currentByte == HDLC_FLAG || currentByte == HDLC_RESET) {
                bitStuffAllowed = 0;
            }
        }
        currentBit = 1;
    }

    if (bitStuffAllowed && bitStuffCount >= 5) {
        AfskSwitchTone();
        bitStuffCount = 0;
    } else {
        if (currentByte & currentBit) {
            ++bitStuffCount;
        } else {
            AfskSwitchTone();
            bitStuffCount = 0;
        }
        currentBit <<= 1;
    }
}

static void AfskSwitchTone(void) {
    if (lastTone == AFSK_MARK_TONE)
        lastTone = AFSK_SPACE_TONE;
    else if (lastTone == AFSK_SPACE_TONE)
        lastTone = AFSK_MARK_TONE;
    DacChangeFrequency(lastTone);
}

static void AfskStartTone(uint32_t tone) {
    DacSetWaveTableSource(DacGetPredefinedWaveTable(SINE_WAVE),
    SINE_WAVETABLE_SIZE);
    DacChangeFrequency(tone);
    lastTone = tone;
}

static void AfskStopTone(void) {
    DacStopWaveGeneration();
}

static void AfskSetPTT(uint8_t state) {
    if (state)
        PTT_GPIO->BSRR = 1 << PTT_PIN;
    else
        PTT_GPIO->BRR = 1 << PTT_PIN;
}

static void AfskFifoPush(char c) {
    *fifoIn++ = c;
    if (fifoIn >= (fifo + AFSK_FIFO_SIZE))
        fifoIn = fifo;
}

static char AfskFifoPop(void) {
    char c = *fifoOut++;
    if (fifoOut >= (fifo + AFSK_FIFO_SIZE))
        fifoOut = fifo;
    return c;
}

static uint8_t AfskFifoIsEmpty(void) {
    return fifoIn == fifoOut;
}

static uint8_t AfskFifoIsFull(void) {
    return (((fifoOut == fifo) && (fifoIn == (fifo + AFSK_FIFO_SIZE - 1))) || (fifoIn == (fifoOut - 1)));
}
