/**
 ******************************************************************************
 * @file          : vexuf_trigs.c
 * @brief        : VexUF Triggers Implementation
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
#include "vexuf_trigs.h"

/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern PwmConfiguration pwmConfig;
extern float cpuTempC;
extern uint32_t AVsRawValues[3];
extern float AVsVoltages[3];

/* Variables -----------------------------------------------------------------*/
TriggerConfiguration triggers[NUMBER_OF_TRIGGERS];

/* Prototypes ----------------------------------------------------------------*/
UF_STATUS TRIGS_checkTriggerRule(TrigComparison comparison,
                                 TrigCompareTest test, uint32_t fromValue,
                                 uint32_t toValue);
UF_STATUS TRIGS_compare(uint32_t ref, TrigCompareTest test, uint32_t fromValue,
                        uint32_t toValue);

/* Code ----------------------------------------------------------------------*/
UF_STATUS TRIGS_Init(void) {
  for (uint8_t i = 0; i < NUMBER_OF_TRIGGERS; i++) {
    if (CONFIG_getTrigConf(&triggers[i], i) != UF_OK) return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS TRIGS_runAll(void) {
  for (int i = 0; i < NUMBER_OF_TRIGGERS; i++) {
    UF_STATUS status = TRIGS_runTrigger(i);
    if (status == UF_ERROR) return UF_ERROR;
    // TODO: Check the status of TRIGS_runTrigger, and log/output accordingly.
  }

  if (ACTUATORS_Update() != UF_OK) {
    return UF_ERROR;
  }

  // TODO: Check the status of PWM calls.
  // If any of them fails, return UF_ERROR.
  PWM_setDutyCycle(PwmChannel1, pwmConfig.pwm1Value);
  PWM_setDutyCycle(PwmChannel2, pwmConfig.pwm2Value);

  return UF_OK;
}

UF_STATUS TRIGS_runTrigger(uint8_t idx) {
  TrigComparison comparison;
  TrigCompareTest test;
  uint16_t fromValue, toValue;

  comparison = triggers[idx].status;
  test = triggers[idx].test;
  fromValue = triggers[idx].fromValue;
  toValue = triggers[idx].toValue;

  UF_STATUS status =
      TRIGS_checkTriggerRule(comparison, test, fromValue, toValue);

  if (status != UF_TRIGGERED) return status;

  // TODO: handle trigger outputs!
  ACTUATORS_trigger(triggers[idx].actuators);

  if (triggers[idx].pwm1 != 0xFFFF)
    pwmConfig.pwm1Value = triggers[idx].pwm1 & 0x0FFF;
  if (triggers[idx].pwm2 != 0xFFFF)
    pwmConfig.pwm2Value = triggers[idx].pwm2 & 0x0FFF;

  return UF_TRIGGERED;
}

/* Private Methods -----------------------------------------------------------*/
UF_STATUS TRIGS_checkTriggerRule(TrigComparison comparison,
                                 TrigCompareTest test, uint32_t fromValue,
                                 uint32_t toValue) {
  switch (comparison) {
    case trigDisabled:
      return UF_DISABLED;
    case Av1ToAv2:
      return TRIGS_compare(AVsRawValues[1], test, AVsRawValues[2], 0);
    case Av2ToAv3:
      return TRIGS_compare(AVsRawValues[2], test, AVsRawValues[3], 0);
    case Av1ToAv3:
      return TRIGS_compare(AVsRawValues[1], test, AVsRawValues[3], 0);
    case IntTempToExternal:
      return UF_DISABLED;  // TODO: IMPLEMENT TEMP.
    case Av1ToTest:
      return TRIGS_compare(AVsRawValues[1], test, fromValue, toValue);
    case Av2ToTest:
      return TRIGS_compare(AVsRawValues[2], test, fromValue, toValue);
    case Av3ToTest:
      return TRIGS_compare(AVsRawValues[3], test, fromValue, toValue);
    case IntTempToTest:
      return UF_DISABLED;  // TODO: IMPLEMENT TEMP.
    case ExtTempToTest:
      return UF_DISABLED;  // TODO: IMPLEMENT TEMP.
    default:
      return UF_ERROR;
  }
}

UF_STATUS TRIGS_compare(uint32_t ref, TrigCompareTest test, uint32_t fromValue,
                        uint32_t toValue) {
  switch (test) {
    case lessThan:
      // TODO: Use different return values when true than UF_OK
      return ref < fromValue ? UF_OK : UF_NOT_TRIGGERED;
    case lessThanOrEqual:
      return ref <= fromValue ? UF_OK : UF_NOT_TRIGGERED;
    case Equal:
      return ref == fromValue ? UF_OK : UF_NOT_TRIGGERED;
    case GreaterThanOrEqual:
      return ref >= fromValue ? UF_OK : UF_NOT_TRIGGERED;
    case GreaterThan:
      return ref > fromValue ? UF_OK : UF_NOT_TRIGGERED;
    case NotEqual:
      return ref != fromValue ? UF_OK : UF_NOT_TRIGGERED;
    case inRange:
      return ref >= fromValue && ref <= toValue ? UF_OK : UF_NOT_TRIGGERED;
    case outtaRange:
      return (ref < fromValue || ref > toValue) ? UF_OK : UF_NOT_TRIGGERED;
    default:
      return UF_ERROR;
  }
}