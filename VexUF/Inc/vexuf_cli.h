#ifndef VEXUF_CLI_H_
#define VEXUF_CLI_H_

#include "vexuf.h"
#include "vexuf_serial.h"

typedef void (*CommandHandler)(const char *args);

typedef struct {
  const char *command_name;
  CommandHandler handler;
} Command;

UF_STATUS CLI_handleCommand(SerialInterface interface);

#endif  // VEXUF_CLI_H_