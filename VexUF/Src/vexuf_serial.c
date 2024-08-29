#include "vexuf_serial.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

extern VexufStatus vexufStatus;

SerialConfiguration serialConf = {0};

unsigned char ttlRxData[SERIAL_BUFFER_SIZE];
unsigned char tncRxData[SERIAL_BUFFER_SIZE];
uint16_t ttlRxIdx;
uint16_t tncRxIdx;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
  uint8_t *rxData;
  uint16_t *rxIdx;

  if (huart == &huart1) {
    rxData = ttlRxData;
    rxIdx = &ttlRxIdx;
  } else {
    rxData = tncRxData;
    rxIdx = &tncRxIdx;
  }

  if (rxData[size - 1] == '\r' || rxData[size - 1] == '\n' || size > 2) {
    huart == &huart1 ? (vexufStatus.ttlBuffered = 1)
                     : (vexufStatus.tncBuffered = 1);
  }
  *rxIdx = size;
  rxData[size] = '\0';
  HAL_UARTEx_ReceiveToIdle_IT(huart, rxData, SERIAL_BUFFER_SIZE);
}