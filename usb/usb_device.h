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

enum {
    TNC_PORT,
    CONFIG_PORT,
};

void UsbInit(void);

#endif /* USB_USB_DEVICE_H_ */
