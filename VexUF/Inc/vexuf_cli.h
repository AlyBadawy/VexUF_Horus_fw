#ifndef VEXUF_CLI_H_
#define VEXUF_CLI_H_

#include "vexuf.h"
#include "vexuf_serial.h"

UF_STATUS CLI_init(void);

UF_STATUS CLI_handleCommand(SerialInterface interface);

#endif  // VEXUF_CLI_H_