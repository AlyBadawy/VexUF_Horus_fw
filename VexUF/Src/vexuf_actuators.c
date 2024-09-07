/**
 ******************************************************************************
 * @file          : vexuf_actuators.c
 * @brief        : VexUF Actuators Implementation
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
#include "vexuf_actuators.h"

#include "74hc595d.h"  // 74HC595D Shift Register
#include "vexuf_config.h"
/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/
#define NUMBER_OF_Actuators 8

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
ActuatorsConfiguration actConf = {0};
ActuatorsValues actDefaultValues;  // default values for the actuators
static uint8_t actuatorsData = 0;

/* Prototypes ----------------------------------------------------------------*/
uint8_t convertToShiftRegisterValue(ActuatorsValues actValues);

/* Code ----------------------------------------------------------------------*/
UF_STATUS ACT_Init(void) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;

  actuatorsData = convertToShiftRegisterValue(actDefaultValues);

  return ACTUATORS_Update();
}

UF_STATUS ACTUATORS_setPin(ActuatorPin pin, ActLevel level) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;
  switch (level) {
    case ActOn:
      actuatorsData |= (1 << (pin));
      return UF_OK;
    case ActOff:
      actuatorsData &= ~(1 << (pin));
      return UF_OK;
    case ActUnchanged:
      return UF_OK;
    default:
      return UF_ERROR;
  }
}

UF_STATUS ACTUATORS_Update(void) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;
  ACTUATORS_setLights(actConf.actuators_lights_enabled);
  SHIFTREG_74HC595D_update(actuatorsData);
  return UF_OK;
}

UF_STATUS ACTUATORS_setLights(uint8_t state) {
  if (actConf.actuators_enabled != 1 || actConf.actuators_lights_enabled != 1)
    return UF_DISABLED;

  HAL_GPIO_WritePin(ActInd_GPIO_Port, ActInd_Pin,
                    state ? GPIO_PIN_SET : GPIO_PIN_RESET);
  return UF_OK;
}

UF_STATUS ACTUATORS_trigger(ActuatorsValues values) {
  if (actConf.actuators_enabled != 1) return UF_DISABLED;

  for (ActuatorPin pin = ACT_PIN_A1; pin <= ACT_PIN_A8; pin++) {
    uint8_t value = ((uint16_t*)&values)[pin];
    if (ACTUATORS_setPin(pin, value) != UF_OK) return UF_ERROR;
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
  return UF_OK;
}

void ACTUATORS_Test(void) {
  ActuatorsConfiguration newConf = {0};
  newConf.actuators_enabled = 1;
  newConf.actuators_lights_enabled = 1;
  ActuatorsValues newValues = {0};
  newValues = (ActuatorsValues){.act1 = ActOn,
                                .act2 = ActOn,
                                .act3 = ActOn,
                                .act4 = ActOff,
                                .act5 = ActOn,
                                .act6 = ActOff,
                                .act7 = ActOn,
                                .act8 = ActOff};
  CONFIG_setActuators(&newConf, &newValues);
  ACT_Init();

  HAL_Delay(1000);

  for (ActuatorPin pin = ACT_PIN_A1; pin <= ACT_PIN_A8; pin++) {
    ACTUATORS_setPin(pin, ActOn);
    ACTUATORS_Update();
    HAL_Delay(50);
    ACTUATORS_setPin(pin, ActOff);
    ACTUATORS_Update();
    HAL_Delay(50);
  }
  CONFIG_setActuators(0, 0);
  ACT_DeInit();
}

/* Private Methods -----------------------------------------------------------*/
uint8_t convertToShiftRegisterValue(ActuatorsValues values) {
  uint8_t data = 0;

  // Check each actuator and set the corresponding bit if it's ActOn
  if (values.act1 == ActOn) data |= (1 << 0);  // Set bit 0
  if (values.act2 == ActOn) data |= (1 << 1);  // Set bit 1
  if (values.act3 == ActOn) data |= (1 << 2);  // Set bit 2
  if (values.act4 == ActOn) data |= (1 << 3);  // Set bit 3
  if (values.act5 == ActOn) data |= (1 << 4);  // Set bit 4
  if (values.act6 == ActOn) data |= (1 << 5);  // Set bit 5
  if (values.act7 == ActOn) data |= (1 << 6);  // Set bit 6
  if (values.act8 == ActOn) data |= (1 << 7);  // Set bit 7

  return data;
}