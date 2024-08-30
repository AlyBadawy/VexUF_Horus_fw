/**
 ******************************************************************************
 * @file          : vexuf_pwm.c
 * @brief        : VexUF PWM Implementation
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
#include "vexuf_pwm.h"

/* TypeDef -------------------------------------------------------------------*/
static TIM_HandleTypeDef *pwm1Timer;
static TIM_HandleTypeDef *pwm2Timer;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
PwmConfiguration pwmConfig;

/* Prototypes ----------------------------------------------------------------*/
UF_STATUS PWM_Start(PwmChannel channel);
UF_STATUS PWM_Stop(PwmChannel channel);

/* Code ----------------------------------------------------------------------*/

/* Private Methods -----------------------------------------------------------*/

UF_STATUS PWM_init(TIM_HandleTypeDef *pwm1, TIM_HandleTypeDef *pwm2) {
  pwm1Timer = pwm1;
  pwm2Timer = pwm2;

  if (pwmConfig.pwm1Enabled == 1 && pwmConfig.pwm1Value <= 1000) {
    if (PWM_Start(PwmChannel1) == UF_ERROR) return UF_ERROR;
    if (PWM_setDutyCycle(PwmChannel1, pwmConfig.pwm1Value) == UF_ERROR)
      return UF_ERROR;
  }
  if (pwmConfig.pwm2Enabled == 1 && pwmConfig.pwm2Value <= 1000) {
    if (PWM_Start(PwmChannel2) == UF_ERROR) return UF_ERROR;
    if (PWM_setDutyCycle(PwmChannel2, pwmConfig.pwm1Value) == UF_ERROR)
      return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS PWM_enable(PwmChannel channel) {
  switch (channel) {
    case PwmChannel1:
      pwmConfig.pwm1Enabled = 1;
      break;
    case PwmChannel2:
      pwmConfig.pwm2Enabled = 1;
      break;
    default:
      return UF_ERROR;
  }
  return PWM_init(pwm1Timer, pwm2Timer);
}

UF_STATUS PWM_Start(PwmChannel channel) {
  TIM_HandleTypeDef *htim;
  uint8_t enabled;
  switch (channel) {
    case PwmChannel1:
      htim = pwm1Timer;
      enabled = pwmConfig.pwm1Enabled;
      break;
    case PwmChannel2:
      htim = pwm2Timer;
      enabled = pwmConfig.pwm2Enabled;
      break;
    default:
      return UF_ERROR;
  }

  if (enabled != 1) return UF_DISABLED;
  if (HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1) != HAL_OK) return UF_ERROR;
  return UF_OK;
}

UF_STATUS PWM_setDutyCycle(PwmChannel channel, uint16_t dutyCycle) {
  if (dutyCycle == 0xFFFF) return UF_OK;  // 0xFFFF means leave unchanged
  if (dutyCycle > 0x0FFF) return UF_ERROR;

  TIM_HandleTypeDef *htim;
  uint8_t enabled;

  switch (channel) {
    case PwmChannel1:
      htim = pwm1Timer;
      enabled = pwmConfig.pwm1Enabled;
      break;
    case PwmChannel2:
      htim = pwm1Timer;
      enabled = pwmConfig.pwm2Enabled;
      break;
    default:
      return UF_ERROR;
  }

  if (enabled != 1) return UF_DISABLED;
  __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, dutyCycle);
  return UF_OK;
}

UF_STATUS PWM_Stop(PwmChannel channel) {
  TIM_HandleTypeDef *htim;
  uint32_t tim_channel;

  switch (channel) {
    case PwmChannel1:
      htim = pwm1Timer;
      tim_channel = TIM_CHANNEL_1;
      break;
    case PwmChannel2:
      htim = pwm2Timer;
      tim_channel = TIM_CHANNEL_2;
      break;
    default:
      return UF_ERROR;
  }

  if (PWM_setDutyCycle(tim_channel, 0) == UF_ERROR) return UF_ERROR;
  return HAL_TIM_PWM_Stop(htim, tim_channel) == HAL_OK ? UF_OK : UF_ERROR;
}

UF_STATUS PWM_disable(PwmChannel channel) {
  uint8_t *enabled;
  switch (channel) {
    case PwmChannel1:
      enabled = &pwmConfig.pwm1Enabled;
      break;
    case PwmChannel2:
      enabled = &pwmConfig.pwm2Enabled;
      break;
    default:
      return UF_ERROR;
  }
  *enabled = 0;
  return PWM_Stop(channel) == UF_ERROR ? UF_ERROR : UF_OK;
}

UF_STATUS PWM_deinit(void) {
  return (PWM_disable(PwmChannel1) == UF_ERROR ||
          PWM_disable(PwmChannel2) == UF_ERROR)
             ? UF_ERROR
             : UF_OK;
}