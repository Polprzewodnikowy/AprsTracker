/*
 * usb_device.c
 *
 *  Created on: 17.04.2017
 *      Author: korgeaux
 */

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_class.h"

USBD_HandleTypeDef hUsbDeviceFS;

void UsbInit(void) {
    USBD_Init(&hUsbDeviceFS, &FS_Desc, 0);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_Class);
    USBD_Start(&hUsbDeviceFS);
}
