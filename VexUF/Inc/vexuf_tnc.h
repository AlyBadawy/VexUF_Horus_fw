#ifndef VEXUF_TNC_H_
#define VEXUF_TNC_H_

#include "vexuf.h"

#define CALLSIGN_LENGTH 20

#define TNC_MESSAGE_LENGTH 64
#define TNC_MESSAGE_COUNT 10

#define TNC_PATH_LENGTH 32
#define TNC_PATH_COUNT 5

void TNC_init(UART_HandleTypeDef *tnc);
void TNC_handleCli(const char *args, char *responseBuffer);

#endif  // VEXUF_TNC_H_