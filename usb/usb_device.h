/*
 * usb_device.h
 *
 *  Created on: 17.04.2017
 *      Author: korgeaux
 */

#ifndef USB_USB_DEVICE_H_
#define USB_USB_DEVICE_H_

#include "usbd_def.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

void UsbInit(void);
void UsbTransmitCDC(uint8_t *data, int length);

#endif /* USB_USB_DEVICE_H_ */
