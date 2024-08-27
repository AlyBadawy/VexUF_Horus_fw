/*
 * actuators.c
 *
 *  Created on: Jul 22, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_actuators.h"

#include "74hc595d.h"

uint8_t actuatorsData = 0;
ActuatorsConfiguration actConf = {0};

void ACT_Init(ActuatorsConfiguration* newActConf) {
  actConf.actuators_enabled = newActConf->actuators_enabled;
  actConf.actuators_lights_enabled = newActConf->actuators_lights_enabled;
}

UF_STATUS ACTUATORS_setPin(ActuatorPin pin) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;
  actuatorsData |= (1 << (pin));
}
UF_STATUS ACTUATORS_resetPin(ActuatorPin pin) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;
  actuatorsData &= ~(1 << (pin));
}

UF_STATUS ACTUATORS_Update(void) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;

  SHIFTREG_74HC595D_update(actuatorsData);  // TODO: Get a return status
  return UF_OK;
}

UF_STATUS ACTUATORS_setLights(uint8_t state) {
  if (actConf.actuators_enabled != 1 || actConf.actuators_lights_enabled != 1)
    return UF_DISABLED;

  HAL_GPIO_WritePin(ActInd_GPIO_Port, ActInd_Pin,
                    state == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
  return UF_OK;
}

UF_STATUS ACTUATORS_trigger(ActuatorsValues values) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;

  const ActuatorPin pins[] = {ACT_PIN_A1, ACT_PIN_A2, ACT_PIN_A3, ACT_PIN_A4,
                              ACT_PIN_A5, ACT_PIN_A6, ACT_PIN_A7, ACT_PIN_A8};
  uint16_t actValues[] = {values.act1, values.act2, values.act3, values.act4,
                          values.act5, values.act6, values.act7, values.act8};

  for (int i = 0; i < 8; i++) {
    if (actValues[i] == ActOn) {
      if (ACTUATORS_setPin(pins[i]) != UF_OK) return UF_ERROR;
    } else if (actValues[i] == ActOff) {
      if (ACTUATORS_resetPin(pins[i]) != UF_OK) return UF_ERROR;
    }
  }
  if (ACTUATORS_Update() != UF_OK) return UF_ERROR;

  return UF_OK;
}

UF_STATUS ACT_DeInit(void) {
  actuatorsData = 0;
  SHIFTREG_74HC595D_update(actuatorsData);
  ACTUATORS_setLights(0);
  actConf.actuators_enabled = 0;
  actConf.actuators_lights_enabled = 0;
}

void ACTUATORS_Test(void) {
  ActuatorsConfiguration newconf;
  newconf.actuators_enabled = 1;
  newconf.actuators_lights_enabled = 1;
  ACT_Init(&newconf);

  ACTUATORS_setLights(1);
  for (ActuatorPin pin = ACT_PIN_A1; pin <= ACT_PIN_A8; pin++) {
    ACTUATORS_setPin(pin);
    ACTUATORS_Update();
    HAL_Delay(50);
    ACTUATORS_resetPin(pin);
    ACTUATORS_Update();
    HAL_Delay(50);
  }
  ACTUATORS_setLights(0);
  ACT_DeInit();
}