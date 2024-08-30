#ifndef VEXUF_CLI_H_
#define VEXUF_CLI_H_

#include "vexuf.h"
#include "vexuf_serial.h"

void CLI_init(UART_HandleTypeDef *ttl, UART_HandleTypeDef *tnc);

UF_STATUS CLI_handleCommand(SerialInterface interface);

#endif  // VEXUF_CLI_H_