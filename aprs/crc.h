/*
 * crc.h
 *
 *  Created on: 26.12.2016
 *      Author: korgeaux
 */

#ifndef APRS_CRC_H_
#define APRS_CRC_H_

enum CrcPoly {
	CRC_POLY_7,
	CRC_POLY_8,
	CRC_POLY_16,
	CRC_POLY_32,
};

enum CrcInv {
	CRC_NO_INV,
	CRC_INV_BYTE,
	CRC_INV_HALFWORD,
	CRC_INV_WORD,
	CRC_INV,
};

void CrcInit(uint32_t poly, uint32_t init, enum CrcPoly polyWidth, enum CrcInv invIn, enum CrcInv invOut);
void CrcReset(void);
uint32_t CrcUpdate8(uint8_t byte);
uint32_t CrcUpdate16(uint8_t halfword);
uint32_t CrcUpdate32(uint32_t word);
uint32_t CrcGet(void);

#endif /* APRS_CRC_H_ */
