/*
 * flash.c
 *
 *  Created on: 15.04.2017
 *      Author: korgeaux
 */

#include "misc/flash.h"

void FlashUnlock(void) {

    while (FLASH->SR & FLASH_SR_BSY)
        ;
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

void FlashLock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}

void FlashPageErase(uint32_t addr) {
    FlashUnlock();
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = addr;
    FLASH->CR |= FLASH_CR_STRT;
    while (FLASH->SR & FLASH_SR_BSY)
        ;
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR |= FLASH_SR_EOP;
    }
    FLASH->CR &= ~FLASH_CR_PER;
}

void FlashWriteHalfWord(uint32_t addr, uint16_t data) {
    FlashUnlock();
    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint16_t *) (addr) = data;
    while (FLASH->SR & FLASH_SR_BSY)
        ;
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR |= FLASH_SR_EOP;
    }
    FLASH->CR &= ~FLASH_CR_PG;
}
