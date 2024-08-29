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
#include "vexuf_pwm.h"
#include "vexuf_rtc.h"
#include "vexuf_serial.h"
#include "vexuf_spi.h"
#include "vexuf_tnc.h"
#include "vexuf_trigs.h"

#define CONFIG_FLAG 0x3E3E
#define CONFIG_VERSION 1

// Memory Map addresses
#define EEPROM_CONFIG_FLAG_ADDRESS 0x0000
#define EEPROM_CONFIG_VERSION_ADDRESS 0x0001
#define EEPROM_CONFIG_COUNT_ADDRESS 0x0002
#define EEPROM_SERIAL_NUMBER_ADDRESS 0x0003  // 12 WORDS - 24 BYTES
#define EEPROM_VEXUF_SERIAL_ADDRESS 0x000F

#define EEPROM_REGISTRATION_NUMBER_ADDRESS 0x0010

#define EEPROM_CALLSIGN_ADDRESS 0x0015  // 10 WORDS - 20 BYTES
// 001F reserved

// 0x0020 to 0x002F reserved

#define EEPROM_PWM1_ENABLED_ADDRESS 0x0030
#define EEPROM_PWM1_DEFAULT_ADDRESS 0x0031
#define EEPROM_PWM2_ENABLED_ADDRESS 0x0032
#define EEPROM_PWM2_DEFAULT_ADDRESS 0x0033

#define EEPROM_ACTUATORS_CONF_ADDRESS 0x0034
#define EEPROM_ACTUATORS_VALUES_ADDRESS 0x0035

#define EEPROM_SERIAL_INTERFACE_ADDRESS 0x0036
#define EEPROM_I2C_TYPE_ADDRESS 0x0037
#define EEPROM_I2C_ADDRESS_ADDRESS 0x0038
#define EEPROM_LCD_TYPE_ADDRESS 0x0039
#define EEPROM_LCD_ADDRESS_ADDRESS 0x003A
#define EEPROM_LCD_PWM_ADDRESS 0x003B
#define EEPROM_SPI_TYPE_ADDRESS 0x003C
// 0x0040 to 0x003D reserved
#define EEPROM_OUTPUT_ADDRESS 0x003E
#define EEPROM_INDICATORS_ADDRESS 0x003F

#define EEPROM_AV_ENABLED_ADDRESS 0x0040
// 0X0041 TO 0X0044 reserved
#define EEPROM_AV_SLOW_RULE_ENABLED_ADDRESS 0x0045
#define EEPROM_AV_SLOW_RULE_MIN_ADDRESS 0x0046
#define EEPROM_AV_SLOW_RULE_MAX_ADDRESS 0x0047
// 0x0048 reserved
#define EEPROM_AV_FAST_RULE_ENABLED_ADDRESS 0x0049
#define EEPROM_AV_FAST_RULE_MIN_ADDRESS 0x004A
#define EEPROM_AV_FAST_RULE_MAX_ADDRESS 0x004B
// 0x004C reserved
#define EEPROM_AV_ON_RULE_ENABLED_ADDRESS 0x004D
#define EEPROM_AV_ON_RULE_MIN_ADDRESS 0x004E
#define EEPROM_AV_ON_RULE_MAX_ADDRESS 0x004F
#define EEPROM_AV_SHIFT 16  // there are 3 AVs

#define EEPROM_ALARM_ENABLED_ADDRESS 0x0071
#define EEPROM_ALARM_ACTUATORS_ADDRESS 0x0071
#define EEPROM_ALARM_OUTPUT_ADDRESS 0x0072
#define EEPROM_ALARM_PWM1_ADDRESS 0x0073
#define EEPROM_ALARM_PWM2_ADDRESS 0x0074
#define EEPROM_ALARM_TNC_ENABLED_ADDRESS 0x0075
#define EEPROM_ALARM_TNC_PATH_ADDRESS 0x0076
#define EEPROM_ALARM_TNC_MSG_ADDRESS 0x0077
#define EEPROM_ALARM_SHIFT 8  // There are 2 alarms

// 0x0080 to 0x00DF reserved

#define EEPROM_TRIG_STATUS_ADDRESS 0x00E0
#define EEPROM_TRIG_FROM_VALUE_ADDRESS 0x00E1
#define EEPROM_TRIG_TO_VALUE_ADDRESS 0x00E2
#define EEPROM_TRIG_TEST_ADDRESS 0x00E3
#define EEPROM_TRIG_ACTUATORS_ADDRESS 0x00E4
#define EEPROM_TRIG_OUTPUT_ADDRESS 0x00E5
#define EEPROM_TRIG_PWM_ADDRESS 0x00E6  // 2 words (FOR PWM1 AND PWM 2)
#define EEPROM_TRIG_TNC1_ADDRESS \
  0x00E8  // 4 words (ENABLED, PATH, MESSAGE, RESERVED)
#define EEPROM_TRIG_TNC2_ADDRESS \
  0x00EC                      // 4 words (ENABLED, PATH, MESSAGE, RESERVED)
#define EEPROM_TRIG_SHIFT 16  // There are 25 triggers

#define EEPROM_TNC_MESSAGE_ADDRESS 0x0270
#define EEPROM_TNC_MESSAGE_LENGTH 32
#define EEPROM_TNC_MESSAGE_COUNT 10

#define EEPROM_TNC_PATH_ADDRESS 0x03B0
#define EEPROM_TNC_PATH_LENGTH 16
#define EEPROM_TNC_PATH_COUNT 5

typedef enum { CONFIG_OK, CONFIG_ERROR, CONFIG_NOT_CONFIGURED } CONFIG_STATUS;

UF_STATUS CONFIG_IsConfigured(void);
UF_STATUS CONFIG_GetConfigValues(uint16_t* version, uint16_t* configCount);
UF_STATUS CONFIG_SetIsConfigured(void);

UF_STATUS CONFIG_ReadSerialNumber(char* serialNumberBuffer[24]);
UF_STATUS CONFIG_WriteSerialNumber(void);

UF_STATUS CONFIG_getRegNumber(uint32_t* regNumber);
UF_STATUS CONFIG_SetRegNumber(const uint32_t* regNumber);

UF_STATUS CONFIG_getCallSign(char* callsign);
UF_STATUS CONFIG_setCallSign(const char* newCallSign);

UF_STATUS CONFIG_getPwmConfigurations(PwmConfiguration* pwmConfigBuffer);
UF_STATUS CONFIG_setPwmConfigurations(const PwmConfiguration* pwmConfig);

UF_STATUS CONFIG_getActuators(ActuatorsConfiguration* actConf,
                              ActuatorsValues* ActValues);
UF_STATUS CONFIG_setActuators(const ActuatorsConfiguration* actConf,
                              const ActuatorsValues* actValues);

UF_STATUS CONFIG_getSerialConf(SerialConfiguration* serialConf);
UF_STATUS CONFIG_setSerialConf(const SerialConfiguration* serialConf);

UF_STATUS CONFIG_getI2cConf(I2CConfiguration* i2cConf);
UF_STATUS CONFIG_setI2cConf(const I2CConfiguration* i2cConf);
UF_STATUS CONFIG_getLcdConf(LcdConfiguration* lcdConf);
UF_STATUS CONFIG_setLcdConf(const LcdConfiguration* lcdConf);

UF_STATUS CONFIG_getSPIType(SpiType* spiType);
UF_STATUS CONFIG_setSPIType(const SpiType* spiType);

UF_STATUS CONFIG_getIndicatorsConf(IndConfiguration* indConf);
UF_STATUS CONFIG_setIndicatorsConf(const IndConfiguration* indConf);

UF_STATUS CONFIG_getAvSensor(AvSensor* av, uint8_t idx);
UF_STATUS CONFIG_SetAvSensor(const AvSensor* av, uint8_t idx);

UF_STATUS CONFIG_getAlarmConf(AlarmConfiguration* alarm, uint8_t idx);
UF_STATUS CONFIG_setAlarmConf(const AlarmConfiguration* alarm, uint8_t idx);

UF_STATUS CONFIG_getTrigConf(TriggerConfiguration* trigConf, uint8_t idx);
UF_STATUS CONFIG_setTrigConf(const TriggerConfiguration* trigConf, uint8_t idx);

UF_STATUS CONFIG_getTncMessage(char* tncMessage, uint8_t idx);
UF_STATUS CONFIG_setTncMessage(const char* tncMessage, uint8_t idx);

UF_STATUS CONFIG_getTncPath(char* tncPath, uint8_t idx);
UF_STATUS CONFIG_setTncPath(const char* tncPath, uint8_t idx);

#endif /* INC_VEXUF_CONFIG_H_ */