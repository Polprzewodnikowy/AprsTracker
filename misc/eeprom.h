/*
 * eeprom.h
 *
 *  Created on: 14.04.2017
 *      Author: korgeaux
 */

#ifndef MISC_EEPROM_H_
#define MISC_EEPROM_H_

enum {
    EE_OK = 0, EE_NO_VALID_PAGE, EE_VARIABLE_NOT_FOUND, EE_PAGE_FULL, EE_VADDR_TOO_HIGH,
};

#define EE_START_ADDR   0x0801F000

#define EE_PAGE_SIZE    2048

#define EE_PAGE0_BASE   (EE_START_ADDR)
#define EE_PAGE1_BASE   (EE_START_ADDR + EE_PAGE_SIZE)
#define EE_FIRST_PAGE   EE_PAGE0_BASE
#define EE_LAST_PAGE    EE_PAGE1_BASE

#define EE_VALID_PAGE   0x0000
#define EE_EMPTY_PAGE   0xFFFF

#define EE_VAR_MAX      (EE_PAGE_SIZE / 4)

void EepromInit(void);
uint8_t EepromFormat(void);
uint8_t EepromWrite(uint16_t addr, uint16_t data);
uint8_t EepromRead(uint16_t addr, uint16_t *data);

#endif /* MISC_EEPROM_H_ */
