/*
 * eeprom.c
 *
 *  Created on: 14.04.2017
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "eeprom.h"
#include "flash.h"

static uint8_t TransferPage(uint16_t addr, uint16_t data);
static uint32_t FindValidPage(void);

void EepromInit(void) {
    if (!FindValidPage()) {
        EepromFormat();
    }
}

uint8_t EepromFormat(void) {
    FlashPageErase(EE_PAGE0_BASE);
    FlashPageErase(EE_PAGE1_BASE);
    FlashWriteHalfWord(EE_PAGE0_BASE, EE_VALID_PAGE);
    return EE_OK;
}

uint8_t EepromWrite(uint16_t addr, uint16_t data) {
    if (addr > EE_VAR_MAX) {
        return EE_VADDR_TOO_HIGH;
    }

    uint32_t page = FindValidPage();

    if (!page) {
        return EE_NO_VALID_PAGE;
    }

    for (uint32_t i = (page + 2); i < (page + EE_PAGE_SIZE); i += 4) {
        uint16_t va = *(__IO uint16_t *) (i);
        uint16_t ed = *(__IO uint16_t *) (i + 2);

        if (va == 0xFFFF && ed == 0xFFFF) {
            FlashWriteHalfWord(i, addr);
            FlashWriteHalfWord(i + 2, data);
            return EE_OK;
        }
    }

    return TransferPage(addr, data);
}

uint8_t EepromRead(uint16_t addr, uint16_t *data) {
    if (addr > EE_VAR_MAX) {
        return EE_VADDR_TOO_HIGH;
    }

    uint32_t page = FindValidPage();

    if (!page) {
        return EE_NO_VALID_PAGE;
    }

    for (uint32_t i = (page + EE_PAGE_SIZE - 2); i >= (page + 2); i -= 4) {
        uint16_t va = *(__IO uint16_t *) (i);

        if (va == addr) {
            *data = *(__IO uint16_t *) (i + 2);
            return EE_OK;
        }
    }

    *data = 0;
    return EE_VARIABLE_NOT_FOUND;
}

static uint8_t TransferPage(uint16_t addr, uint16_t data) {
    uint32_t page = FindValidPage();

    if (!page) {
        return EE_NO_VALID_PAGE;
    }

    uint32_t currentPage = page;
    uint32_t nextPage;

    if ((currentPage + EE_PAGE_SIZE) > EE_LAST_PAGE) {
        nextPage = EE_FIRST_PAGE;
    } else {
        nextPage = currentPage + EE_PAGE_SIZE;
    }

    FlashPageErase(nextPage);

    FlashWriteHalfWord(nextPage + 2, addr);
    FlashWriteHalfWord(nextPage + 4, data);

    int pt = nextPage + 6;

    for (int va = 0; va <= EE_VAR_MAX; va++) {
        uint16_t ed;
        uint8_t s = EepromRead(va, &ed);
        if (s == EE_OK) {
            FlashWriteHalfWord(pt, addr);
            FlashWriteHalfWord(pt + 2, data);
            pt += 4;
        }
    }

    FlashPageErase(currentPage);
    FlashWriteHalfWord(nextPage, 0);

    return EE_OK;
}

static uint32_t FindValidPage(void) {
    uint16_t page0Status = *(__IO uint16_t *) (EE_PAGE0_BASE);
    uint16_t page1Status = *(__IO uint16_t *) (EE_PAGE1_BASE);

    if (page0Status == EE_VALID_PAGE) {
        if (page1Status == EE_EMPTY_PAGE) {
            return EE_PAGE0_BASE;
        } else if (page1Status == EE_VALID_PAGE) {
            return 0;
        }
    } else if (page1Status == EE_VALID_PAGE) {
        if (page0Status == EE_EMPTY_PAGE) {
            return EE_PAGE1_BASE;
        } else if (page0Status == EE_VALID_PAGE) {
            return 0;
        }
    }

    return 0;
}
