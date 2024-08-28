#ifndef VEXUF_H
#define VEXUF_H

#include <stdio.h>

#include "main.h"
#include "string.h"

#define SERIAL_NUMBER_LENGTH 24

typedef enum {
  UF_OK,
  UF_DISABLED,
  UF_TRIGGERED,
  UF_NOT_TRIGGERED,
  UF_NOT_CONFIGURED,
  UF_OVERWRITTEN,
  UF_CARD_EJECTED,
  UF_CARD_FULL,
  UF_BUSY,
  UF_INVALID_FORMAT,
  UF_ERROR,
  UF_HARD_ERROR
} UF_STATUS;

typedef struct {
  uint16_t isConfigured : 1;
  uint16_t timer_1hz_ticked : 1;
  uint16_t timer_10hz_ticked : 1;
  uint16_t timer_0d1hz_ticked : 1;
  uint16_t noConfigError : 1;
  uint16_t sdCardError : 1;
  uint16_t sdCardEjected : 1;
  uint16_t sdCardFull : 1;
  uint16_t sdCardMounted : 1;
  uint16_t ttlBuffered : 1;
  uint16_t tncBuffered : 1;
  uint16_t cdcBuffered : 1;
  uint16_t reserved : 4;
} VexufStatus;

#define SERIAL_NUMBER_LENGTH 24
#define CALLSIGN_LENGTH 20

// void VexUF_CurrentDateTimeString(char *string);
void VexUF_GenerateSerialNumber();
uint16_t getSerialBytes(void);

void trim(char **str);

#endif  // VEXUF_H