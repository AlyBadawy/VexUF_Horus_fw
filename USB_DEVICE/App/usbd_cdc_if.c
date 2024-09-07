/**
 ******************************************************************************
 * @file          : usbd_cdc_if.c
 * @brief        :  Usb device for Virtual Com Port.
 ******************************************************************************
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @copyright     : Aly Badawy
 * @author website: https://alybadawy.com
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

#include "vexuf.h"
#include "vexuf_serial.h"

/* TypeDef -------------------------------------------------------------------*/
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern VexufStatus vexufStatus;
unsigned char cdcRxData[2048];
uint32_t cdcRxIdx;

/* Variables -----------------------------------------------------------------*/
uint8_t UserRxBufferFS[2048];
uint8_t UserTxBufferFS[2048];

/* Prototypes ----------------------------------------------------------------*/
static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* Code ----------------------------------------------------------------------*/
USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {CDC_Init_FS, CDC_DeInit_FS,
                                              CDC_Control_FS, CDC_Receive_FS,
                                              CDC_TransmitCplt_FS};

uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len) {
  uint8_t result = USBD_OK;
  USBD_CDC_HandleTypeDef *hcdc =
      (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;
  uint16_t timeout = 0xFFFF;
  while (hcdc->TxState != 0) {
    if (timeout-- == 0) {
      return USBD_FAIL;
    }
  }

  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  return result;
}

/* Private Methods -----------------------------------------------------------*/
static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len) {
  if (*Len == 1 || Buf[0] == '\r' || Buf[0] == '\n') {
    CDC_Transmit_FS(Buf, *Len);
  }
  if (Buf[0] == '\b') {
    if (cdcRxIdx > 0) {
      cdcRxIdx--;
      CDC_Transmit_FS((uint8_t *)"\b \b",
                      3);  // Send backspace and space to clear the character on
                           // the terminal
    }
  } else {
    memcpy(&cdcRxData[cdcRxIdx], Buf, *Len);
    cdcRxIdx += *Len;
  }

  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  // Check if the last character is a carriage return or line feed
  if (cdcRxData[cdcRxIdx - 1] == '\r' || cdcRxData[cdcRxIdx - 1] == '\n') {
    vexufStatus.cdcBuffered = 1;
  }

  // Check if the last character is a carriage return or line feed
  if (cdcRxData[cdcRxIdx - 1] == '\r' || cdcRxData[cdcRxIdx - 1] == '\n') {
    vexufStatus.cdcBuffered = 1;
  }

  return (USBD_OK);
}

static int8_t CDC_Init_FS(void) {
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
}

static int8_t CDC_DeInit_FS(void) { return (USBD_OK); }

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
  UNUSED(pbuf);
  UNUSED(length);

  /*******************************************************************************/
  /* Line Coding Structure */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per
   * second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits */
  /*                                        0 - 1 Stop bit */
  /*                                        1 - 1.5 Stop bits */
  /*                                        2 - 2 Stop bits */
  /* 5      | bParityType |  1   | Number | Parity */
  /*                                        0 - None */
  /*                                        1 - Odd */
  /*                                        2 - Even */
  /*                                        3 - Mark */
  /*                                        4 - Space */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16). */
  /*******************************************************************************/
  switch (cmd) {
    case CDC_SEND_ENCAPSULATED_COMMAND:
    case CDC_GET_ENCAPSULATED_RESPONSE:
    case CDC_SET_COMM_FEATURE:
    case CDC_GET_COMM_FEATURE:
    case CDC_CLEAR_COMM_FEATURE:
    case CDC_SET_LINE_CODING:
    case CDC_GET_LINE_CODING:
    case CDC_SET_CONTROL_LINE_STATE:
    case CDC_SEND_BREAK:
    default:
      break;
  }

  return (USBD_OK);
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum) {
  memset(Buf, 0, *Len);
  uint8_t result = USBD_OK;
  UNUSED(epnum);
  return result;
}
