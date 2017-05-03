/*
 * usbd_desc.c
 *
 *  Created on: 17.04.2017
 *      Author: korgeaux
 */

#include "usbd_desc.h"
#include "usbd_conf.h"
#include "core/usbd_core.h"

#define USBD_VID                        0x0483
#define USBD_PID                        0x2002
#define USBD_LANGID_STRING              0x0409
#define USBD_MANUFACTURER_STRING        "korgeaux"
#define USBD_PRODUCT_STRING             "AprsTracker"
#define USBD_SERIALNUMBER_STRING        "00000000001A"
#define USBD_CONFIGURATION_STRING       "Configuration"
#define USBD_INTERFACE_STRING           "Interface"

uint8_t *USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

USBD_DescriptorsTypeDef FS_Desc = {
        USBD_FS_DeviceDescriptor,
        USBD_FS_LangIDStrDescriptor,
        USBD_FS_ManufacturerStrDescriptor,
        USBD_FS_ProductStrDescriptor,
        USBD_FS_SerialStrDescriptor,
        USBD_FS_ConfigStrDescriptor,
        USBD_FS_InterfaceStrDescriptor,
};

__ALIGN_BEGIN uint8_t USBD_FS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END = {
        0x12, //bLength
        USB_DESC_TYPE_DEVICE, //bDescriptorType
        0x00, //bcdUSB[0]
        0x02, //bcdUSB[1]
        0xEF, //bDeviceClass
        0x02, //bDeviceSubClass
        0x01, //bDeviceProtocol
        USB_MAX_EP0_SIZE, //bMaxPacketSize
        LOBYTE(USBD_VID), //idVendor[0]
        HIBYTE(USBD_VID), //idVendor[1]
        LOBYTE(USBD_PID), //idProduct[0]
        HIBYTE(USBD_PID), //idProduct[1]
        0x00, //bcdDevice[0]
        0x02, //bcdDevice[1]
        USBD_IDX_MFC_STR, //iManufacturer
        USBD_IDX_PRODUCT_STR, //iProduct
        USBD_IDX_SERIAL_STR, //iSerialNumber
        USBD_MAX_NUM_CONFIGURATION //bNumConfigurations
};

__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
        USB_LEN_LANGID_STR_DESC, //bLength
        USB_DESC_TYPE_STRING, //bDescriptorType
        LOBYTE(USBD_LANGID_STRING), //idLang[0]
        HIBYTE(USBD_LANGID_STRING), //idLang[1]
};

__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

uint8_t *USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
    *length = sizeof(USBD_FS_DeviceDesc);
    return USBD_FS_DeviceDesc;
}

uint8_t *USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
    *length = sizeof(USBD_LangIDDesc);
    return USBD_LangIDDesc;
}

uint8_t *USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
    USBD_GetString((uint8_t *) USBD_PRODUCT_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

uint8_t *USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
    USBD_GetString((uint8_t *) USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

uint8_t *USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
    USBD_GetString((uint8_t *) USBD_SERIALNUMBER_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

uint8_t *USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
    USBD_GetString((uint8_t *) USBD_CONFIGURATION_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

uint8_t *USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
    USBD_GetString((uint8_t *) USBD_INTERFACE_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}
