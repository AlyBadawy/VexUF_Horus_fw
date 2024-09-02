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
  }

  if (serialConf.tnc_enabled) {
    if (SERIAL_setBaudRate(tncUart, serialConf.ttl_baud) != UF_OK)
      return UF_ERROR;
    if (HAL_UARTEx_ReceiveToIdle_IT(tncUart, tncRxData, SERIAL_BUFFER_SIZE) !=
        HAL_OK) {
      return UF_ERROR;
    }
  }
  return UF_OK;
}

UF_STATUS SERIAL_setBaudRate(UART_HandleTypeDef *huart, SerialBaudRate baud) {
  uint32_t newBaudRate = 0;
  switch (baud) {
    case Baud300:
      newBaudRate = 300;
      break;
    case Baud600:
      newBaudRate = 600;
      break;
    case Baud1200:
      newBaudRate = 1200;
      break;
    case Baud4800:
      newBaudRate = 4800;
      break;
    case Baud9600:
      newBaudRate = 9600;
      break;
    case Baud19200:
      newBaudRate = 19200;
      break;
    case Baud57600:
      newBaudRate = 57600;
      break;
    case Baud115200:
    default:  // TODO: return uf_error if baud rate is not supported
      newBaudRate = 115200;
      break;
  }

  huart->Init.BaudRate = newBaudRate;
  if (HAL_UART_Init(huart) != HAL_OK) return UF_ERROR;

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

  if (rxData[size - 1] == '\r' || rxData[size - 1] == '\n' || size > 2) {
    huart == ttlUart ? (vexufStatus.ttlBuffered = 1)
                     : (vexufStatus.tncBuffered = 1);
  }
  *rxIdx = size;
  rxData[size] = '\0';

  if (huart == ttlUart) {
    HAL_UARTEx_ReceiveToIdle_IT(ttlUart, ttlRxData, SERIAL_BUFFER_SIZE);
  } else {
    HAL_UARTEx_ReceiveToIdle_IT(tncUart, tncRxData, SERIAL_BUFFER_SIZE);
  }
}
