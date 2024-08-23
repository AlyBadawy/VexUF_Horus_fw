/*
 * vexuf_helpers.h
 *
 *  Created on: Aug 10, 2024
 *      Author: Aly Badawy
 */

#ifndef VEXUF_VEXUF_HELPERS_H_
#define VEXUF_VEXUF_HELPERS_H_

#include "main.h"
#include "string.h"

#define SERIAL_NUMBER_LENGTH		24
#define CALLSIGN_LENGTH				20

typedef struct {
	uint16_t timer_1hz_ticked: 1;
	uint16_t timer_10hz_ticked: 1;
	uint16_t timer_0d1hz_ticked: 1;
	uint16_t no_config_error: 1;
	uint16_t sd_card_error: 1;
	uint16_t sd_card_present: 1;
	uint16_t ttlBuffered: 1;
	uint16_t tncBuffered: 1;
	uint16_t cdcBuffered: 1;
	uint16_t reserved: 7;
} VexufStatus;

float cToF(float c);
float fToC(float f);
// void VexUF_CurrentDateTimeString(char *string);
void VexUF_GenerateSerialNumber();

#endif /* VEXUF_VEXUF_HELPERS_H_ */