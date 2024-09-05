/**
 ******************************************************************************
 * @file          : vexuf_config.c
 * @brief        : Configuration functions for VexUF
 ******************************************************************************
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @copyright     : Aly Badawy
 * @author website: https://alybadawy.com
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "vexuf_config.h"

#include <ctype.h>
#include <string.h>

#include "93c86.h"
#include "vexuf_tnc.h"

/* TypeDef -------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

/* Defines -------------------------------------------------------------------*/
#define UART_TTL_HANDLER huart1
#define UART_TNC_HANDLER huart6

// Memory Map addresses
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

#define EEPROM_TNC_PATH_ADDRESS 0x03B0

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern char serialNumber[SERIAL_NUMBER_LENGTH];
extern char regNumber[REGISTRATION_NUMBER_LENGTH];
extern char callsign[CALLSIGN_LENGTH];
extern PwmConfiguration pwmConfig;
extern ActuatorsConfiguration actConf;
extern ActuatorsValues actDefaultValues;
extern SerialConfiguration serialConf;
extern I2CConfiguration i2cConf;
extern LcdConfiguration lcdConf;
extern SpiConfiguration spiConf;
extern IndConfiguration indConf;
extern AvSensor avSensors[3];
extern AlarmConfiguration alarms[2];
extern TriggerConfiguration triggers[NUMBER_OF_TRIGGERS];
extern OutputConfiguration outputConf;
extern char tncMessages[TNC_MESSAGE_COUNT][TNC_MESSAGE_LENGTH];
extern char tncPaths[TNC_PATH_COUNT][TNC_PATH_LENGTH];
extern VexufStatus vexufStatus;

/* Variables -----------------------------------------------------------------*/
uint16_t configVersion, configCount;

/* Prototypes ----------------------------------------------------------------*/
UF_STATUS
CONFIG_loadRegNumber(void);
UF_STATUS CONFIG_saveRegNumber(void);

UF_STATUS CONFIG_loadCallSign(void);
UF_STATUS CONFIG_saveCallSign(void);

UF_STATUS CONFIG_loadPwmConfigurations(void);
UF_STATUS CONFIG_savePwmConfigurations(void);

UF_STATUS CONFIG_loadActuators(void);
UF_STATUS CONFIG_saveActuators(void);

UF_STATUS CONFIG_loadSerialConf(void);
UF_STATUS CONFIG_saveSerialConf(void);

UF_STATUS CONFIG_loadI2cConf(void);
UF_STATUS CONFIG_saveI2cConf(void);
UF_STATUS CONFIG_loadLcdConf(void);
UF_STATUS CONFIG_saveLcdConf(void);

UF_STATUS CONFIG_loadSpiConfiguration(void);
UF_STATUS CONFIG_saveSpiConfiguration(void);

UF_STATUS CONFIG_loadOutputConf(void);
UF_STATUS CONFIG_saveOutputConf(void);

UF_STATUS CONFIG_loadIndicatorsConf(void);
UF_STATUS CONFIG_saveIndicatorsConf(void);

UF_STATUS CONFIG_loadAvSensors(void);
UF_STATUS CONFIG_saveAvSensors(void);

UF_STATUS CONFIG_loadAlarmsConf(void);
UF_STATUS CONFIG_saveAlarmsConf(void);

UF_STATUS CONFIG_loadTrigsConf(void);
UF_STATUS CONFIG_saveTrigsConf(void);

UF_STATUS CONFIG_loadTncMessages(void);
UF_STATUS CONFIG_saveTncMessages(void);

UF_STATUS CONFIG_loadTncPaths(void);
UF_STATUS CONFIG_saveTncPaths(void);
/* Code ----------------------------------------------------------------------*/

UF_STATUS CONFIG_IsConfigured(void) {
  uint16_t data;
  if (EEPROM_93C86_Read(EEPROM_CONFIG_FLAG_ADDRESS, &data) != UF_OK)
    return UF_ERROR;

  if (data == CONFIG_FLAG) {
    vexufStatus.isConfigured = 1;
    return UF_OK;
  }
  vexufStatus.isConfigured = 0;
  return UF_NOT_CONFIGURED;
}

UF_STATUS CONFIG_loadConfigValues(void) {
  if (CONFIG_IsConfigured() != UF_OK) {
    return UF_NOT_CONFIGURED;
  }

  if (EEPROM_93C86_Read(EEPROM_CONFIG_VERSION_ADDRESS, &configVersion) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_CONFIG_COUNT_ADDRESS, &configCount) != UF_OK)
    return UF_ERROR;

  return UF_OK;
}

UF_STATUS CONFIG_loadConfiguration(void) {
  if (CONFIG_loadConfigValues() != UF_OK) return UF_ERROR;

  if (CONFIG_loadRegNumber() != UF_OK) return UF_ERROR;
  if (CONFIG_loadCallSign() != UF_OK) return UF_ERROR;
  if (CONFIG_loadPwmConfigurations() != UF_OK) return UF_ERROR;
  if (CONFIG_loadActuators() != UF_OK) return UF_ERROR;
  if (CONFIG_loadSerialConf() != UF_OK) return UF_ERROR;
  if (CONFIG_loadI2cConf() != UF_OK) return UF_ERROR;
  if (CONFIG_loadLcdConf() != UF_OK) return UF_ERROR;
  if (CONFIG_loadSpiConfiguration() != UF_OK) return UF_ERROR;
  if (CONFIG_loadOutputConf() != UF_OK) return UF_ERROR;
  if (CONFIG_loadIndicatorsConf() != UF_OK) return UF_ERROR;
  if (CONFIG_loadAvSensors() != UF_OK) return UF_ERROR;
  if (CONFIG_loadAlarmsConf() != UF_OK) return UF_ERROR;
  if (CONFIG_loadTrigsConf() != UF_OK) return UF_ERROR;
  if (CONFIG_loadTncMessages() != UF_OK) return UF_ERROR;
  if (CONFIG_loadTncPaths() != UF_OK) return UF_ERROR;

  return UF_OK;
}

UF_STATUS CONFIG_saveConfiguration(void) {
  uint16_t confFlag = CONFIG_FLAG;
  uint16_t confVer = CONFIG_VERSION;

  if (CONFIG_saveRegNumber() != UF_OK) return UF_ERROR;
  if (CONFIG_saveCallSign() != UF_OK) return UF_ERROR;
  if (CONFIG_savePwmConfigurations() != UF_OK) return UF_ERROR;
  if (CONFIG_saveActuators() != UF_OK) return UF_ERROR;
  if (CONFIG_saveSerialConf() != UF_OK) return UF_ERROR;
  if (CONFIG_saveI2cConf() != UF_OK) return UF_ERROR;
  if (CONFIG_saveLcdConf() != UF_OK) return UF_ERROR;
  if (CONFIG_saveSpiConfiguration() != UF_OK) return UF_ERROR;
  if (CONFIG_saveOutputConf() != UF_OK) return UF_ERROR;
  if (CONFIG_saveIndicatorsConf() != UF_OK) return UF_ERROR;
  if (CONFIG_saveAvSensors() != UF_OK) return UF_ERROR;
  if (CONFIG_saveAlarmsConf() != UF_OK) return UF_ERROR;
  if (CONFIG_saveTrigsConf() != UF_OK) return UF_ERROR;
  if (CONFIG_saveTncMessages() != UF_OK) return UF_ERROR;
  if (CONFIG_saveTncPaths() != UF_OK) return UF_ERROR;

  if (EEPROM_93C86_Write(EEPROM_CONFIG_FLAG_ADDRESS, confFlag) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_CONFIG_VERSION_ADDRESS, confVer) != UF_OK)
    return UF_ERROR;
  if (CONFIG_WriteSerialNumber() != UF_OK) return UF_ERROR;

  configCount++;

  if (EEPROM_93C86_Write(EEPROM_CONFIG_COUNT_ADDRESS, configCount) != UF_OK)
    return UF_ERROR;

  vexufStatus.isConfigured = 1;
  return UF_OK;
}

UF_STATUS CONFIG_ReadSerialNumber(char* serialNumberBuffer) {
  uint16_t buffer[SERIAL_NUMBER_LENGTH / 2] = {0};
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_SERIAL_NUMBER_ADDRESS, buffer,
                                     SERIAL_NUMBER_LENGTH / 2) != UF_OK)
    return UF_ERROR;

  for (int i = 0; i < 12; i++) {
    serialNumberBuffer[2 * i] = (char)((buffer[i] >> 8) & 0xFF);  // MSB first
    serialNumberBuffer[2 * i + 1] = (char)(buffer[i] & 0xFF);     // LSB second
  }
  return UF_OK;
}
UF_STATUS CONFIG_WriteSerialNumber(void) {
  uint16_t vexufSerial = getSerialBytes();
  uint16_t eepromVexufSerial;
  if (EEPROM_93C86_Read(EEPROM_VEXUF_SERIAL_ADDRESS, &eepromVexufSerial) !=
      UF_OK)
    return UF_ERROR;

  if (vexufSerial != eepromVexufSerial) {
    if (EEPROM_93C86_Write(EEPROM_VEXUF_SERIAL_ADDRESS, vexufSerial) != UF_OK)
      return UF_ERROR;
  }

  char serialNumberString[SERIAL_NUMBER_LENGTH];
  VexUF_GenerateSerialNumber(serialNumberString);

  uint16_t buffer[SERIAL_NUMBER_LENGTH / 2] = {0};
  for (int i = 0; i < SERIAL_NUMBER_LENGTH / 2; i++) {
    // MSB first: higher byte from serialNumberString[2 * i],
    // lower byte from serialNumberString[2 * i + 1]
    buffer[i] = ((serialNumberString[2 * i] & 0xFF) << 8) |
                (serialNumberString[2 * i + 1] & 0xFF);
  }

  char eepromSerialNumberString[SERIAL_NUMBER_LENGTH];
  if (CONFIG_ReadSerialNumber(eepromSerialNumberString) != UF_OK)
    return UF_ERROR;

  // Convert eepromSerialNumberString to a buffer with MSB first for comparison
  uint16_t eepromBuffer[SERIAL_NUMBER_LENGTH / 2] = {0};
  for (int i = 0; i < SERIAL_NUMBER_LENGTH / 2; i++) {
    eepromBuffer[i] = ((eepromSerialNumberString[2 * i] & 0xFF) << 8) |
                      (eepromSerialNumberString[2 * i + 1] & 0xFF);
  }

  // Compare the two buffers
  if (memcmp(buffer, eepromBuffer, sizeof(buffer)) != 0) {
    if (EEPROM_93C86_WriteMultipleWords(EEPROM_SERIAL_NUMBER_ADDRESS, buffer,
                                        SERIAL_NUMBER_LENGTH / 2) != UF_OK)
      return UF_ERROR;
  }

  return UF_OK;
}

/* Private Methods -----------------------------------------------------------*/

UF_STATUS CONFIG_loadRegNumber(void) {
  uint16_t buffer[REGISTRATION_NUMBER_LENGTH / 2] = {0};
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_REGISTRATION_NUMBER_ADDRESS, buffer,
                                     REGISTRATION_NUMBER_LENGTH / 2) != UF_OK)
    return UF_ERROR;
  for (int i = 0; i < REGISTRATION_NUMBER_LENGTH / 2; i++) {
    regNumber[2 * i] = toupper((buffer[i] >> 8) & 0xFF);  // MSB first
    regNumber[2 * i + 1] = toupper(buffer[i] & 0xFF);     // LSB second
  }
  return UF_OK;
}
UF_STATUS CONFIG_saveRegNumber(void) {
  uint16_t buffer[REGISTRATION_NUMBER_LENGTH / 2] = {0};
  for (int i = 0; i < REGISTRATION_NUMBER_LENGTH / 2; i++) {
    char upperChar = toupper(regNumber[2 * i]);      // MSB
    char lowerChar = toupper(regNumber[2 * i + 1]);  // LSB

    buffer[i] =
        ((upperChar & 0xFF) << 8) | (lowerChar & 0xFF);  // MSB first, then LSB
  }
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_REGISTRATION_NUMBER_ADDRESS,
                                      buffer,
                                      REGISTRATION_NUMBER_LENGTH / 2) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadCallSign(void) {
  uint16_t buffer[CALLSIGN_LENGTH / 2] = {0};
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_CALLSIGN_ADDRESS, buffer,
                                     CALLSIGN_LENGTH / 2) != UF_OK)
    return UF_ERROR;

  for (int i = 0; i < CALLSIGN_LENGTH / 2; i++) {
    callsign[2 * i] = (buffer[i] >> 8) & 0xFF;  // MSB first
    callsign[2 * i + 1] = buffer[i] & 0xFF;     // LSB second
  }
  return UF_OK;
}
UF_STATUS CONFIG_saveCallSign(void) {
  uint16_t buffer[CALLSIGN_LENGTH / 2] = {0};
  for (int i = 0; i < CALLSIGN_LENGTH / 2; i++) {
    char upperChar = toupper(callsign[2 * i]);      // MSB
    char lowerChar = toupper(callsign[2 * i + 1]);  // LSB

    buffer[i] =
        ((upperChar & 0xFF) << 8) | (lowerChar & 0xFF);  // MSB first, then LSB
  }
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_CALLSIGN_ADDRESS, buffer,
                                      CALLSIGN_LENGTH / 2) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadPwmConfigurations(void) {
  uint16_t pwm1Enabled, pwm2Enabled, pwm1Value, pwm2Value;
  if (EEPROM_93C86_Read(EEPROM_PWM1_ENABLED_ADDRESS, &pwm1Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM1_DEFAULT_ADDRESS, &pwm1Value) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM2_ENABLED_ADDRESS, &pwm2Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM2_DEFAULT_ADDRESS, &pwm2Value) != UF_OK)
    return UF_ERROR;

  pwmConfig.pwm1Enabled = pwm1Enabled == 1 ? 1 : 0;
  pwmConfig.pwm2Enabled = pwm2Enabled == 1 ? 1 : 0;
  pwmConfig.pwm1Value = pwm1Value;
  pwmConfig.pwm1Value = pwm2Value;
  return UF_OK;
}
UF_STATUS CONFIG_savePwmConfigurations(void) {
  uint16_t pwm1Enabled = pwmConfig.pwm1Enabled ? 1 : 0;
  uint16_t pwm2Enabled = pwmConfig.pwm2Enabled ? 1 : 0;
  uint16_t pwm1Value = pwmConfig.pwm1Value;
  uint16_t pwm2Value = pwmConfig.pwm2Value;

  if (EEPROM_93C86_Write(EEPROM_PWM1_ENABLED_ADDRESS, pwm1Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM1_DEFAULT_ADDRESS, pwm1Value) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM2_ENABLED_ADDRESS, pwm2Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM2_DEFAULT_ADDRESS, pwm2Value) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadActuators(void) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_ACTUATORS_CONF_ADDRESS, buffer,
                                     2) != UF_OK)
    return UF_ERROR;

  actConf.actuators_enabled = buffer[0] & 0x1;
  actConf.actuators_lights_enabled = (buffer[0] >> 1) & 0x1;
  actDefaultValues.act1 = (buffer[1]) & 0x3;
  actDefaultValues.act2 = (buffer[1] >> 2) & 0x3;
  actDefaultValues.act3 = (buffer[1] >> 4) & 0x3;
  actDefaultValues.act4 = (buffer[1] >> 6) & 0x3;
  actDefaultValues.act5 = (buffer[1] >> 8) & 0x3;
  actDefaultValues.act6 = (buffer[1] >> 10) & 0x3;
  actDefaultValues.act7 = (buffer[1] >> 12) & 0x3;
  actDefaultValues.act8 = (buffer[1] >> 14) & 0x3;

  return UF_OK;
}
UF_STATUS CONFIG_saveActuators(void) {
  uint16_t buffer[2];
  buffer[0] = (actConf.actuators_enabled & 0x1) |
              ((actConf.actuators_lights_enabled & 0x1) << 1);
  // TODO: Check if we need those to be ActuatorsValues for the default?
  buffer[1] = (actDefaultValues.act1 & 0x3) |
              ((actDefaultValues.act2 & 0x3) << 2) |
              ((actDefaultValues.act3 & 0x3) << 4) |
              ((actDefaultValues.act4 & 0x3) << 6) |
              ((actDefaultValues.act5 & 0x3) << 8) |
              ((actDefaultValues.act6 & 0x3) << 10) |
              ((actDefaultValues.act7 & 0x3) << 12) |
              ((actDefaultValues.act8 & 0x3) << 14);

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_ACTUATORS_CONF_ADDRESS, buffer,
                                      2) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadSerialConf() {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_SERIAL_INTERFACE_ADDRESS, &buffer) != UF_OK)
    return UF_ERROR;

  serialConf.ttl_enabled = buffer & 0x1;
  serialConf.ttl_led_enabled = (buffer >> 1) & 0x1;
  serialConf.ttl_baud = (buffer >> 2) & 0xF;
  serialConf.ttlConf = (buffer >> 6) & 0x7;
  serialConf.tnc_enabled = (buffer >> 9) & 0x1;
  serialConf.tnc_baud = (buffer >> 10) & 0xF;

  return UF_OK;
}
UF_STATUS CONFIG_saveSerialConf(void) {
  uint16_t buffer = (serialConf.ttl_enabled & 0x1) |
                    ((serialConf.ttl_led_enabled & 0x1) << 1) |
                    ((serialConf.ttl_baud & 0xF) << 2) |
                    ((serialConf.ttlConf & 0x7) << 6) |
                    ((serialConf.tnc_enabled & 0x1) << 9) |
                    ((serialConf.tnc_baud & 0xF) << 10);

  if (EEPROM_93C86_Write(EEPROM_SERIAL_INTERFACE_ADDRESS, buffer) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadI2cConf(void) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_I2C_TYPE_ADDRESS, buffer, 2) !=
      UF_OK)
    return UF_ERROR;

  i2cConf.i2cAdd = buffer[0];
  i2cConf.i2cType = buffer[1];

  return UF_OK;
}
UF_STATUS CONFIG_saveI2cConf(void) {
  uint16_t buffer[2];
  buffer[0] = i2cConf.i2cAdd;
  buffer[1] = i2cConf.i2cType;

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_I2C_TYPE_ADDRESS, buffer, 2) !=
      UF_OK)
    return UF_ERROR;
  return UF_OK;
}
UF_STATUS CONFIG_loadLcdConf(void) {
  uint16_t buffer[3];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_LCD_TYPE_ADDRESS, buffer, 3) !=
      UF_OK)
    return UF_ERROR;

  lcdConf.lcdAdd = buffer[0];
  lcdConf.lcdType = buffer[1] & 0x3;
  lcdConf.lcdPwm = buffer[2];

  return UF_OK;
}
UF_STATUS CONFIG_saveLcdConf(void) {
  uint16_t buffer[3];
  buffer[0] = lcdConf.lcdAdd;
  buffer[1] = lcdConf.lcdType & 0x3;
  buffer[2] = lcdConf.lcdPwm;

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_LCD_TYPE_ADDRESS, buffer, 3) !=
      UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadSpiConfiguration(void) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_SPI_TYPE_ADDRESS, &buffer) != UF_OK)
    return UF_ERROR;
  spiConf = (SpiConfiguration)buffer;
  return UF_OK;
}
UF_STATUS CONFIG_saveSpiConfiguration(void) {
  if (EEPROM_93C86_Write(EEPROM_SPI_TYPE_ADDRESS, (uint16_t)spiConf) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadOutputConf(void) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_OUTPUT_ADDRESS, &buffer) != UF_OK)
    return UF_ERROR;
  outputConf.haltOnSdCardErrors = buffer & 0x1;
  outputConf.log_info_to_sd = (buffer >> 1) & 0x1;
  outputConf.log_warn_to_sd = (buffer >> 2) & 0x1;
  outputConf.log_error_to_sd = (buffer >> 3) & 0x1;
  return UF_OK;
}
UF_STATUS CONFIG_saveOutputConf(void) {
  uint16_t buffer = (outputConf.haltOnSdCardErrors & 0x1) |
                    ((outputConf.log_info_to_sd & 0x1) << 1) |
                    ((outputConf.log_warn_to_sd & 0x1) << 2) |
                    ((outputConf.log_error_to_sd & 0x1) << 3);

  if (EEPROM_93C86_Write(EEPROM_OUTPUT_ADDRESS, buffer) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadIndicatorsConf(void) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_INDICATORS_ADDRESS, &buffer) != UF_OK)
    return UF_ERROR;

  indConf.globalIndicatorEnabled = buffer & 0x1;
  indConf.buzzerEnabled = (buffer >> 1) & 0x1;
  indConf.buzzer1sEnabled = (buffer >> 2) & 0x1;
  indConf.buzzerHoldOnError = (buffer >> 3) & 0x1;
  indConf.statusIndicatorsEnabled = (buffer >> 4) & 0x1;
  indConf.sdCardIndicatorEnabled = (buffer >> 5) & 0x1;
  indConf.AvGlobalIndEnabled = (buffer >> 6) & 0x1;
  indConf.Av1IndEnabled = (buffer >> 7) & 0x1;
  indConf.Av2IndEnabled = (buffer >> 8) & 0x1;
  indConf.Av3IndEnabled = (buffer >> 9) & 0x1;
  return UF_OK;
}
UF_STATUS CONFIG_saveIndicatorsConf(void) {
  uint16_t buffer = (indConf.globalIndicatorEnabled & 0x1) |
                    ((indConf.buzzerEnabled & 0x1) << 1) |
                    ((indConf.buzzer1sEnabled & 0x1) << 2) |
                    ((indConf.buzzerHoldOnError & 0x1) << 3) |
                    ((indConf.statusIndicatorsEnabled & 0x1) << 4) |
                    ((indConf.sdCardIndicatorEnabled & 0x1) << 5) |
                    ((indConf.AvGlobalIndEnabled & 0x1) << 6) |
                    ((indConf.Av1IndEnabled & 0x1) << 7) |
                    ((indConf.Av2IndEnabled & 0x1) << 8) |
                    ((indConf.Av3IndEnabled & 0x1) << 9);

  if (EEPROM_93C86_Write(EEPROM_INDICATORS_ADDRESS, buffer) != UF_OK)
    return UF_ERROR;
  return UF_OK;
}

UF_STATUS CONFIG_loadAvSensors(void) {
  for (uint8_t i = 0; i < NUMBER_OF_AVS; i++) {
    uint16_t buffer[16];
    if (EEPROM_93C86_ReadMultipleWords(
            EEPROM_AV_ENABLED_ADDRESS + (EEPROM_AV_SHIFT * i), buffer, 16) !=
        UF_OK)
      return UF_ERROR;

    avSensors[i].enabled = buffer[0] & 0x1;
    avSensors[i].statusSlow = buffer[5] & 0x1;
    avSensors[i].minSlow = buffer[6];
    avSensors[i].maxSlow = buffer[7];
    avSensors[i].statusFast = buffer[9] & 0x1;
    avSensors[i].minFast = buffer[10];
    avSensors[i].maxFast = buffer[11];
    avSensors[i].statusOn = buffer[13] & 0x1;
    avSensors[i].minOn = buffer[14];
    avSensors[i].maxOn = buffer[15];
  }
  return UF_OK;
}
UF_STATUS CONFIG_saveAvSensors(void) {
  for (uint8_t i = 0; i < NUMBER_OF_AVS; i++) {
    uint16_t buffer[16];
    buffer[0] = avSensors[i].enabled & 0x1;
    buffer[5] = avSensors[i].statusSlow & 0x1;
    buffer[6] = avSensors[i].minSlow;
    buffer[7] = avSensors[i].maxSlow;
    buffer[9] = avSensors[i].statusFast & 0x1;
    buffer[10] = avSensors[i].minFast;
    buffer[11] = avSensors[i].maxFast;
    buffer[13] = avSensors[i].statusOn & 0x1;
    buffer[14] = avSensors[i].minOn;
    buffer[15] = avSensors[i].maxOn;

    if (EEPROM_93C86_WriteMultipleWords(
            EEPROM_AV_ENABLED_ADDRESS + (EEPROM_AV_SHIFT * i), buffer, 16) !=
        UF_OK)
      return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS CONFIG_loadAlarmsConf(void) {
  for (uint8_t i = 0; i < 2; i++) {
    uint16_t buffer[8];
    if (EEPROM_93C86_ReadMultipleWords(
            EEPROM_ALARM_ENABLED_ADDRESS + (EEPROM_ALARM_ENABLED_ADDRESS * i),
            buffer, 8) != UF_OK)
      return UF_ERROR;

    alarms[i].enabled = buffer[0] & 0x1;
    alarms[i].actuators.act1 = buffer[1] & 0x3;
    alarms[i].actuators.act2 = (buffer[1] >> 2) & 0x3;
    alarms[i].actuators.act3 = (buffer[1] >> 4) & 0x3;
    alarms[i].actuators.act4 = (buffer[1] >> 6) & 0x3;
    alarms[i].actuators.act5 = (buffer[1] >> 8) & 0x3;
    alarms[i].actuators.act6 = (buffer[1] >> 10) & 0x3;
    alarms[i].actuators.act7 = (buffer[1] >> 12) & 0x3;
    alarms[i].actuators.act8 = (buffer[1] >> 14) & 0x3;
    alarms[i].output.buzz = buffer[2] & 0x1;
    alarms[i].output.info = (buffer[2] >> 1) & 0x1;
    alarms[i].output.log = (buffer[2] >> 2) & 0x1;
    alarms[i].output.pwm1_change = (buffer[2] >> 3) & 0x1;
    alarms[i].output.pwm2_change = (buffer[2] >> 4) & 0x1;
    alarms[i].pwm1 = buffer[3];
    alarms[i].pwm2 = buffer[4];
    alarms[i].tncEnabled = buffer[5] & 0x1;
    alarms[i].tncMessage = (buffer[5] >> 1) & 0xF;
    alarms[i].tncPath = (buffer[5] >> 5) & 0x7;
  }
  return UF_OK;
}
UF_STATUS CONFIG_saveAlarmsConf(void) {
  for (uint8_t i = 0; i < 2; i++) {
    uint16_t buffer[8];
    buffer[0] = alarms[i].enabled & 0x1;
    buffer[1] = (alarms[i].actuators.act1 & 0x3) |
                ((alarms[i].actuators.act2 & 0x3) << 2) |
                ((alarms[i].actuators.act3 & 0x3) << 4) |
                ((alarms[i].actuators.act4 & 0x3) << 6) |
                ((alarms[i].actuators.act5 & 0x3) << 8) |
                ((alarms[i].actuators.act6 & 0x3) << 10) |
                ((alarms[i].actuators.act7 & 0x3) << 12) |
                ((alarms[i].actuators.act8 & 0x3) << 14);
    buffer[2] = (alarms[i].output.buzz & 0x1) |
                ((alarms[i].output.info & 0x1) << 1) |
                ((alarms[i].output.log & 0x1) << 2) |
                ((alarms[i].output.pwm1_change & 0x1) << 3) |
                ((alarms[i].output.pwm2_change & 0x1) << 4);
    buffer[3] = alarms[i].pwm1;
    buffer[4] = alarms[i].pwm2;
    buffer[5] = (alarms[i].tncEnabled & 0x1) |
                ((alarms[i].tncMessage & 0xF) << 1) |
                ((alarms[i].tncPath & 0x7) << 5);

    if (EEPROM_93C86_WriteMultipleWords(
            EEPROM_ALARM_ENABLED_ADDRESS + (EEPROM_ALARM_ENABLED_ADDRESS * i),
            buffer, 8) != UF_OK)
      return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS CONFIG_loadTrigsConf(void) {
  for (uint8_t i = 0; i < NUMBER_OF_TRIGGERS; i++) {
    uint16_t buffer[16];
    if (EEPROM_93C86_ReadMultipleWords(
            EEPROM_TRIG_STATUS_ADDRESS + (EEPROM_TRIG_SHIFT * i), buffer, 16) !=
        UF_OK)
      return UF_ERROR;

    triggers[i].status = buffer[0] & 0xF;
    triggers[i].fromValue = buffer[1];
    triggers[i].toValue = buffer[2];
    triggers[i].test = (buffer[3]) & 0xF;
    triggers[i].actuators.act1 = buffer[4] & 0x3;
    triggers[i].actuators.act2 = (buffer[4] >> 2) & 0x3;
    triggers[i].actuators.act3 = (buffer[4] >> 4) & 0x3;
    triggers[i].actuators.act4 = (buffer[4] >> 6) & 0x3;
    triggers[i].actuators.act5 = (buffer[4] >> 8) & 0x3;
    triggers[i].actuators.act6 = (buffer[4] >> 10) & 0x3;
    triggers[i].actuators.act7 = (buffer[4] >> 12) & 0x3;
    triggers[i].actuators.act8 = (buffer[4] >> 14) & 0x3;
    triggers[i].output.buzz = buffer[5] & 0x1;
    triggers[i].output.info = (buffer[5] >> 1) & 0x1;
    triggers[i].output.log = (buffer[5] >> 2) & 0x1;
    triggers[i].output.pwm1_change = (buffer[5] >> 3) & 0x1;
    triggers[i].output.pwm2_change = (buffer[5] >> 4) & 0x1;
    triggers[i].pwm1 = buffer[6];
    triggers[i].pwm2 = buffer[7];
    triggers[i].tnc1Enabled = buffer[8] & 0x1;
    triggers[i].tnc1Path = (buffer[9]) & 0x7;
    triggers[i].tnc1Message = (buffer[10]) & 0xF;
    triggers[i].tnc2Enabled = buffer[12] & 0x1;
    triggers[i].tnc2Path = (buffer[13]) & 0x7;
    triggers[i].tnc2Message = (buffer[14]) & 0xF;
  }

  return UF_OK;
}

UF_STATUS CONFIG_saveTrigsConf(void) {
  for (uint8_t i = 0; i < NUMBER_OF_TRIGGERS; i++) {
    uint16_t buffer[16] = {0};

    buffer[0] = triggers[i].status & 0xF;
    buffer[1] = triggers[i].fromValue;
    buffer[2] = triggers[i].toValue;
    buffer[3] = triggers[i].test & 0xF;
    buffer[4] = (triggers[i].actuators.act1 & 0x3) |
                ((triggers[i].actuators.act2 & 0x3) << 2) |
                ((triggers[i].actuators.act3 & 0x3) << 4) |
                ((triggers[i].actuators.act4 & 0x3) << 6) |
                ((triggers[i].actuators.act5 & 0x3) << 8) |
                ((triggers[i].actuators.act6 & 0x3) << 10) |
                ((triggers[i].actuators.act7 & 0x3) << 12) |
                ((triggers[i].actuators.act8 & 0x3) << 14);
    buffer[5] = (triggers[i].output.buzz & 0x1) |
                ((triggers[i].output.info & 0x1) << 1) |
                ((triggers[i].output.log & 0x1) << 2) |
                ((triggers[i].output.pwm1_change & 0x1) << 3) |
                ((triggers[i].output.pwm2_change & 0x1) << 4);
    buffer[6] = triggers[i].pwm1;
    buffer[7] = triggers[i].pwm2;
    buffer[8] = triggers[i].tnc1Enabled & 0x1;
    buffer[9] = triggers[i].tnc1Path & 0x7;
    buffer[10] = triggers[i].tnc1Message & 0xF;
    buffer[12] = triggers[i].tnc2Enabled & 0x1;
    buffer[13] = triggers[i].tnc2Path & 0x7;
    buffer[14] = triggers[i].tnc2Message & 0xF;

    if (EEPROM_93C86_WriteMultipleWords(
            EEPROM_TRIG_STATUS_ADDRESS + (EEPROM_TRIG_SHIFT * i), buffer, 16) !=
        UF_OK)
      return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS CONFIG_loadTncMessages(void) {
  for (uint8_t i = 0; i < TNC_MESSAGE_COUNT; i++) {
    uint16_t buffer[TNC_MESSAGE_LENGTH];
    if (EEPROM_93C86_ReadMultipleWords(
            EEPROM_TNC_MESSAGE_ADDRESS + ((TNC_MESSAGE_LENGTH / 2) * i), buffer,
            TNC_MESSAGE_LENGTH / 2) != UF_OK)
      return UF_ERROR;

    for (int i = 0; i < TNC_MESSAGE_LENGTH / 2; i++) {
      tncMessages[i][2 * i] = (buffer[i] >> 8) & 0xFF;
      tncMessages[i][2 * i + 1] = buffer[i] & 0xFF;
    }
  }
  return UF_OK;
}
UF_STATUS CONFIG_saveTncMessages(void) {
  for (uint8_t i = 0; i < TNC_MESSAGE_COUNT; i++) {
    uint16_t buffer[TNC_MESSAGE_LENGTH];
    for (int i = 0; i < TNC_MESSAGE_LENGTH / 2; i++) {
      buffer[i] = (tncMessages[i][2 * i] & 0xFF) << 8 |
                  ((tncMessages[i][2 * i + 1] & 0xFF));
    }

    if (EEPROM_93C86_WriteMultipleWords(
            EEPROM_TNC_MESSAGE_ADDRESS + ((TNC_MESSAGE_LENGTH / 2) * i), buffer,
            TNC_MESSAGE_LENGTH / 2) != UF_OK)
      return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS CONFIG_loadTncPaths(void) {
  for (uint8_t i = 0; i < TNC_PATH_COUNT; i++) {
    uint16_t buffer[TNC_PATH_LENGTH];
    if (EEPROM_93C86_ReadMultipleWords(
            EEPROM_TNC_PATH_ADDRESS + ((TNC_PATH_LENGTH / 2) * i), buffer,
            TNC_PATH_LENGTH / 2) != UF_OK)
      return UF_ERROR;

    for (int i = 0; i < TNC_PATH_LENGTH / 2; i++) {
      tncPaths[i][2 * i] = (buffer[i] >> 8) & 0xFF;
      tncPaths[i][2 * i + 1] = buffer[i] & 0xFF;
    }
  }
  return UF_OK;
}
UF_STATUS CONFIG_saveTncPaths(void) {
  for (uint8_t i = 0; i < TNC_PATH_COUNT; i++) {
    uint16_t buffer[TNC_PATH_LENGTH];
    for (int i = 0; i < TNC_PATH_LENGTH / 2; i++) {
      buffer[i] =
          (tncPaths[i][2 * i] & 0xFF) << 8 | ((tncPaths[i][2 * i + 1] & 0xFF));
    }

    if (EEPROM_93C86_WriteMultipleWords(
            EEPROM_TNC_PATH_ADDRESS + ((TNC_PATH_LENGTH / 2) * i), buffer,
            TNC_PATH_LENGTH / 2) != UF_OK)
      return UF_ERROR;
  }
  return UF_OK;
}
