#ifndef VEXUF_SERIAL_H
#define VEXUF_SERIAL_H

#include "vexuf.h"

#define SERIAL_BUFFER_SIZE 1024

typedef enum {
  Baud300,
  Baud600,
  Baud1200,
  Baud4800,
  Baud9600,
  Baud19200,
  Baud57600,
  Baud115200
} BaudRate;

typedef enum { TTL, TNC, CDC } SerialInterface;
typedef enum { debugOnly, cliOnly, debugAndCli } TtlLogConfiguration;

typedef struct {
  uint16_t ttl_enabled : 1;
  uint16_t ttl_led_enabled : 1;
  BaudRate ttl_baud : 4;
  TtlLogConfiguration ttlConf : 3;
  uint16_t tnc_enabled : 1;
  BaudRate tnc__baud : 4;
  uint16_t reserved : 2;
} SerialConfiguration;

void SERIAL_init(UART_HandleTypeDef *ttl, UART_HandleTypeDef *tnc);

#endif  // VEXUF_SERIAL_H