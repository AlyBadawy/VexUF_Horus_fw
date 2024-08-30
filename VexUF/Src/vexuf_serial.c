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
void SERIAL_init(UART_HandleTypeDef *ttl, UART_HandleTypeDef *tnc) {
  ttlUart = ttl;
  tncUart = tnc;
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
  HAL_UARTEx_ReceiveToIdle_IT(huart, rxData, SERIAL_BUFFER_SIZE);
}

/* Private Methods -----------------------------------------------------------*/