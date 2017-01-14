/*
 * gpio.c
 *
 *  Created on: 27.12.2016
 *  Based on work of: freddiechopin.info
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "gpio.h"

void GpioInit(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN
            | RCC_AHBENR_GPIOFEN;
}

void GpioConfig(GPIO_TypeDef *gpio, uint32_t pin, uint32_t configuration) {
    uint32_t moder, otyper, ospeedr, pupdr, afr, afrx;

    moder = gpio->MODER;
    moder &= ~(GPIO_MODER_mask << (pin * 2));
    moder |= (GPIO_GET_MODER(configuration) << (pin * 2));
    gpio->MODER = moder;

    otyper = gpio->OTYPER;
    otyper &= ~(GPIO_OTYPER_mask << pin);
    otyper |= (GPIO_GET_OTYPER(configuration) << pin);
    gpio->OTYPER = otyper;

    ospeedr = gpio->OSPEEDR;
    ospeedr &= ~(GPIO_OSPEEDR_mask << (pin * 2));
    ospeedr |= (GPIO_GET_OSPEEDR(configuration) << (pin * 2));
    gpio->OSPEEDR = ospeedr;

    pupdr = gpio->PUPDR;
    pupdr &= ~(GPIO_PUPDR_mask << (pin * 2));
    pupdr |= (GPIO_GET_PUPDR(configuration) << (pin * 2));
    gpio->PUPDR = pupdr;

    if (pin < 8) {
        afrx = 0;
    } else {
        afrx = 1;
        pin -= 8;
    }

    afr = gpio->AFR[afrx];
    afr &= ~(GPIO_AFRx_mask << (pin * 4));
    afr |= (GPIO_GET_AFR(configuration) << (pin * 4));
    gpio->AFR[afrx] = afr;
}
