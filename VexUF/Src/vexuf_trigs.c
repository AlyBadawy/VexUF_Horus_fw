#include "vexuf_trigs.h"

extern PwmConfiguration pwmConfig;
extern float cpuTempC;
extern uint32_t AVsRawValues[3];
extern float AVsVoltages[3];

TriggerConfiguration triggers[NUMBER_OF_TRIGGERS];

TRIGS_Status TRIGS_checkTriggerRule(TrigComparison comparison,
                                    TrigCompareTest test, uint32_t fromValue,
                                    uint32_t toValue);
TRIGS_Status TRIGS_compare(uint32_t ref, TrigCompareTest test,
                           uint32_t fromValue, uint32_t toValue);

TRIGS_Status TRIGS_runAll(void) {
  for (int i = 0; i < NUMBER_OF_TRIGGERS; i++) {
    TRIGS_Status status = TRIGS_runTrigger(i);
    if (status == TRIGS_ERROR) return TRIGS_ERROR;
    // TODO: Check the status of TRIGS_runTrigger, and log/output accordingly.
  }

  if (ACTUATORS_Update() != ACT_OK) {
    return TRIGS_ERROR;
  }

  if (PWM_setDutyCycle(PwmChannel1, pwmConfig.pwm1Value) != UF_OK)
    return TRIGS_ERROR;
  if (PWM_setDutyCycle(PwmChannel2, pwmConfig.pwm2Value) != UF_OK)
    return TRIGS_ERROR;

  return TRIGS_OK;
}

TRIGS_Status TRIGS_runTrigger(uint8_t idx) {
  TrigComparison comparison;
  TrigCompareTest test;
  uint16_t fromValue, toValue;

  comparison = triggers[idx].status;
  test = triggers[idx].test;
  fromValue = triggers[idx].fromValue;
  toValue = triggers[idx].toValue;

  TRIGS_Status status =
      TRIGS_checkTriggerRule(comparison, test, fromValue, toValue);

  if (status != TRIGS_TRIGGERED) return status;

  // TODO: handle trigger outputs!
  ACTUATORS_trigger(triggers[idx].actuators);

  if (triggers[idx].pwm1 != 0xFFFF)
    pwmConfig.pwm1Value = triggers[idx].pwm1 & 0x0FFF;
  if (triggers[idx].pwm2 != 0xFFFF)
    pwmConfig.pwm2Value = triggers[idx].pwm2 & 0x0FFF;

  return TRIGS_TRIGGERED;
}

TRIGS_Status TRIGS_checkTriggerRule(TrigComparison comparison,
                                    TrigCompareTest test, uint32_t fromValue,
                                    uint32_t toValue) {
  switch (comparison) {
    case trigDisabled:
      return TRIGS_DISABLED;
    case Av1ToAv2:
      return TRIGS_compare(AVsRawValues[1], test, AVsRawValues[2], 0);
    case Av2ToAv3:
      return TRIGS_compare(AVsRawValues[2], test, AVsRawValues[3], 0);
    case Av1ToAv3:
      return TRIGS_compare(AVsRawValues[1], test, AVsRawValues[3], 0);
    case IntTempToExternal:
      return TRIGS_DISABLED;  // TODO: IMPLEMENT TEMP.
    case Av1ToTest:
      return TRIGS_compare(AVsRawValues[1], test, fromValue, toValue);
    case Av2ToTest:
      return TRIGS_compare(AVsRawValues[2], test, fromValue, toValue);
    case Av3ToTest:
      return TRIGS_compare(AVsRawValues[3], test, fromValue, toValue);
    case IntTempToTest:
      return TRIGS_DISABLED;  // TODO: IMPLEMENT TEMP.
    case ExtTempToTest:
      return TRIGS_DISABLED;  // TODO: IMPLEMENT TEMP.
    default:
      return TRIGS_ERROR;
  }
}

TRIGS_Status TRIGS_compare(uint32_t ref, TrigCompareTest test,
                           uint32_t fromValue, uint32_t toValue) {
  switch (test) {
    case lessThan:
      return ref < fromValue ? TRIGS_OK : TRIGS_NOT_TRIGGERED;
    case lessThanOrEqual:
      return ref <= fromValue ? TRIGS_OK : TRIGS_NOT_TRIGGERED;
    case Equal:
      return ref == fromValue ? TRIGS_OK : TRIGS_NOT_TRIGGERED;
    case GreaterThanOrEqual:
      return ref >= fromValue ? TRIGS_OK : TRIGS_NOT_TRIGGERED;
    case GreaterThan:
      return ref > fromValue ? TRIGS_OK : TRIGS_NOT_TRIGGERED;
    case NotEqual:
      return ref != fromValue ? TRIGS_OK : TRIGS_NOT_TRIGGERED;
    case inRange:
      return ref >= fromValue && ref <= toValue ? TRIGS_OK
                                                : TRIGS_NOT_TRIGGERED;
    case outtaRange:
      return (ref < fromValue || ref > toValue) ? TRIGS_OK
                                                : TRIGS_NOT_TRIGGERED;
    default:
      return TRIGS_ERROR;
  }
}