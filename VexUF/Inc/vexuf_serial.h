#ifndef VEXUF_SERIAL_H
#define VEXUF_SERIAL_H

#include "vexuf.h"

#define SERIAL_BUFFER_SIZE 2048

typedef enum {
  Baud300 = 0,
  Baud600,
  Baud1200,
  Baud4800,
  Baud9600,
  Baud19200,
  Baud57600,
  Baud115200
} SerialBaudRate;

typedef enum { TTL, TNC, CDC } SerialInterface;
typedef enum { debugOnly, cliOnly, debugAndCli } TtlLogConfiguration;

typedef struct {
  uint16_t ttl_enabled : 1;
  uint16_t ttl_led_enabled : 1;
  SerialBaudRate ttl_baud : 4;
  TtlLogConfiguration ttlConf : 3;
  uint16_t tnc_enabled : 1;
  SerialBaudRate tnc_baud : 4;
  uint16_t reserved : 2;
} SerialConfiguration;

UF_STATUS SERIAL_init(UART_HandleTypeDef *ttl, UART_HandleTypeDef *tnc);
UF_STATUS SERIAL_baudToInt(SerialBaudRate baud, uint32_t *baudInt);
UF_STATUS SERIAL_intToBaud(uint32_t baudInt, SerialBaudRate *baud);
UF_STATUS SERIAL_setBaudRate(UART_HandleTypeDef *huart, SerialBaudRate baud);

#endif  // VEXUF_SERIAL_H