/*
 * main.c
 *
 *  Created on: 17.12.2016
 *      Author: korgeaux
 */

#include "stm32f0xx.h"
#include "tracker.h"

volatile int trackerTimer, trackerUpdate;

int main(void)
{
	trackerTimer = 0;
	trackerUpdate = 1;
	SysTick_Config(FREQ / 1000);
	while (1) {
		if (trackerUpdate) {
			trackerUpdate = 0;
			TrackerUpdate();
		}
	}
}

void SysTick_Handler(void)
{
	if (trackerTimer++ >= 1000) {
		trackerTimer = 0;
		trackerUpdate = 1;
	}
}
