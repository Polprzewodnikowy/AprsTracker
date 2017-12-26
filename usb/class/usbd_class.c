/*
 * usbd_class.c
 *
 *  Created on: 17.04.2017
 *      Author: korgeaux
 */

#include "usbd_class.h"
#include "usbd_core.h"


static uint8_t cdcRxBuffer[CDC_DATA_PACKET_SIZE];
static uint8_t cdcTxBuffer[CDC_DATA_PACKET_SIZE];
static cdcFifo cdcRxFifo, cdcTxFifo;
static uint8_t cdcRxFifoBuffer[CDC_RX_BUFFER_SIZE];
static uint8_t cdcTxFifoBuffer[CDC_TX_BUFFER_SIZE];
static uint8_t txState, portState;
static LineCoding lineCoding = { 9600, 0, 0, 8 };

static uint8_t USBD_Class_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_Class_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_Class_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_Class_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_Class_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_Class_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t *USBD_Class_GetConfigDescriptor(uint16_t *length);
uint8_t *USBD_Class_GetUsrStrDescriptor(struct _USBD_HandleTypeDef *pdev, uint8_t index, uint16_t *length);

USBD_ClassTypeDef USBD_Class = {
        USBD_Class_Init, //Init
        USBD_Class_DeInit, //DeInit
        USBD_Class_Setup, //Setup
        NULL, //EP0_TxSent
        USBD_Class_EP0_RxReady, //EP0_RxReady
		USBD_Class_DataIn, //DataIn
        USBD_Class_DataOut, //DataOut
        NULL, //SOF
        NULL, //IsoINIncomplete
        NULL, //IsoOUTIncomplete
        USBD_Class_GetConfigDescriptor, //GetHSConfigDescriptor
        USBD_Class_GetConfigDescriptor, //GetFSConfigDescriptor
        USBD_Class_GetConfigDescriptor, //GetOtherSpeedConfigDescriptor
        NULL, //GetDeviceQualifierDescriptor
        USBD_Class_GetUsrStrDescriptor, //GetUsrStrDescriptor
};

__ALIGN_BEGIN static uint8_t USBD_Class_ConfigDescriptor[0x4B] __ALIGN_END = {
        //Configuration Descriptor:
        //------------------------------
        0x09, //bLength
        USB_DESC_TYPE_CONFIGURATION, //bDescriptorType
        0x4B, //wTotalLength[0]
        0x00, //wTotalLength[1]
        0x02, //bNumInterfaces
        0x01, //bConfigurationValue
        0x04, //iConfiguration ("Configuration")
        0xE0, //bmAttributes (Self-powered Device, Remote-Wakeup)
        0x32, //bMaxPower (100 mA)

        //Interface Association Descriptor:
        //------------------------------
        0x08, //bLength
        0x0B, //bDescriptorType
        0x00, //bFirstInterface
        0x02, //bInterfaceCount
        0x02, //bFunctionClass (Communication Device Class)
        0x02, //bFunctionSubClass (Abstract Control Model)
        0x00, //bFunctionProtocol
        0x06, //iFunction ("AprsTracker")

        //Interface Descriptor:
        //------------------------------
        0x09, //bLength
        USB_DESC_TYPE_INTERFACE, //bDescriptorType
        0x00, //bInterfaceNumber
        0x00, //bAlternateSetting
        0x01, //bNumEndPoints
        0x02, //bInterfaceClass (Communication Device Class)
        0x02, //bInterfaceSubClass (Abstract Control Model)
        0x01, //bInterfaceProtocol (ITU-T V.250)
        0x06, //iInterface ("AprsTracker")

        //CDC Header Functional Descriptor:
        //------------------------------
        0x05, //bFunctionalLength
        0x24, //bDescriptorType
        0x00, //bDescriptorSubtype
        0x10, //bcdCDC[0]
        0x01, //bcdCDC[1]

        //CDC Call Management Functional Descriptor:
        //------------------------------
        0x05, //bFunctionalLength
        0x24, //bDescriptorType
        0x01, //bDescriptorSubtype
        0x00, //bmCapabilities
        0x01, //bDataInterface

        //CDC Abstract Control Management Functional Descriptor:
        //------------------------------
        0x04, //bFunctionalLength
        0x24, //bDescriptorType
        0x02, //bDescriptorSubtype
        0x02, //bmCapabilities

        //CDC Union Functional Descriptor:
        //------------------------------
        0x05, //bFunctionalLength
        0x24, //bDescriptorType
        0x06, //bDescriptorSubtype
        0x00, //bControlInterface
        0x01, //bSubordinateInterface(0)

        //Endpoint Descriptor:
        //------------------------------
        0x07, //bLength
        USB_DESC_TYPE_ENDPOINT, //bDescriptorType
        CDC_CMD_EP, //bEndpointAddress (IN Endpoint)
        0x03, //bmAttributes (Transfer: Interrupt / Synch: None / Usage: Data)
        LOBYTE(CDC_CMD_PACKET_SIZE), //wMaxPacketSize[0]
        HIBYTE(CDC_CMD_PACKET_SIZE), //wMaxPacketSize[1]
        0x10, //bInterval

        //Interface Descriptor:
        //------------------------------
        0x09, //bLength
        USB_DESC_TYPE_INTERFACE, //bDescriptorType
        0x01, //bInterfaceNumber
        0x00, //bAlternateSetting
        0x02, //bNumEndPoints
        0x0A, //bInterfaceClass (CDC Data)
        0x00, //bInterfaceSubClass
        0x00, //bInterfaceProtocol
        0x06, //iInterface ("AprsTracker TNC")

        //Endpoint Descriptor:
        //------------------------------
        0x07, //bLength
        USB_DESC_TYPE_ENDPOINT, //bDescriptorType
        CDC_OUT_EP, //bEndpointAddress (OUT Endpoint)
        0x02, //bmAttributes (Transfer: Bulk / Synch: None / Usage: Data)
        LOBYTE(CDC_DATA_PACKET_SIZE), //wMaxPacketSize[0]
        HIBYTE(CDC_DATA_PACKET_SIZE), //wMaxPacketSize[1]
        0x00, //bInterval

        //Endpoint Descriptor:
        //------------------------------
        0x07, //bLength
        USB_DESC_TYPE_ENDPOINT, //bDescriptorType
        CDC_IN_EP, //bEndpointAddress (IN Endpoint)
        0x02, //bmAttributes (Transfer: Bulk / Synch: None / Usage: Data)
        LOBYTE(CDC_DATA_PACKET_SIZE), //wMaxPacketSize[0]
        HIBYTE(CDC_DATA_PACKET_SIZE), //wMaxPacketSize[1]
        0x00, //bInterval
};

static uint8_t USBD_Class_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	cdcRxFifo = (cdcFifo){ .buffer = cdcRxFifoBuffer, .head = cdcRxFifoBuffer, .tail = cdcRxFifoBuffer };
	cdcTxFifo = (cdcFifo){ .buffer = cdcTxFifoBuffer, .head = cdcTxFifoBuffer, .tail = cdcTxFifoBuffer };
	txState = CDC_TX_STATE_FREE;
	portState = 0;

    USBD_LL_OpenEP(pdev, CDC_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
    USBD_LL_OpenEP(pdev, CDC_OUT_EP, USBD_EP_TYPE_BULK, CDC_DATA_PACKET_SIZE);
    USBD_LL_OpenEP(pdev, CDC_IN_EP, USBD_EP_TYPE_BULK, CDC_DATA_PACKET_SIZE);

    USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, cdcRxBuffer, CDC_DATA_PACKET_SIZE);

    return USBD_OK;
}

static uint8_t USBD_Class_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    USBD_LL_CloseEP(pdev, CDC_CMD_EP);
    USBD_LL_CloseEP(pdev, CDC_OUT_EP);
    USBD_LL_CloseEP(pdev, CDC_IN_EP);

    return USBD_OK;
}

static uint8_t USBD_Class_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	static uint32_t requests = 0;
	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS:
        switch (req->bRequest) {
        case CDC_REQ_SET_LINE_CODING:            
            USBD_CtlPrepareRx(pdev, (uint8_t *)&lineCoding, req->wLength);
            requests++;
            break;
        case CDC_REQ_GET_LINE_CODING:
            USBD_CtlSendData(pdev, (uint8_t *)&lineCoding, req->wLength);
            requests++;
			if(requests >= 4)
				portState = CDC_PORT_STATE_OPEN;
            break;
        case CDC_REQ_SET_CONTROL_LINE_STATE:
        	portState = CDC_PORT_STATE_CLOSED;
            break;
        default:
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
        }
        break;

    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest) {
        default:
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
        }
    }

    return USBD_OK;
}

static uint8_t USBD_Class_EP0_RxReady(USBD_HandleTypeDef *pdev) {
    return USBD_OK;
}

uint8_t USBD_Class_DataIn(struct _USBD_HandleTypeDef *pdev , uint8_t epnum) {
	if(cdcTxFifo.tail != cdcTxFifo.head) {
		int i = 0;
		while((cdcTxFifo.tail != cdcTxFifo.head) && (i < CDC_DATA_PACKET_SIZE)) {
			cdcTxBuffer[i++] = *cdcTxFifo.tail++;
			if(cdcTxFifo.tail >= cdcTxFifo.buffer + CDC_TX_BUFFER_SIZE) {
				cdcTxFifo.tail = cdcTxFifo.buffer;
			}
		}
		USBD_LL_Transmit(pdev, CDC_IN_EP, cdcTxBuffer, i);
	} else {
		txState = CDC_TX_STATE_FREE;
	}

	return USBD_OK;
}

static uint8_t USBD_Class_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	size_t length = USBD_LL_GetRxDataSize(pdev, epnum);
	for(size_t i = 0; i < length; i++) {
		*cdcRxFifo.head++ = cdcRxBuffer[i];
		if(cdcRxFifo.head == (cdcRxFifo.buffer + CDC_RX_BUFFER_SIZE)) {
			cdcRxFifo.head = cdcRxFifo.buffer;
		}
	}
	USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, cdcRxBuffer, CDC_DATA_PACKET_SIZE);

    return USBD_OK;
}

static uint8_t *USBD_Class_GetConfigDescriptor(uint16_t *length) {
    *length = sizeof(USBD_Class_ConfigDescriptor);
    return USBD_Class_ConfigDescriptor;
}

uint8_t *USBD_Class_GetUsrStrDescriptor(struct _USBD_HandleTypeDef *pdev, uint8_t index, uint16_t *length) {
    static uint8_t stringBuffer[USBD_MAX_STR_DESC_SIZ];
    const char *string;
    switch (index) {
    case 6:
        string = "AprsTracker";
        break;
    default:
        string = "AprsTracker";
        break;
    }

    USBD_GetString((uint8_t *) string, stringBuffer, length);
    return stringBuffer;
}

uint8_t USBD_CDC_ReceiveIsEmpty(void) {
	return cdcRxFifo.head == cdcRxFifo.tail;
}

void USBD_CDC_ReceiveData(USBD_HandleTypeDef *pdev, uint8_t *buffer, size_t size) {

}

void USBD_CDC_TransmitData(USBD_HandleTypeDef *pdev, uint8_t *buffer, size_t size) {
	if((pdev->dev_state == USBD_STATE_CONFIGURED) && (portState == CDC_PORT_STATE_OPEN)) {
		if(cdcTxFifo.head == cdcTxFifo.tail && txState == CDC_TX_STATE_FREE) {
			for(size_t i = 0; i < ((size > CDC_DATA_PACKET_SIZE) ? CDC_DATA_PACKET_SIZE : size); i++) {
				cdcTxBuffer[i] = *buffer++;
			}
			txState = CDC_TX_STATE_BUSY;
			USBD_LL_Transmit(pdev, CDC_IN_EP, cdcTxBuffer, (size > CDC_DATA_PACKET_SIZE) ? CDC_DATA_PACKET_SIZE : size);
			if(size > CDC_DATA_PACKET_SIZE) {
				for(size_t i = 0; i < (size - CDC_DATA_PACKET_SIZE); i++) {
					*cdcTxFifo.head++ = *buffer++;
					if(cdcTxFifo.head >= cdcTxFifo.buffer + CDC_TX_BUFFER_SIZE) {
						cdcTxFifo.head = cdcTxFifo.buffer;
					}
				}
			}
		} else {
			for(size_t i = 0; i < size; i++) {
				*cdcTxFifo.head++ = *buffer++;
				if(cdcTxFifo.head >= cdcTxFifo.buffer + CDC_TX_BUFFER_SIZE) {
					cdcTxFifo.head = cdcTxFifo.buffer;
				}
			}
		}
	}
}
