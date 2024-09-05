/*
 * config.h
 *
 *  Created on: Jul 31, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_VEXUF_CONFIG_H_
#define INC_VEXUF_CONFIG_H_

#include "vexuf.h"
#include "vexuf_actuators.h"
#include "vexuf_avs.h"
#include "vexuf_i2c.h"
#include "vexuf_indicators.h"
#include "vexuf_lcd.h"
#include "vexuf_outputs.h"
#include "vexuf_pwm.h"
#include "vexuf_rtc.h"
#include "vexuf_serial.h"
#include "vexuf_spi.h"
#include "vexuf_tnc.h"
#include "vexuf_trigs.h"

#define CONFIG_FLAG 0x3E3E
#define CONFIG_VERSION 3
#define EEPROM_CONFIG_FLAG_ADDRESS 0x0000

UF_STATUS CONFIG_IsConfigured(void);
UF_STATUS CONFIG_loadConfigValues(uint16_t* version, uint16_t* configCount);

UF_STATUS CONFIG_loadConfiguration(void);
UF_STATUS CONFIG_saveConfiguration(void);

UF_STATUS CONFIG_ReadSerialNumber(char* serialNumberBuffer);
UF_STATUS CONFIG_WriteSerialNumber(void);

#endif /* INC_VEXUF_CONFIG_H_ */