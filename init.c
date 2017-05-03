/*
 * init.c
 *
 *  Created on: 17.12.2016
 *      Author: korgeaux
 */

#include "misc/def.h"

#include "afsk/afsk.h"
#include "aprs/aprs.h"
#include "gps/gps.h"
#include "misc/config.h"
#include "misc/eeprom.h"
#include "misc/gpio.h"
#include "tracker/tracker.h"
#include "usb/usb_device.h"

void SystemInit(void) {
    FLASH->ACR = FLASH_ACR_LATENCY;

    RCC->CR |= RCC_CR_HSEBYP | RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY))
        ;

    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL12;
    RCC->CFGR2 = RCC_CFGR2_PREDIV_DIV2;

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;

    RCC->AHBENR |= RCC_AHBENR_DMAEN;

    GpioInit();
    ConfigInit();
    Config *config = ConfigGet();
    AfskInit(config);
    AX25Init();
    GpsInit(config);
    TrackerInit(config);
    UsbInit();

    GpioConfig(GPIOC, 6, GPIO_OUT_PP_LOW);
    GpioConfig(GPIOC, 7, GPIO_OUT_PP_LOW);
    GpioConfig(GPIOC, 8, GPIO_OUT_PP_LOW);
    GpioConfig(GPIOC, 9, GPIO_OUT_PP_LOW);

    SysTick_Config(FREQ / 1000);
}
