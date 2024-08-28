#include "vexuf_serial.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

extern VexufStatus vexufStatus;

uint8_t ttlRxData[SERIAL_BUFFER_SIZE];
uint8_t tncRxData[SERIAL_BUFFER_SIZE];
uint16_t ttlRxIdx;
uint16_t tncRxIdx;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
  if (huart == &huart1) {
    if (ttlRxData[size - 1] == '\r' || ttlRxData[size - 1] == '\n' ||
        size > 2) {
      vexufStatus.ttlBuffered = 1;
    }
    ttlRxIdx = size;
    ttlRxData[size] = '\0';
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, ttlRxData, SERIAL_BUFFER_SIZE);
  } else if (huart == &huart6) {
    if (tncRxData[size - 1] == '\r' || tncRxData[size - 1] == '\n' ||
        size > 2) {
      vexufStatus.tncBuffered = 1;
    }
    tncRxIdx = size;
    tncRxData[size] = '\0';
    HAL_UARTEx_ReceiveToIdle_IT(&huart6, tncRxData, SERIAL_BUFFER_SIZE);
  }
}