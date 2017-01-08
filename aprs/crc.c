/*
 * crc.c
 *
 *  Created on: 26.12.2016
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "crc.h"

void CrcInit(uint32_t poly, uint32_t init, enum CrcPoly polyWidth, enum CrcInv invIn, enum CrcInv invOut)
{
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
	CRC->POL = poly;
	CRC->INIT = init;

	uint32_t crc_cr = 0;

	switch (polyWidth) {
	case CRC_POLY_7:
		crc_cr |= CRC_CR_POLYSIZE;
		break;
	case CRC_POLY_8:
		crc_cr |= CRC_CR_POLYSIZE_1;
		break;
	case CRC_POLY_16:
		crc_cr |= CRC_CR_POLYSIZE_0;
		break;
	case CRC_POLY_32:
	default:
		break;
	}

	switch (invIn) {
	case CRC_INV_BYTE:
		crc_cr |= CRC_CR_REV_IN_0;
		break;
	case CRC_INV_HALFWORD:
		crc_cr |= CRC_CR_REV_IN_1;
		break;
	case CRC_INV_WORD:
		crc_cr |= CRC_CR_REV_IN;
		break;
	case CRC_NO_INV:
	default:
		break;
	}

	if(invOut == CRC_INV)
		crc_cr |= CRC_CR_REV_OUT;

	CRC->CR = crc_cr | CRC_CR_RESET;
}

void CrcReset(void)
{
	CRC->CR |= CRC_CR_RESET;
}

uint32_t CrcUpdate8(uint8_t byte)
{
	*(__IO uint8_t *)&CRC->DR = byte;
	return CRC->DR;
}

uint32_t CrcUpdate16(uint8_t halfword)
{
	*(__IO uint16_t *)&CRC->DR = halfword;
	return CRC->DR;
}

uint32_t CrcUpdate32(uint32_t word)
{
	*(__IO uint32_t *)&CRC->DR = word;
	return CRC->DR;
}

uint32_t CrcGet(void)
{
	return CRC->DR;
}
