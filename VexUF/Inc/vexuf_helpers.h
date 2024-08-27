/*
 * vexuf_helpers.h
 *
 *  Created on: Aug 10, 2024
 *      Author: Aly Badawy
 */

#ifndef VEXUF_VEXUF_HELPERS_H_
#define VEXUF_VEXUF_HELPERS_H_

#include <stdio.h>

#include "main.h"
#include "string.h"

#define SERIAL_NUMBER_LENGTH 24
#define CALLSIGN_LENGTH 20

typedef enum { UF_OK, UF_DISABLED, UF_ERROR } UF_Status;

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

float cToF(float c);
float fToC(float f);
// void VexUF_CurrentDateTimeString(char *string);
void VexUF_GenerateSerialNumber();
uint16_t getSerialBytes(void);

#endif /* VEXUF_VEXUF_HELPERS_H_ */