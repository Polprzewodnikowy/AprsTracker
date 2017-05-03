/*
 * flash.h
 *
 *  Created on: 15.04.2017
 *      Author: korgeaux
 */

#ifndef MISC_FLASH_H_
#define MISC_FLASH_H_

#include "misc/def.h"

void FlashUnlock(void);
void FlashLock(void);
void FlashPageErase(uint32_t addr);
void FlashWriteHalfWord(uint32_t addr, uint16_t data);

#endif /* MISC_FLASH_H_ */
