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
/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/
#define NUMBER_OF_Actuators 8

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
ActuatorsConfiguration actConf = {0};
ActuatorsValues actValues;  // default values for the actuators
static uint8_t actuatorsData = 0;

/* Prototypes ----------------------------------------------------------------*/

/* Code
   ----------------------------------------------------------------------*/

void ACT_Init(ActuatorsConfiguration* newActConf) {
  actConf.actuators_enabled = newActConf->actuators_enabled;
  actConf.actuators_lights_enabled = newActConf->actuators_lights_enabled;
  // TODO: update the actuators values to the default values
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
  ActuatorsConfiguration newConf;
  newConf.actuators_enabled = 1;
  newConf.actuators_lights_enabled = 1;
  ACT_Init(&newConf);

  ACTUATORS_setLights(1);
  for (ActuatorPin pin = ACT_PIN_A1; pin <= ACT_PIN_A8; pin++) {
    ACTUATORS_setPin(pin, ActOn);
    ACTUATORS_Update();
    HAL_Delay(50);
    ACTUATORS_setPin(pin, ActOff);
    ACTUATORS_Update();
    HAL_Delay(50);
  }
  ACTUATORS_setLights(0);
  ACT_DeInit();
}
