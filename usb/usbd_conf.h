/*
 * usbd_conf.h
 *
 *  Created on: 17.04.2017
 *      Author: korgeaux
 */

#ifndef USB_USBD_CONF_H_
#define USB_USBD_CONF_H_

#include <stddef.h>
#include "stm32f0xx.h"

#define USBD_MAX_NUM_INTERFACES     3
#define USBD_MAX_NUM_CONFIGURATION  1
#define USBD_MAX_STR_DESC_SIZ       512
#define USBD_SUPPORT_USER_STRING    1
#define USBD_SELF_POWERED           1

#define USBD_ErrLog(...)

#endif /* USB_USBD_CONF_H_ */
