/*
 * gps.c
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#include "gps/gps.h"

#include "misc/gpio.h"

static Config *config;
static char dmaBuffer[GPS_BUFFER_SIZE];
static char lineBuffer[GPS_BUFFER_SIZE];
static volatile bool lineAvaiable;
static struct minmea_sentence_rmc rmc;
static struct minmea_sentence_gga gga;

void GpsInit(Config *c) {
    config = c;
    GpioConfig(GPIOA, 1, GPIO_AF4_PP_HIGH);

    RCC->APB1ENR |= RCC_APB1ENR_USART4EN;

    DMA1_Channel6->CMAR = (uint32_t) dmaBuffer;
    DMA1_Channel6->CPAR = (uint32_t) &USART4->RDR;
    DMA1_Channel6->CNDTR = GPS_BUFFER_SIZE;
    DMA1_Channel6->CCR = DMA_CCR_MINC | DMA_CCR_EN;

    USART4->BRR = FREQ / config->gps.baud;
    USART4->CR2 = '\n' << USART_CR2_ADD_Pos;
    USART4->CR3 = USART_CR3_OVRDIS | USART_CR3_DMAR;
    USART4->CR1 = USART_CR1_CMIE | USART_CR1_RE | USART_CR1_UE;

    NVIC_EnableIRQ(USART3_4_IRQn);
    NVIC_SetPriority(USART3_4_IRQn, 3);
}

void GpsProcess(void) {
    if (lineAvaiable) {
        lineAvaiable = false;
        enum minmea_sentence_id id = minmea_sentence_id(lineBuffer, false);
        if (id == MINMEA_SENTENCE_RMC) {
            minmea_parse_rmc(&rmc, lineBuffer);
        } else if (id == MINMEA_SENTENCE_GGA) {
            minmea_parse_gga(&gga, lineBuffer);
        }
    }
}

int GpsIsValid(void) {
    return rmc.valid;
}

struct minmea_sentence_rmc *GpsGetRMC(void) {
    return (struct minmea_sentence_rmc *) &rmc;
}

struct minmea_sentence_gga *GpsGetGGA(void) {
    return (struct minmea_sentence_gga *) &gga;
}

void USART3_4_IRQHandler(void) {
    if (USART4->ISR & USART_ISR_CMF) {
        USART4->ICR = USART_ICR_CMCF;

        DMA1_Channel6->CCR &= ~DMA_CCR_EN;
        int length = GPS_BUFFER_SIZE - DMA1_Channel6->CNDTR;

        memset(lineBuffer, 0, GPS_BUFFER_SIZE);
        memcpy(lineBuffer, dmaBuffer, length);

        lineAvaiable = true;

        DMA1_Channel6->CNDTR = GPS_BUFFER_SIZE;
        DMA1_Channel6->CCR |= DMA_CCR_EN;
    }
}
