/*
 * config.c
 *
 *  Created on: Jul 31, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_config.h"

#include <ctype.h>
#include <string.h>

#include "93c86.h"

extern char serialNumber[SERIAL_NUMBER_LENGTH];
extern char regNumber[REGISTRATION_NUMBER_LENGTH];
extern char callsign[CALLSIGN_LENGTH];
extern PwmConfiguration pwmConfig;
extern ActuatorsConfiguration actConf;
extern ActuatorsValues actValues;
extern SerialConfiguration serialConf;
extern I2CConfiguration i2cConf;
extern LcdConfiguration lcdConf;
extern SpiType spiConf;
extern IndConfiguration indConf;
extern AvSensor avSensors[3];
extern AlarmConfiguration alarms[2];
extern TriggerConfiguration triggers[NUMBER_OF_TRIGGERS];
// extern char tncMessages[EEPROM_TNC_MESSAGE_COUNT][EEPROM_TNC_MESSAGE_LENGTH];
// extern char tncPaths[EEPROM_TNC_PATH_COUNT][EEPROM_TNC_PATH_LENGTH];

extern VexufStatus vexufStatus;

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
UF_STATUS CONFIG_GetConfigValues(uint16_t* version, uint16_t* configCount) {
  if (!CONFIG_IsConfigured()) {
    return UF_NOT_CONFIGURED;
  }

  if (EEPROM_93C86_Read(EEPROM_CONFIG_VERSION_ADDRESS, version) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_CONFIG_COUNT_ADDRESS, configCount) != UF_OK)
    return UF_ERROR;

  return UF_OK;
}
UF_STATUS CONFIG_SetIsConfigured(void) {
  uint16_t confFlag = CONFIG_FLAG;
  uint16_t confVer = CONFIG_VERSION;
  if (EEPROM_93C86_Write(EEPROM_CONFIG_FLAG_ADDRESS, confFlag) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_CONFIG_VERSION_ADDRESS, confVer) != UF_OK)
    return UF_ERROR;
  if (CONFIG_WriteSerialNumber() != UF_OK) return UF_ERROR;

  uint16_t version = 0, confCount = 0;
  UF_STATUS status = CONFIG_GetConfigValues(&version, &confCount);
  if (status == UF_ERROR) return UF_ERROR;

  confCount++;

  if (EEPROM_93C86_Write(EEPROM_CONFIG_COUNT_ADDRESS, confCount) != UF_OK)
    return UF_ERROR;

  vexufStatus.isConfigured = 1;
  return UF_OK;
}

UF_STATUS CONFIG_ReadSerialNumber(char* serialNumberBuffer[24]) {
  uint16_t buffer[12] = {0};
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_SERIAL_NUMBER_ADDRESS, buffer,
                                     SERIAL_NUMBER_LENGTH / 2) != UF_OK)
    return UF_ERROR;

  for (int i = 0; i < 12; i++) {
    *serialNumberBuffer[2 * i] = (char)(buffer[i] & 0xFF);
    *serialNumberBuffer[2 * i + 1] = (char)((buffer[i] >> 8) & 0xFF);
  }
  return UF_OK;
}
UF_STATUS CONFIG_WriteSerialNumber(void) {
  // TODO: utilize generate serial number function instead.
  uint16_t vexufSerial = getSerialBytes();

  uint16_t buffer[SERIAL_NUMBER_LENGTH / 2] = {0};
  for (int i = 0; i < SERIAL_NUMBER_LENGTH / 2; i++) {
    buffer[i] =
        (serialNumber[2 * i] & 0xFF) | ((serialNumber[2 * i + 1] & 0xFF) << 8);
  }
  // TODO: return if eeprom already has the correct serial.
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_SERIAL_NUMBER_ADDRESS, buffer,
                                      SERIAL_NUMBER_LENGTH / 2) != UF_OK)
    return UF_ERROR;

  if (EEPROM_93C86_Write(EEPROM_VEXUF_SERIAL_ADDRESS, vexufSerial) != UF_OK)
    return UF_ERROR;

  return UF_OK;
}

UF_STATUS CONFIG_getRegNumber(uint32_t* regNumber) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_REGISTRATION_NUMBER_ADDRESS, buffer,
                                     REGISTRATION_NUMBER_LENGTH / 2) != UF_OK)
    return UF_ERROR;
  *regNumber = ((uint32_t)buffer[1] << 16) | buffer[0];
  return UF_OK;
}
UF_STATUS CONFIG_SetRegNumber(const uint32_t* newRegNumber) {
  uint16_t buffer[2];
  buffer[0] = (uint16_t)(*newRegNumber & 0xFFFF);          // Lower 16 bits
  buffer[1] = (uint16_t)((*newRegNumber >> 16) & 0xFFFF);  // Upper 16 bits
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_REGISTRATION_NUMBER_ADDRESS,
                                      buffer, 2) != UF_OK)
    return UF_ERROR;
  memcpy(&regNumber, newRegNumber, REGISTRATION_NUMBER_LENGTH);
  return UF_OK;
}

UF_STATUS CONFIG_getCallSign(char* callsign) {
  uint16_t buffer[CALLSIGN_LENGTH / 2] = {0};
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_CALLSIGN_ADDRESS, buffer,
                                     CALLSIGN_LENGTH / 2) != UF_OK)
    return UF_ERROR;
  for (int i = 0; i < CALLSIGN_LENGTH / 2; i++) {
    callsign[2 * i] = buffer[i] & 0xFF;
    callsign[2 * i + 1] = (buffer[i] >> 8) & 0xFF;
  }
  return UF_OK;
}
UF_STATUS CONFIG_setCallSign(const char* newCallSign) {
  uint16_t buffer[CALLSIGN_LENGTH / 2] = {0};
  for (int i = 0; i < CALLSIGN_LENGTH / 2; i++) {
    char upperChar = toupper((unsigned char)newCallSign[2 * i]);
    char lowerChar = toupper((unsigned char)newCallSign[2 * i + 1]);

    buffer[i] = (upperChar & 0xFF) | ((lowerChar & 0xFF) << 8);
  }
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_CALLSIGN_ADDRESS, buffer,
                                      CALLSIGN_LENGTH / 2) != UF_OK)
    return UF_ERROR;
  memcpy(&callsign, newCallSign, CALLSIGN_LENGTH);
  return UF_OK;
}

UF_STATUS CONFIG_getPwmConfigurations(PwmConfiguration* pwmConfigBuffer) {
  uint16_t pwm1Enabled, pwm2Enabled, pwm1Value, pwm2Value;
  if (EEPROM_93C86_Read(EEPROM_PWM1_ENABLED_ADDRESS, &pwm1Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM1_DEFAULT_ADDRESS, &pwm1Value) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM2_ENABLED_ADDRESS, &pwm2Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM2_DEFAULT_ADDRESS, &pwm2Value) != UF_OK)
    return UF_ERROR;

  pwmConfigBuffer->pwm1Enabled = pwm1Enabled == 1 ? 1 : 0;
  pwmConfigBuffer->pwm2Enabled = pwm2Enabled == 1 ? 1 : 0;
  pwmConfigBuffer->pwm1Value = pwm1Value;
  pwmConfigBuffer->pwm1Value = pwm2Value;
  return UF_OK;
}
UF_STATUS CONFIG_setPwmConfigurations(const PwmConfiguration* newPwmConfig) {
  uint16_t pwm1Enabled = newPwmConfig->pwm1Enabled ? 1 : 0;
  uint16_t pwm2Enabled = newPwmConfig->pwm2Enabled ? 1 : 0;
  uint16_t pwm1Value = newPwmConfig->pwm1Value;
  uint16_t pwm2Value = newPwmConfig->pwm2Value;

  if (EEPROM_93C86_Write(EEPROM_PWM1_ENABLED_ADDRESS, pwm1Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM1_DEFAULT_ADDRESS, pwm1Value) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM2_ENABLED_ADDRESS, pwm2Enabled) != UF_OK)
    return UF_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM2_DEFAULT_ADDRESS, pwm2Value) != UF_OK)
    return UF_ERROR;
  memcpy(&pwmConfig, newPwmConfig, sizeof(PwmConfiguration));
  return UF_OK;
}

UF_STATUS CONFIG_getActuators(ActuatorsConfiguration* actConf,
                              ActuatorsValues* ActValues) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_ACTUATORS_CONF_ADDRESS, buffer,
                                     2) != UF_OK)
    return UF_ERROR;

  actConf->actuators_enabled = buffer[0] & 0x1;
  actConf->actuators_lights_enabled = (buffer[0] >> 1) & 0x1;
  ActValues->act1 = (buffer[1]) & 0x3;
  ActValues->act2 = (buffer[1] >> 2) & 0x3;
  ActValues->act3 = (buffer[1] >> 4) & 0x3;
  ActValues->act4 = (buffer[1] >> 6) & 0x3;
  ActValues->act5 = (buffer[1] >> 8) & 0x3;
  ActValues->act6 = (buffer[1] >> 10) & 0x3;
  ActValues->act7 = (buffer[1] >> 12) & 0x3;
  ActValues->act8 = (buffer[1] >> 14) & 0x3;

  return UF_OK;
}
UF_STATUS CONFIG_setActuators(const ActuatorsConfiguration* newActConf,
                              const ActuatorsValues* newActValues) {
  uint16_t buffer[2];
  buffer[0] = (newActConf->actuators_enabled & 0x1) |
              ((newActConf->actuators_lights_enabled & 0x1) << 1);
  // TODO: Check if we need those to be ActuatorsValues for the default?
  buffer[1] =
      (newActValues->act1 & 0x3) | ((newActValues->act2 & 0x3) << 2) |
      ((newActValues->act3 & 0x3) << 4) | ((newActValues->act4 & 0x3) << 6) |
      ((newActValues->act5 & 0x3) << 8) | ((newActValues->act6 & 0x3) << 10) |
      ((newActValues->act7 & 0x3) << 12) | ((newActValues->act8 & 0x3) << 14);

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_ACTUATORS_CONF_ADDRESS, buffer,
                                      2) != UF_OK)
    return UF_ERROR;
  memcpy(&actConf, newActConf, sizeof(ActuatorsConfiguration));
  memcpy(&actValues, newActValues, sizeof(ActuatorsValues));

  return UF_OK;
}

UF_STATUS CONFIG_getSerialConf(SerialConfiguration* serialConf) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_SERIAL_INTERFACE_ADDRESS, &buffer) != UF_OK)
    return UF_ERROR;

  serialConf->ttl_enabled = buffer & 0x1;
  serialConf->ttl_led_enabled = (buffer >> 1) & 0x1;
  serialConf->ttl_baud = (buffer >> 2) & 0xF;
  serialConf->ttlConf = (buffer >> 6) & 0x7;
  serialConf->tnc_enabled = (buffer >> 9) & 0x1;
  serialConf->tnc__baud = (buffer >> 10) & 0xF;

  return UF_OK;
}
UF_STATUS CONFIG_setSerialConf(const SerialConfiguration* newSerialConf) {
  uint16_t buffer = (newSerialConf->ttl_enabled & 0x1) |
                    ((newSerialConf->ttl_led_enabled & 0x1) << 1) |
                    ((newSerialConf->ttl_baud & 0xF) << 2) |
                    ((newSerialConf->ttlConf & 0x7) << 6) |
                    ((newSerialConf->tnc_enabled & 0x1) << 9) |
                    ((newSerialConf->tnc__baud & 0xF) << 10);

  if (EEPROM_93C86_Write(EEPROM_SERIAL_INTERFACE_ADDRESS, buffer) != UF_OK)
    return UF_ERROR;
  memcpy(&serialConf, newSerialConf, sizeof(SerialConfiguration));
  return UF_OK;
}

UF_STATUS CONFIG_getI2cConf(I2CConfiguration* i2cConf) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_I2C_TYPE_ADDRESS, buffer, 2) !=
      UF_OK)
    return UF_ERROR;

  i2cConf->i2cAdd = buffer[0];
  i2cConf->i2cType = buffer[1];

  return UF_OK;
}
UF_STATUS CONFIG_setI2cConf(const I2CConfiguration* newI2cConf) {
  uint16_t buffer[2];
  buffer[0] = newI2cConf->i2cAdd;
  buffer[1] = newI2cConf->i2cType;

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_I2C_TYPE_ADDRESS, buffer, 2) !=
      UF_OK)
    return UF_ERROR;
  memcpy(&i2cConf, newI2cConf, sizeof(I2CConfiguration));
  return UF_OK;
}
UF_STATUS CONFIG_getLcdConf(LcdConfiguration* lcdConf) {
  uint16_t buffer[3];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_LCD_TYPE_ADDRESS, buffer, 3) !=
      UF_OK)
    return UF_ERROR;

  lcdConf->lcdAdd = buffer[0];
  lcdConf->lcdType = buffer[1] & 0x3;
  lcdConf->lcdPwm = buffer[2];

  return UF_OK;
}
UF_STATUS CONFIG_setLcdConf(const LcdConfiguration* newLcdConf) {
  uint16_t buffer[3];
  buffer[0] = newLcdConf->lcdAdd;
  buffer[1] = newLcdConf->lcdType & 0x3;
  buffer[2] = newLcdConf->lcdPwm;

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_LCD_TYPE_ADDRESS, buffer, 3) !=
      UF_OK)
    return UF_ERROR;
  memcpy(&lcdConf, newLcdConf, sizeof(LcdConfiguration));
  return UF_OK;
}

UF_STATUS CONFIG_getSPIType(SpiType* spiType) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_SPI_TYPE_ADDRESS, &buffer) != UF_OK)
    return UF_ERROR;
  *spiType = (SpiType)buffer;
  return UF_OK;
}
UF_STATUS CONFIG_setSPIType(const SpiType* newSpiType) {
  uint16_t buffer = (uint16_t)*newSpiType;
  if (EEPROM_93C86_Write(EEPROM_SPI_TYPE_ADDRESS, buffer) != UF_OK)
    return UF_ERROR;
  memcpy(&spiConf, newSpiType, sizeof(SpiType));
  return UF_OK;
}

// TODO: implement getter and setter for the Output Configuration

UF_STATUS CONFIG_getIndicatorsConf(IndConfiguration* indConf) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_INDICATORS_ADDRESS, &buffer) != UF_OK)
    return UF_ERROR;

  indConf->globalIndicatorEnabled = buffer & 0x1;
  indConf->buzzerEnabled = (buffer >> 1) & 0x1;
  indConf->buzzer1sEnabled = (buffer >> 2) & 0x1;
  indConf->buzzerHoldOnError = (buffer >> 3) & 0x1;
  indConf->statusIndicatorsEnabled = (buffer >> 4) & 0x1;
  indConf->sdCardIndicatorEnabled = (buffer >> 5) & 0x1;
  indConf->AvGlobalIndEnabled = (buffer >> 6) & 0x1;
  indConf->Av1IndEnabled = (buffer >> 7) & 0x1;
  indConf->Av2IndEnabled = (buffer >> 8) & 0x1;
  indConf->Av3IndEnabled = (buffer >> 9) & 0x1;
  return UF_OK;
}
UF_STATUS CONFIG_setIndicatorsConf(const IndConfiguration* newIndConf) {
  uint16_t buffer = (newIndConf->globalIndicatorEnabled & 0x1) |
                    ((newIndConf->buzzerEnabled & 0x1) << 1) |
                    ((newIndConf->buzzer1sEnabled & 0x1) << 2) |
                    ((newIndConf->buzzerHoldOnError & 0x1) << 3) |
                    ((newIndConf->statusIndicatorsEnabled & 0x1) << 4) |
                    ((newIndConf->sdCardIndicatorEnabled & 0x1) << 5) |
                    ((newIndConf->AvGlobalIndEnabled & 0x1) << 6) |
                    ((newIndConf->Av1IndEnabled & 0x1) << 7) |
                    ((newIndConf->Av2IndEnabled & 0x1) << 8) |
                    ((newIndConf->Av3IndEnabled & 0x1) << 9);

  if (EEPROM_93C86_Write(EEPROM_INDICATORS_ADDRESS, buffer) != UF_OK)
    return UF_ERROR;
  memcpy(&indConf, newIndConf, sizeof(IndConfiguration));
  return UF_OK;
}

UF_STATUS CONFIG_getAvSensor(AvSensor* av, uint8_t idx) {
  if (idx > 2) return UF_ERROR;

  uint16_t buffer[16];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_AV_ENABLED_ADDRESS + (EEPROM_AV_SHIFT * idx), buffer, 16) !=
      UF_OK)
    return UF_ERROR;

  av->enabled = buffer[0] & 0x1;
  av->statusSlow = buffer[5] & 0x1;
  av->minSlow = buffer[6];
  av->maxSlow = buffer[7];
  av->statusFast = buffer[9] & 0x1;
  av->minFast = buffer[10];
  av->maxFast = buffer[11];
  av->statusOn = buffer[13] & 0x1;
  av->minOn = buffer[14];
  av->maxOn = buffer[15];

  return UF_OK;
}
UF_STATUS CONFIG_SetAvSensor(const AvSensor* av, uint8_t idx) {
  if (idx > 2) return UF_ERROR;

  uint16_t buffer[16];
  buffer[0] = av->enabled & 0x1;
  buffer[5] = av->statusSlow & 0x1;
  buffer[6] = av->minSlow;
  buffer[7] = av->maxSlow;
  buffer[9] = av->statusFast & 0x1;
  buffer[10] = av->minFast;
  buffer[11] = av->maxFast;
  buffer[13] = av->statusOn & 0x1;
  buffer[14] = av->minOn;
  buffer[15] = av->maxOn;

  if (EEPROM_93C86_WriteMultipleWords(
          EEPROM_AV_ENABLED_ADDRESS + (EEPROM_AV_SHIFT * idx), buffer, 16) !=
      UF_OK)
    return UF_ERROR;
  memcpy(&avSensors[idx], av, sizeof(AvSensor));
  return UF_OK;
}

UF_STATUS CONFIG_getAlarmConf(AlarmConfiguration* alarm, uint8_t idx) {
  if (idx > 1) return UF_ERROR;
  uint16_t buffer[8];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_ALARM_ENABLED_ADDRESS + (EEPROM_ALARM_ENABLED_ADDRESS * idx),
          buffer, 8) != UF_OK)
    return UF_ERROR;

  alarm->enabled = buffer[0] & 0x1;
  alarm->actuators.act1 = buffer[1] & 0x3;
  alarm->actuators.act2 = (buffer[1] >> 2) & 0x3;
  alarm->actuators.act3 = (buffer[1] >> 4) & 0x3;
  alarm->actuators.act4 = (buffer[1] >> 6) & 0x3;
  alarm->actuators.act5 = (buffer[1] >> 8) & 0x3;
  alarm->actuators.act6 = (buffer[1] >> 10) & 0x3;
  alarm->actuators.act7 = (buffer[1] >> 12) & 0x3;
  alarm->actuators.act8 = (buffer[1] >> 14) & 0x3;
  alarm->output.buzz = buffer[2] & 0x1;
  alarm->output.info = (buffer[2] >> 1) & 0x1;
  alarm->output.log = (buffer[2] >> 2) & 0x1;
  alarm->output.pwm1_change = (buffer[2] >> 3) & 0x1;
  alarm->output.pwm2_change = (buffer[2] >> 4) & 0x1;
  alarm->pwm1 = buffer[3];
  alarm->pwm2 = buffer[4];
  alarm->tncEnabled = buffer[5] & 0x1;
  alarm->tncMessage = (buffer[5] >> 1) & 0xF;
  alarm->tncPath = (buffer[5] >> 5) & 0x7;
  return UF_OK;
}
UF_STATUS CONFIG_SetAlarmConf(const AlarmConfiguration* alarm, uint8_t idx) {
  if (idx > 1) return UF_ERROR;
  uint16_t buffer[8];
  buffer[0] = alarm->enabled & 0x1;
  buffer[1] = (alarm->actuators.act1 & 0x3) |
              ((alarm->actuators.act2 & 0x3) << 2) |
              ((alarm->actuators.act3 & 0x3) << 4) |
              ((alarm->actuators.act4 & 0x3) << 6) |
              ((alarm->actuators.act5 & 0x3) << 8) |
              ((alarm->actuators.act6 & 0x3) << 10) |
              ((alarm->actuators.act7 & 0x3) << 12) |
              ((alarm->actuators.act8 & 0x3) << 14);
  buffer[2] = (alarm->output.buzz & 0x1) | ((alarm->output.info & 0x1) << 1) |
              ((alarm->output.log & 0x1) << 2) |
              ((alarm->output.pwm1_change & 0x1) << 3) |
              ((alarm->output.pwm2_change & 0x1) << 4);
  buffer[3] = alarm->pwm1;
  buffer[4] = alarm->pwm2;
  buffer[5] = (alarm->tncEnabled & 0x1) | ((alarm->tncMessage & 0xF) << 1) |
              ((alarm->tncPath & 0x7) << 5);

  if (EEPROM_93C86_WriteMultipleWords(
          EEPROM_ALARM_ENABLED_ADDRESS + (EEPROM_ALARM_ENABLED_ADDRESS * idx),
          buffer, 8) != UF_OK)
    return UF_ERROR;
  memcpy(&alarms[idx], alarm, sizeof(AlarmConfiguration));
  return UF_OK;
}

UF_STATUS CONFIG_getTrigConf(TriggerConfiguration* trigConf, uint8_t idx) {
  if (idx >= NUMBER_OF_TRIGGERS) return UF_ERROR;

  uint16_t buffer[16];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_TRIG_STATUS_ADDRESS + (EEPROM_TRIG_SHIFT * idx), buffer, 16) !=
      UF_OK)
    return UF_ERROR;

  trigConf->status = buffer[0] & 0xF;
  trigConf->fromValue = buffer[1];
  trigConf->toValue = buffer[2];
  trigConf->test = (buffer[3]) & 0xF;
  trigConf->actuators.act1 = buffer[4] & 0x3;
  trigConf->actuators.act2 = (buffer[4] >> 2) & 0x3;
  trigConf->actuators.act3 = (buffer[4] >> 4) & 0x3;
  trigConf->actuators.act4 = (buffer[4] >> 6) & 0x3;
  trigConf->actuators.act5 = (buffer[4] >> 8) & 0x3;
  trigConf->actuators.act6 = (buffer[4] >> 10) & 0x3;
  trigConf->actuators.act7 = (buffer[4] >> 12) & 0x3;
  trigConf->actuators.act8 = (buffer[4] >> 14) & 0x3;
  trigConf->output.buzz = buffer[5] & 0x1;
  trigConf->output.info = (buffer[5] >> 1) & 0x1;
  trigConf->output.log = (buffer[5] >> 2) & 0x1;
  trigConf->output.pwm1_change = (buffer[5] >> 3) & 0x1;
  trigConf->output.pwm2_change = (buffer[5] >> 4) & 0x1;
  trigConf->pwm1 = buffer[6];
  trigConf->pwm2 = buffer[7];
  trigConf->tnc1Enabled = buffer[8] & 0x1;
  trigConf->tnc1Path = (buffer[9]) & 0x7;
  trigConf->tnc1Message = (buffer[10]) & 0xF;
  trigConf->tnc2Enabled = buffer[12] & 0x1;
  trigConf->tnc2Path = (buffer[13]) & 0x7;
  trigConf->tnc2Message = (buffer[14]) & 0xF;

  return UF_OK;
}

UF_STATUS CONFIG_setTrigConf(const TriggerConfiguration* trigConf,
                             uint8_t idx) {
  if (idx >= NUMBER_OF_TRIGGERS) return UF_ERROR;

  uint16_t buffer[16] = {0};

  buffer[0] = trigConf->status & 0xF;
  buffer[1] = trigConf->fromValue;
  buffer[2] = trigConf->toValue;
  buffer[3] = trigConf->test & 0xF;
  buffer[4] = (trigConf->actuators.act1 & 0x3) |
              ((trigConf->actuators.act2 & 0x3) << 2) |
              ((trigConf->actuators.act3 & 0x3) << 4) |
              ((trigConf->actuators.act4 & 0x3) << 6) |
              ((trigConf->actuators.act5 & 0x3) << 8) |
              ((trigConf->actuators.act6 & 0x3) << 10) |
              ((trigConf->actuators.act7 & 0x3) << 12) |
              ((trigConf->actuators.act8 & 0x3) << 14);
  buffer[5] = (trigConf->output.buzz & 0x1) |
              ((trigConf->output.info & 0x1) << 1) |
              ((trigConf->output.log & 0x1) << 2) |
              ((trigConf->output.pwm1_change & 0x1) << 3) |
              ((trigConf->output.pwm2_change & 0x1) << 4);
  buffer[6] = trigConf->pwm1;
  buffer[7] = trigConf->pwm2;
  buffer[8] = trigConf->tnc1Enabled & 0x1;
  buffer[9] = trigConf->tnc1Path & 0x7;
  buffer[10] = trigConf->tnc1Message & 0xF;
  buffer[12] = trigConf->tnc2Enabled & 0x1;
  buffer[13] = trigConf->tnc2Path & 0x7;
  buffer[14] = trigConf->tnc2Message & 0xF;

  if (EEPROM_93C86_WriteMultipleWords(
          EEPROM_TRIG_STATUS_ADDRESS + (EEPROM_TRIG_SHIFT * idx), buffer, 16) !=
      UF_OK)
    return UF_ERROR;
  memcpy(&triggers[idx], trigConf, sizeof(TriggerConfiguration));
  return UF_OK;
}

UF_STATUS CONFIG_getTncMessage(char* message, uint8_t idx) {
  if (idx >= EEPROM_TNC_MESSAGE_COUNT) return UF_ERROR;

  uint16_t buffer[EEPROM_TNC_MESSAGE_LENGTH];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_TNC_MESSAGE_ADDRESS + (EEPROM_TNC_MESSAGE_LENGTH * idx),
          buffer, EEPROM_TNC_MESSAGE_LENGTH) != UF_OK)
    return UF_ERROR;

  for (int i = 0; i < EEPROM_TNC_MESSAGE_LENGTH; i++) {
    message[2 * i] = (buffer[i] >> 8) & 0xFF;
    message[2 * i + 1] = buffer[i] & 0xFF;
  }
  return UF_OK;
}
UF_STATUS CONFIG_setTncMessage(const char* message, uint8_t idx) {
  if (idx >= EEPROM_TNC_MESSAGE_COUNT) return UF_ERROR;

  uint16_t buffer[EEPROM_TNC_MESSAGE_LENGTH];
  for (int i = 0; i < EEPROM_TNC_MESSAGE_LENGTH; i++) {
    buffer[i] = (message[2 * i] & 0xFF) << 8 | ((message[2 * i + 1] & 0xFF));
  }

  if (EEPROM_93C86_WriteMultipleWords(
          EEPROM_TNC_MESSAGE_ADDRESS + (EEPROM_TNC_MESSAGE_LENGTH * idx),
          buffer, EEPROM_TNC_MESSAGE_LENGTH) != UF_OK)
    return UF_ERROR;

  return UF_OK;
}

UF_STATUS CONFIG_getTncPath(char* tncPath, uint8_t idx) {
  if (idx >= EEPROM_TNC_PATH_COUNT) return UF_ERROR;

  uint16_t buffer[EEPROM_TNC_PATH_LENGTH];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_TNC_PATH_ADDRESS + (EEPROM_TNC_PATH_LENGTH * idx), buffer,
          EEPROM_TNC_MESSAGE_LENGTH) != UF_OK)
    return UF_ERROR;

  for (int i = 0; i < EEPROM_TNC_MESSAGE_LENGTH; i++) {
    tncPath[2 * i] = (buffer[i] >> 8) & 0xFF;
    tncPath[2 * i + 1] = buffer[i] & 0xFF;
  }
  return UF_OK;
}
UF_STATUS CONFIG_setTncPath(const char* tncPath, uint8_t idx) {
  if (idx >= EEPROM_TNC_PATH_COUNT) return UF_ERROR;

  uint16_t buffer[EEPROM_TNC_PATH_LENGTH];
  for (int i = 0; i < EEPROM_TNC_PATH_LENGTH; i++) {
    buffer[i] = (tncPath[2 * i] & 0xFF) << 8 | ((tncPath[2 * i + 1] & 0xFF));
  }

  if (EEPROM_93C86_WriteMultipleWords(
          EEPROM_TNC_PATH_ADDRESS + (EEPROM_TNC_PATH_LENGTH * idx), buffer,
          EEPROM_TNC_PATH_LENGTH) != UF_OK)
    return UF_ERROR;

  return UF_OK;
}
