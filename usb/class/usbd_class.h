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

#define TNC_CMD_EP                      0x83
#define TNC_IN_EP                       0x81
#define TNC_OUT_EP                      0x01
#define CONFIG_CMD_EP                   0x84
#define CONFIG_IN_EP                    0x82
#define CONFIG_OUT_EP                   0x02

#define CDC_DATA_PACKET_SIZE            64
#define CDC_CMD_PACKET_SIZE             8

#define CDC_REQ_SET_LINE_CODING         0x20
#define CDC_REQ_GET_LINE_CODING         0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE  0x22

typedef struct {
    uint32_t dwDTERate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
} LineCoding;

#endif /* USB_CLASS_USBD_CLASS_H_ */
