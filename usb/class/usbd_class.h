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

#define CDC_CMD_EP                      0x82
#define CDC_IN_EP                       0x81
#define CDC_OUT_EP                      0x01

#define CDC_DATA_PACKET_SIZE            64
#define CDC_CMD_PACKET_SIZE             8

#define CDC_RX_BUFFER_SIZE				256
#define CDC_TX_BUFFER_SIZE				256

#define CDC_REQ_SET_LINE_CODING         0x20
#define CDC_REQ_GET_LINE_CODING         0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE  0x22

enum {
	CDC_TX_STATE_FREE,
	CDC_TX_STATE_BUSY,
};

enum {
	CDC_PORT_STATE_CLOSED,
	CDC_PORT_STATE_OPEN,
};

typedef struct {
	uint8_t *buffer;
	uint8_t *head;
	uint8_t *tail;
} cdcFifo;

typedef struct {
    uint32_t dwDTERate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
} LineCoding;

void USBD_CDC_TransmitData(USBD_HandleTypeDef *pdev, uint8_t *buffer, size_t size);

#endif /* USB_CLASS_USBD_CLASS_H_ */
