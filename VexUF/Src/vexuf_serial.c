/**
 ******************************************************************************
 * @file          : vexuf_serial.c
 * @brief        : VexUF Serial Implementation (TTL, TNC, and CDC)
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
#include "vexuf_serial.h"

#include "vexuf_config.h"
#include "vexuf_tnc.h"

/* TypeDef -------------------------------------------------------------------*/
static UART_HandleTypeDef *ttlUart;
static UART_HandleTypeDef *tncUart;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern VexufStatus vexufStatus;

/* Variables -----------------------------------------------------------------*/
SerialConfiguration serialConf = {0};
unsigned char ttlRxData[SERIAL_BUFFER_SIZE];
unsigned char tncRxData[SERIAL_BUFFER_SIZE];
uint16_t ttlRxIdx;
uint16_t tncRxIdx;

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
UF_STATUS SERIAL_init(UART_HandleTypeDef *ttl, UART_HandleTypeDef *tnc) {
  ttlUart = ttl;
  tncUart = tnc;

  if (serialConf.ttl_enabled) {
    if (SERIAL_setBaudRate(ttlUart, (uint16_t)serialConf.ttl_baud) != UF_OK)
      return UF_ERROR;
    if (HAL_UARTEx_ReceiveToIdle_IT(ttlUart, ttlRxData, SERIAL_BUFFER_SIZE) !=
        HAL_OK) {
      return UF_ERROR;
    }
  } else {
    if (HAL_UART_DeInit(ttlUart) != HAL_OK) return UF_ERROR;
  }

  if (serialConf.tnc_enabled) {
    if (SERIAL_setBaudRate(tncUart, serialConf.ttl_baud) != UF_OK)
      return UF_ERROR;
    if (HAL_UARTEx_ReceiveToIdle_IT(tncUart, tncRxData, SERIAL_BUFFER_SIZE) !=
        HAL_OK) {
      return UF_ERROR;
    }
    if (TNC_init(tncUart) != UF_OK) return UF_ERROR;
  } else {
    if (HAL_UART_DeInit(tncUart) != HAL_OK) return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS SERIAL_setBaudRate(UART_HandleTypeDef *huart, SerialBaudRate baud) {
  // TODO: is this working?
  uint32_t newBaudRate = 0;
  if (SERIAL_baudToInt(baud, &newBaudRate) != UF_OK) return UF_ERROR;

  huart->Init.BaudRate = newBaudRate;
  if (HAL_UART_Init(huart) != HAL_OK) return UF_ERROR;

  return UF_OK;
}

UF_STATUS SERIAL_intToBaud(uint32_t baudInt, SerialBaudRate *baud) {
  switch (baudInt) {
    case 300:
      *baud = Baud300;
      break;
    case 600:
      *baud = Baud600;
      break;
    case 1200:
      *baud = Baud1200;
      break;
    case 4800:
      *baud = Baud4800;
      break;
    case 9600:
      *baud = Baud9600;
      break;
    case 19200:
      *baud = Baud19200;
      break;
    case 57600:
      *baud = Baud57600;
      break;
    case 115200:
      *baud = Baud115200;
      break;
    default:
      return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS SERIAL_baudToInt(SerialBaudRate baud, uint32_t *baudInt) {
  switch (baud) {
    case Baud300:
      *baudInt = 300;
      break;
    case Baud600:
      *baudInt = 600;
      break;
    case Baud1200:
      *baudInt = 1200;
      break;
    case Baud4800:
      *baudInt = 4800;
      break;
    case Baud9600:
      *baudInt = 9600;
      break;
    case Baud19200:
      *baudInt = 19200;
      break;
    case Baud57600:
      *baudInt = 57600;
      break;
    case Baud115200:
    default:
      *baudInt = 115200;
      break;
  }
  return UF_OK;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
  uint8_t *rxData;
  uint16_t *rxIdx;

  if (huart == ttlUart) {
    rxData = ttlRxData;
    rxIdx = &ttlRxIdx;
  } else {
    rxData = tncRxData;
    rxIdx = &tncRxIdx;
  }

  huart == ttlUart ? (vexufStatus.ttlBuffered = 1)
                   : (vexufStatus.tncBuffered = 1);
  *rxIdx = size;
  rxData[size] = '\0';

  if (huart == ttlUart) {
    HAL_UARTEx_ReceiveToIdle_IT(ttlUart, ttlRxData, SERIAL_BUFFER_SIZE);
  } else {
    HAL_UARTEx_ReceiveToIdle_IT(tncUart, tncRxData, SERIAL_BUFFER_SIZE);
  }
}
