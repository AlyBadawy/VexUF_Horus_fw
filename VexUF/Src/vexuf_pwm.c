/*
 * vexuf_pwm.c
 *
 *  Created on: Aug 12, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_pwm.h"

extern TIM_HandleTypeDef htim10;  // Servo 2 Timer
extern TIM_HandleTypeDef htim11;  // Servo 1 Timer

UF_Status PWM_Start(PwmChannel channel);
UF_Status PWM_Stop(PwmChannel channel);

PwmConfiguration pwmConfig;

UF_Status PWM_init(void) {
  if ((PWM_Start(PwmChannel1) != UF_ERROR) &&
      PWM_Start(PwmChannel2) != UF_ERROR) {
    return UF_OK;
  }
  return UF_ERROR;
}

UF_Status PWM_enable(PwmChannel channel) {
  switch (channel) {
    case PwmChannel1:
      pwmConfig.pwm1Enabled = 1;
      break;
    case PwmChannel2:
      pwmConfig.pwm2Enabled = 1;
      break;
    default:
      break;
  }
  return PWM_init();
}

UF_Status PWM_Start(PwmChannel channel) {
  switch (channel) {
    case PwmChannel1:
      if (pwmConfig.pwm1Enabled != 1) return UF_DISABLED;
      return HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1) == HAL_OK ? UF_OK
                                                                 : UF_ERROR;
    case PwmChannel2:
      if (pwmConfig.pwm2Enabled != 1) return UF_DISABLED;
      return HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1) == HAL_OK ? UF_OK
                                                                 : UF_ERROR;
    default:
      UF_ERROR;
  }
}

UF_Status PWM_setDutyCycle(PwmChannel channel, uint16_t dutyCycle) {
  if (dutyCycle == 0xFFFF) return UF_OK;  // 0xFFFF means leave unchanged
  if (dutyCycle > 0x0FFF) return UF_ERROR;

  switch (channel) {
    case PwmChannel1:
      if (pwmConfig.pwm1Enabled != 1) return UF_DISABLED;
      __HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, dutyCycle);
      return UF_OK;
    case PwmChannel2:
      if (pwmConfig.pwm2Enabled != 1) return UF_DISABLED;
      __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, dutyCycle);
      return UF_OK;
    default:
      return UF_ERROR;
  }
}

UF_Status PWM_Stop(PwmChannel channel) {
  switch (channel) {
    case PwmChannel1:
      if (PWM_setDutyCycle(TIM_CHANNEL_1, 0) != UF_OK) return UF_ERROR;
      if (HAL_TIM_PWM_Stop(&htim11, TIM_CHANNEL_1) != HAL_OK) return UF_ERROR;
      return UF_OK;
    case PwmChannel2:
      if (PWM_setDutyCycle(TIM_CHANNEL_2, 0) != UF_OK) return UF_ERROR;
      if (HAL_TIM_PWM_Stop(&htim10, TIM_CHANNEL_1) != HAL_OK) return UF_ERROR;
      return UF_OK;
    default:
      return UF_ERROR;
  }
}

UF_Status PWM_disable(PwmChannel channel) {
  switch (channel) {
    case PwmChannel1:
      pwmConfig.pwm1Enabled = 0;
      return PWM_Stop(PwmChannel1);
    case PwmChannel2:
      pwmConfig.pwm2Enabled = 0;
      return PWM_Stop(PwmChannel2);
    default:
      return UF_ERROR;
  }
}

UF_Status PWM_deinit(void) {
  if ((PWM_disable(PwmChannel1) != UF_ERROR) &&
      PWM_disable(PwmChannel2) != UF_ERROR) {
    return UF_OK;
  }
  return UF_ERROR;
}