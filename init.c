/*
 * init.c
 *
 *  Created on: 17.12.2016
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "gpio.h"
#include "config.h"
#include "aprs.h"
#include "gps.h"
#include "tracker.h"

void SystemInit(void)
{
	FLASH->ACR = FLASH_ACR_LATENCY;

	RCC->CR |= RCC_CR_HSEBYP | RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));

	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL12;
	RCC->CFGR2 = RCC_CFGR2_PREDIV_DIV2;

	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

	RCC->AHBENR |= RCC_AHBENR_DMAEN;

	GpioInit();
	ConfigInit();
	AprsInit();
	GpsInit();
	TrackerInit();

	GpioConfig(GPIOC, 6, GPIO_OUT_PP_LOW);
	GpioConfig(GPIOC, 7, GPIO_OUT_PP_LOW);
	GpioConfig(GPIOC, 8, GPIO_OUT_PP_LOW);
	GpioConfig(GPIOC, 9, GPIO_OUT_PP_LOW);
}
