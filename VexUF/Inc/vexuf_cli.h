#ifndef VEXUF_CLI_H_
#define VEXUF_CLI_H_

#include "vexuf.h"
#include "vexuf_serial.h"

typedef struct {
  const char *command_name;
  void (*handler)(const char *args);
} Command;

UF_STATUS CLI_handleCommand(SerialInterface interface);

#endif  // VEXUF_CLI_H_