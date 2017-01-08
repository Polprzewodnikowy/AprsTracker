/*
 * gps.c
 *
 *  Created on: 27.12.2016
 *      Author: korgeaux
 */

#include <string.h>
#include "stm32f0xx.h"
#include "gps.h"
#include "gpio.h"
#include "minmea.h"
#include "config.h"

char buffer[GPS_BUFFER_SIZE];
volatile struct minmea_sentence_rmc rmc;
volatile struct minmea_sentence_gga gga;

void GpsInit(void)
{
	Config *c = ConfigGet();

	GpioConfig(GPIOA, 1, GPIO_AF4_PP_HIGH);

	RCC->APB1ENR |= RCC_APB1ENR_USART4EN;

	DMA1_Channel6->CMAR = (uint32_t)buffer;
	DMA1_Channel6->CPAR = (uint32_t)&USART4->RDR;
	DMA1_Channel6->CNDTR = GPS_BUFFER_SIZE;
	DMA1_Channel6->CCR = DMA_CCR_MINC | DMA_CCR_EN;

	USART4->BRR = FREQ / c->gpsBaud;
	USART4->CR2 = '\n' << USART_CR2_ADD_Pos;
	USART4->CR3 = USART_CR3_OVRDIS | USART_CR3_DMAR;
	USART4->CR1 = USART_CR1_CMIE | USART_CR1_RE | USART_CR1_UE;

	NVIC_EnableIRQ(USART3_4_IRQn);
	NVIC_SetPriority(USART3_4_IRQn, 3);
}

int GpsIsValid(void)
{
	return rmc.valid;
}

struct minmea_sentence_rmc *GpsGetRMC(void)
{
	return (struct minmea_sentence_rmc *)&rmc;
}

struct minmea_sentence_gga *GpsGetGGA(void)
{
	return (struct minmea_sentence_gga *)&gga;
}

void USART3_4_IRQHandler(void)
{
	if (USART4->ISR & USART_ISR_CMF) {
		USART4->ICR = USART_ICR_CMCF;

		char line[GPS_BUFFER_SIZE];
		int len;

		DMA1_Channel6->CCR &= ~DMA_CCR_EN;
		len = GPS_BUFFER_SIZE - DMA1_Channel6->CNDTR;

		memset(line, 0, GPS_BUFFER_SIZE);
		memcpy(line, buffer, len);

		enum minmea_sentence_id id = minmea_sentence_id(line, false);
		if (id == MINMEA_SENTENCE_RMC) {
			struct minmea_sentence_rmc tmp;
			minmea_parse_rmc(&tmp, line);
			rmc = tmp;
		} else if (id == MINMEA_SENTENCE_GGA) {
			struct minmea_sentence_gga tmp;
			minmea_parse_gga(&tmp, line);
			gga = tmp;
		}

		DMA1_Channel6->CNDTR = GPS_BUFFER_SIZE;
		DMA1_Channel6->CCR |= DMA_CCR_EN;
	}
}
