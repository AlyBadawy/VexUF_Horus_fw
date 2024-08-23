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
#include "vexuf_actuators.h"

#define SERIAL_NUMBER_LENGTH		24
#define CALLSIGN_LENGTH				20

typedef enum {NoLCD = 0, LCD2004 = 1, LCD1602 = 2} LcdType;
typedef enum {NoI2C = 0, AHT20 = 1, AHT21 = 2} I2CType;
typedef enum {NoSpi = 0, BLUETOOTH} SpiType;

typedef struct {
    uint16_t lcdAdd;
    LcdType lcdType;  // Use the enum type for lcdType
    uint16_t lcdPwm;
} LcdConfiguration;

typedef struct {
    uint16_t i2cAdd;
    I2CType i2cType;  // Use the enum type for lcdType
} I2CConfiguration;

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
void base32_encode(const uint8_t *data, size_t length, char *output);
// void VexUF_CurrentDateTimeString(char *string);
void VexUF_GenerateSerialNumber();

#endif /* VEXUF_VEXUF_HELPERS_H_ */