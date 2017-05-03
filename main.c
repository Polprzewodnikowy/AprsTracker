/*
 * main.c
 *
 *  Created on: 17.12.2016
 *      Author: korgeaux
 */

#include "misc/def.h"

#include "gps/gps.h"
#include "tracker/tracker.h"
#include "misc/config.h"

volatile int tickTimer, tickUpdate;

int main(void) {
    while (1) {
        GpsProcess();
        if (tickUpdate) {
            tickUpdate = 0;
            TrackerUpdate();
        }
    }
}

void SysTick_Handler(void) {
    if (tickTimer < 50) {
       GPIOC->BSRR = GPIO_BSRR_BS_8;
    } else {
       GPIOC->BSRR = GPIO_BSRR_BR_8;
    }
    if (tickTimer++ >= 1000) {
        tickTimer = 0;
        tickUpdate = 1;
    }
}
