/*
 * usbd_class.h
 *
 *  Created on: 17.04.2017
 *      Author: korgeaux
 */

#ifndef USB_CLASS_USBD_CLASS_H_
#define USB_CLASS_USBD_CLASS_H_

#include "usbd_def.h"

extern USBD_ClassTypeDef USBD_Class;

#define CDC_IN_EP                       0x81
#define CDC_OUT_EP                      0x01
#define CDC_CMD_EP                      0x82
#define CONFIG_IN_EP                    0x83
#define CONFIG_OUT_EP                   0x03

#define CDC_DATA_PACKET_SIZE            64
#define CDC_CMD_PACKET_SIZE             8
#define CONFIG_PACKET_SIZE              64

#define CDC_REQ_SET_LINE_CODING         0x20
#define CDC_REQ_GET_LINE_CODING         0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE  0x22

#endif /* USB_CLASS_USBD_CLASS_H_ */
