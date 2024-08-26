/*
 * config.c
 *
 *  Created on: Jul 31, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_config.h"

#include "93c86.h"

extern VexufStatus vexufStatus;
extern char serialNumber[24];

CONFIG_STATUS CONFIG_IsConfigured(void) {
  uint16_t data;
  if (EEPROM_93C86_Read(EEPROM_CONFIG_FLAG_ADDRESS, &data) != EEPROM_OK)
    return CONFIG_ERROR;

  if (data == CONFIG_FLAG) {
    vexufStatus.isConfigured = 1;
    return CONFIG_OK;
  }
  vexufStatus.isConfigured = 0;
  return CONFIG_NOT_CONFIGURED;
}
CONFIG_STATUS CONFIG_GetConfigValues(uint16_t* version, uint16_t* configCount) {
  if (!CONFIG_IsConfigured()) {
    return CONFIG_NOT_CONFIGURED;
  }

  if (EEPROM_93C86_Read(EEPROM_CONFIG_VERSION_ADDRESS, version) != EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Read(EEPROM_CONFIG_COUNT_ADDRESS, configCount) != EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_SetIsConfigured(void) {
  uint16_t confFlag = CONFIG_FLAG;
  uint16_t confVer = CONFIG_VERSION;
  if (EEPROM_93C86_Write(EEPROM_CONFIG_FLAG_ADDRESS, &confFlag) != EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Write(EEPROM_CONFIG_VERSION_ADDRESS, &confVer) != EEPROM_OK)
    return CONFIG_ERROR;
  if (CONFIG_WriteSerialNumber() != CONFIG_OK) return CONFIG_ERROR;

  uint16_t version = 0, confCount = 0;
  CONFIG_STATUS status = CONFIG_GetConfigValues(&version, &confCount);
  if (status != CONFIG_OK) {
    return CONFIG_ERROR;
  }
  confCount++;

  if (EEPROM_93C86_Write(EEPROM_CONFIG_COUNT_ADDRESS, &confCount) != EEPROM_OK)
    return CONFIG_ERROR;

  vexufStatus.isConfigured = 1;
  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_ReadSerialNumber(char* serialNumberBuffer[24]) {
  uint16_t buffer[12] = {0};
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_SERIAL_NUMBER_ADDRESS, buffer,
                                     EEPROM_SERIAL_NUMBER_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;

  for (int i = 0; i < 12; i++) {
    *serialNumberBuffer[2 * i] = (char)(buffer[i] & 0xFF);
    *serialNumberBuffer[2 * i + 1] = (char)((buffer[i] >> 8) & 0xFF);
  }
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_WriteSerialNumber(void) {
  // TODO: return if eeprom already has the correct serial.

  uint16_t vexufSerial = getSerialBytes();

  uint16_t buffer[EEPROM_SERIAL_NUMBER_LENGTH] = {0};
  for (int i = 0; i < EEPROM_SERIAL_NUMBER_LENGTH; i++) {
    buffer[i] =
        (serialNumber[2 * i] & 0xFF) | ((serialNumber[2 * i + 1] & 0xFF) << 8);
  }
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_SERIAL_NUMBER_ADDRESS, buffer,
                                      EEPROM_SERIAL_NUMBER_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;

  if (EEPROM_93C86_Write(EEPROM_VEXUF_SERIAL_ADDRESS, &vexufSerial) !=
      EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getRegNumber(uint32_t* regNumber) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_REGISTRATION_NUMBER_ADDRESS, buffer,
                                     2) != EEPROM_OK)
    return CONFIG_ERROR;
  *regNumber = ((uint32_t)buffer[1] << 16) | buffer[0];
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_SetRegNumber(const uint32_t* regNumber) {
  uint16_t buffer[2];
  buffer[0] = (uint16_t)(*regNumber & 0xFFFF);          // Lower 16 bits
  buffer[1] = (uint16_t)((*regNumber >> 16) & 0xFFFF);  // Upper 16 bits
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_REGISTRATION_NUMBER_ADDRESS,
                                      buffer, 2) != EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_LoadCallSign(char* callsign[CALLSIGN_LENGTH]) {
  uint16_t buffer[EEPROM_CALLSIGN_LENGTH] = {0};
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_CALLSIGN_ADDRESS, buffer,
                                     EEPROM_CALLSIGN_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;
  for (int i = 0; i < EEPROM_CALLSIGN_LENGTH; i++) {
    *callsign[2 * i] = buffer[i] & 0xFF;
    *callsign[2 * i + 1] = (buffer[i] >> 8) & 0xFF;
  }
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_SetCallSign(const char* newCallSign[CALLSIGN_LENGTH]) {
  uint16_t buffer[EEPROM_CALLSIGN_LENGTH] = {0};
  for (int i = 0; i < EEPROM_CALLSIGN_LENGTH; i++) {
    buffer[i] =
        (*newCallSign[2 * i] & 0xFF) | ((*newCallSign[2 * i + 1] & 0xFF) << 8);
  }
  if (EEPROM_93C86_WriteMultipleWords(EEPROM_CALLSIGN_ADDRESS, buffer,
                                      EEPROM_CALLSIGN_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;
  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getPwmConfigurations(PwmConfiguration* pwmConfigBuffer) {
  uint16_t pwm1Enabled, pwm2Enabled, pwm1Value, pwm2Value;
  if (EEPROM_93C86_Read(EEPROM_PWM1_ENABLED_ADDRESS, &pwm1Enabled) != EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM1_DEFAULT_ADDRESS, &pwm1Value) != EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM2_ENABLED_ADDRESS, &pwm2Enabled) != EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Read(EEPROM_PWM2_DEFAULT_ADDRESS, &pwm2Value) != EEPROM_OK)
    return CONFIG_ERROR;

  pwmConfigBuffer->pwm1Enabled = pwm1Enabled == 1 ? 1 : 0;
  pwmConfigBuffer->pwm2Enabled = pwm2Enabled == 1 ? 1 : 0;
  pwmConfigBuffer->pwm1Value = pwm1Value;
  pwmConfigBuffer->pwm1Value = pwm2Value;
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setPwmConfigurations(const PwmConfiguration* pwmConfig) {
  uint16_t pwm1Enabled = pwmConfig->pwm1Enabled ? 1 : 0;
  uint16_t pwm2Enabled = pwmConfig->pwm2Enabled ? 1 : 0;
  uint16_t pwm1Value = pwmConfig->pwm1Value;
  uint16_t pwm2Value = pwmConfig->pwm2Value;

  if (EEPROM_93C86_Write(EEPROM_PWM1_ENABLED_ADDRESS, &pwm1Enabled) !=
      EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM1_DEFAULT_ADDRESS, &pwm1Value) != EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM2_ENABLED_ADDRESS, &pwm2Enabled) !=
      EEPROM_OK)
    return CONFIG_ERROR;
  if (EEPROM_93C86_Write(EEPROM_PWM2_DEFAULT_ADDRESS, &pwm2Value) != EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getActuators(ActuatorsConfiguration* actConf,
                                  ActuatorsValues* ActValues) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_ACTUATORS_CONF_ADDRESS, buffer,
                                     2) != EEPROM_OK)
    return CONFIG_ERROR;

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

  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setActuators(const ActuatorsConfiguration* actConf,
                                  const ActuatorsValues* actValues) {
  uint16_t buffer[2];
  buffer[0] = (actConf->actuators_enabled & 0x1) |
              ((actConf->actuators_lights_enabled & 0x1) << 1);
  buffer[1] = (actValues->act1 & 0x3) | ((actValues->act2 & 0x3) << 2) |
              ((actValues->act3 & 0x3) << 4) | ((actValues->act4 & 0x3) << 6) |
              ((actValues->act5 & 0x3) << 8) | ((actValues->act6 & 0x3) << 10) |
              ((actValues->act7 & 0x3) << 12) | ((actValues->act8 & 0x3) << 14);

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_ACTUATORS_CONF_ADDRESS, buffer,
                                      2) != EEPROM_OK)
    return CONFIG_ERROR;
  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getSerialConf(SerialConfiguration* serialConf) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_SERIAL_INTERFACE_ADDRESS, &buffer) != EEPROM_OK)
    return CONFIG_ERROR;

  serialConf->ttl_enabled = buffer & 0x1;
  serialConf->ttl_led_enabled = (buffer >> 1) & 0x1;
  serialConf->ttl_baud = (buffer >> 2) & 0xF;
  serialConf->ttlConf = (buffer >> 6) & 0x7;
  serialConf->tnc_enabled = (buffer >> 9) & 0x1;
  serialConf->tnc__baud = (buffer >> 10) & 0xF;

  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setSerialConf(const SerialConfiguration* serialConf) {
  uint16_t buffer = (serialConf->ttl_enabled & 0x1) |
                    ((serialConf->ttl_led_enabled & 0x1) << 1) |
                    ((serialConf->ttl_baud & 0xF) << 2) |
                    ((serialConf->ttlConf & 0x7) << 6) |
                    ((serialConf->tnc_enabled & 0x1) << 9) |
                    ((serialConf->tnc__baud & 0xF) << 10);

  if (EEPROM_93C86_Write(EEPROM_SERIAL_INTERFACE_ADDRESS, &buffer) != EEPROM_OK)
    return CONFIG_ERROR;
  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getI2cConf(I2CConfiguration* i2cConf) {
  uint16_t buffer[2];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_I2C_TYPE_ADDRESS, buffer, 2) !=
      EEPROM_OK)
    return CONFIG_ERROR;

  i2cConf->i2cAdd = buffer[0];
  i2cConf->i2cType = buffer[1];

  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setI2cConf(const I2CConfiguration* i2cConf) {
  uint16_t buffer[2];
  buffer[0] = i2cConf->i2cAdd;
  buffer[1] = i2cConf->i2cType;

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_I2C_TYPE_ADDRESS, buffer, 2) !=
      EEPROM_OK)
    return CONFIG_ERROR;
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_getLcdConf(LcdConfiguration* lcdConf) {
  uint16_t buffer[3];
  if (EEPROM_93C86_ReadMultipleWords(EEPROM_LCD_TYPE_ADDRESS, buffer, 3) !=
      EEPROM_OK)
    return CONFIG_ERROR;

  lcdConf->lcdAdd = buffer[0];
  lcdConf->lcdType = buffer[1] & 0x3;
  lcdConf->lcdPwm = buffer[2];

  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setLcdConf(const LcdConfiguration* lcdConf) {
  uint16_t buffer[3];
  buffer[0] = lcdConf->lcdAdd;
  buffer[1] = lcdConf->lcdType & 0x3;
  buffer[2] = lcdConf->lcdPwm;

  if (EEPROM_93C86_WriteMultipleWords(EEPROM_LCD_TYPE_ADDRESS, buffer, 3) !=
      EEPROM_OK)
    return CONFIG_ERROR;
  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getSPIType(SpiType* spiType) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_SPI_TYPE_ADDRESS, &buffer) != EEPROM_OK)
    return CONFIG_ERROR;
  *spiType = (SpiType)buffer;
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setSPIType(const SpiType* spiType) {
  uint16_t buffer = (uint16_t)*spiType;
  if (EEPROM_93C86_Write(EEPROM_SPI_TYPE_ADDRESS, &buffer) != EEPROM_OK)
    return CONFIG_ERROR;
  return CONFIG_OK;
}

// TODO: implement getter and setter for the Output Configuration

CONFIG_STATUS CONFIG_getIndicatorsConf(IndConfiguration* indConf) {
  uint16_t buffer;
  if (EEPROM_93C86_Read(EEPROM_INDICATORS_ADDRESS, &buffer) != EEPROM_OK)
    return CONFIG_ERROR;

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
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setIndicatorsConf(const IndConfiguration* indConf) {
  uint16_t buffer = (indConf->globalIndicatorEnabled & 0x1) |
                    ((indConf->buzzerEnabled & 0x1) << 1) |
                    ((indConf->buzzer1sEnabled & 0x1) << 2) |
                    ((indConf->buzzerHoldOnError & 0x1) << 3) |
                    ((indConf->statusIndicatorsEnabled & 0x1) << 4) |
                    ((indConf->sdCardIndicatorEnabled & 0x1) << 5) |
                    ((indConf->AvGlobalIndEnabled & 0x1) << 6) |
                    ((indConf->Av1IndEnabled & 0x1) << 7) |
                    ((indConf->Av2IndEnabled & 0x1) << 8) |
                    ((indConf->Av3IndEnabled & 0x1) << 9);

  if (EEPROM_93C86_Write(EEPROM_INDICATORS_ADDRESS, &buffer) != EEPROM_OK)
    return CONFIG_ERROR;
  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getAvSensor(AvSensor* av, uint8_t idx) {
  if (idx > 2) return CONFIG_ERROR;

  uint16_t buffer[16];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_AV_ENABLED_ADDRESS + (EEPROM_AV_SHIFT * idx), buffer, 16) !=
      EEPROM_OK)
    return CONFIG_ERROR;

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

  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_SetAvSensor(const AvSensor* av, uint8_t idx) {
  if (idx > 2) return CONFIG_ERROR;

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
      EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getAlarmConf(AlarmConfiguration* alarm, uint8_t idx) {
  if (idx > 1) return CONFIG_ERROR;
  uint16_t buffer[8];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_ALARM_ENABLED_ADDRESS + (EEPROM_ALARM_ENABLED_ADDRESS * idx),
          buffer, 8) != EEPROM_OK)
    return CONFIG_ERROR;

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
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_SetAlarmConf(const AlarmConfiguration* alarm,
                                  uint8_t idx) {
  if (idx > 1) return CONFIG_ERROR;
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
          buffer, 8) != EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getTrigConf(TriggerConfiguration* trigConf, uint8_t idx) {
  if (idx >= NUMBER_OF_TRIGGERS) return CONFIG_ERROR;

  uint16_t buffer[16];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_TRIG_STATUS_ADDRESS + (EEPROM_TRIG_SHIFT * idx), buffer, 16) !=
      EEPROM_OK)
    return CONFIG_ERROR;

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

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_setTrigConf(const TriggerConfiguration* trigConf,
                                 uint8_t idx) {
  if (idx >= NUMBER_OF_TRIGGERS) return CONFIG_ERROR;

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
      EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getTncMessage(char* message, uint8_t idx) {
  if (idx >= EEPROM_TNC_MESSAGE_COUNT) return CONFIG_ERROR;

  uint16_t buffer[EEPROM_TNC_MESSAGE_LENGTH];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_TNC_MESSAGE_ADDRESS + (EEPROM_TNC_MESSAGE_LENGTH * idx),
          buffer, EEPROM_TNC_MESSAGE_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;

  for (int i = 0; i < EEPROM_TNC_MESSAGE_LENGTH; i++) {
    message[2 * i] = (buffer[i] >> 8) & 0xFF;
    message[2 * i + 1] = buffer[i] & 0xFF;
  }
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setTncMessage(const char* message, uint8_t idx) {
  if (idx >= EEPROM_TNC_MESSAGE_COUNT) return CONFIG_ERROR;

  uint16_t buffer[EEPROM_TNC_MESSAGE_LENGTH];
  for (int i = 0; i < EEPROM_TNC_MESSAGE_LENGTH; i++) {
    buffer[i] = (message[2 * i] & 0xFF) << 8 | ((message[2 * i + 1] & 0xFF));
  }

  if (EEPROM_93C86_WriteMultipleWords(
          EEPROM_TNC_MESSAGE_ADDRESS + (EEPROM_TNC_MESSAGE_LENGTH * idx),
          buffer, EEPROM_TNC_MESSAGE_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}

CONFIG_STATUS CONFIG_getTncPath(char* tncPath, uint8_t idx) {
  if (idx >= EEPROM_TNC_PATH_COUNT) return CONFIG_ERROR;

  uint16_t buffer[EEPROM_TNC_PATH_LENGTH];
  if (EEPROM_93C86_ReadMultipleWords(
          EEPROM_TNC_PATH_ADDRESS + (EEPROM_TNC_PATH_LENGTH * idx), buffer,
          EEPROM_TNC_MESSAGE_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;

  for (int i = 0; i < EEPROM_TNC_MESSAGE_LENGTH; i++) {
    tncPath[2 * i] = (buffer[i] >> 8) & 0xFF;
    tncPath[2 * i + 1] = buffer[i] & 0xFF;
  }
  return CONFIG_OK;
}
CONFIG_STATUS CONFIG_setTncPath(const char* tncPath, uint8_t idx) {
  if (idx >= EEPROM_TNC_PATH_COUNT) return CONFIG_ERROR;

  uint16_t buffer[EEPROM_TNC_PATH_LENGTH];
  for (int i = 0; i < EEPROM_TNC_PATH_LENGTH; i++) {
    buffer[i] = (tncPath[2 * i] & 0xFF) << 8 | ((tncPath[2 * i + 1] & 0xFF));
  }

  if (EEPROM_93C86_WriteMultipleWords(
          EEPROM_TNC_PATH_ADDRESS + (EEPROM_TNC_PATH_LENGTH * idx), buffer,
          EEPROM_TNC_PATH_LENGTH) != EEPROM_OK)
    return CONFIG_ERROR;

  return CONFIG_OK;
}
