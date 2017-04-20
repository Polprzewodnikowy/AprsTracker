/*
 * stm32f0xx_hal_conf.h
 *
 *  Created on: 17.04.2017
 *      Author: korgeaux
 */

#ifndef USB_STM32F0XX_HAL_CONF_H_
#define USB_STM32F0XX_HAL_CONF_H_

#define HAL_PCD_MODULE_ENABLED

#ifdef HAL_PCD_MODULE_ENABLED
    #include "stm32f0xx_hal_pcd.h"
#endif

#define assert_param(expr)  ((void)0)

#endif /* USB_STM32F0XX_HAL_CONF_H_ */
