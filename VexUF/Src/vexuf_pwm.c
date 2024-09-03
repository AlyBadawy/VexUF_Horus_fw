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

#include <ctype.h>
#include <stdlib.h>

#include "vexuf_config.h"

/* TypeDef -------------------------------------------------------------------*/
static TIM_HandleTypeDef *pwm1Timer;
static TIM_HandleTypeDef *pwm2Timer;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern char *ok;
extern char *no;

/* Variables -----------------------------------------------------------------*/
PwmConfiguration pwmConfig;

/* Prototypes ----------------------------------------------------------------*/
UF_STATUS PWM_Start(PwmChannel channel);
UF_STATUS PWM_Stop(PwmChannel channel);

/* Code ----------------------------------------------------------------------*/
UF_STATUS PWM_init(TIM_HandleTypeDef *pwm1, TIM_HandleTypeDef *pwm2) {
  pwm1Timer = pwm1;
  pwm2Timer = pwm2;

  if (pwmConfig.pwm1Enabled == 1 && pwmConfig.pwm1Value < 1000) {
    if (PWM_Start(PwmChannel1) == UF_ERROR) return UF_ERROR;
    if (PWM_setDutyCycle(PwmChannel1, pwmConfig.pwm1Value) == UF_ERROR)
      return UF_ERROR;
  }

  if (pwmConfig.pwm2Enabled == 1 && pwmConfig.pwm2Value < 1000) {
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
  if (dutyCycle > 0x03E7)
    return UF_ERROR;  // resolution of 0x03E7 is 1000 (0x3E7 is 999)

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

void PWM_handleCli(const char *args, char *responseBuffer) {
  static char *invalidValue =
      "\r\nInvalid PWM value.\r\n Allowed range is: 0 to 999.";
  static char *unknown = "Unknown PWM command. Use 'help PWM' for help.";
  static char *statusEnabled = "PWM%c: Enabled - Default value: %d.%s";
  static char *statusDisabled = "PWM%c: Disabled.%s";

  if (args == NULL || strlen(args) == 0) {
    // Show status for both PWM 1 and PWM 2
    if (pwmConfig.pwm1Enabled) {
      sprintf(responseBuffer, statusEnabled, '1', pwmConfig.pwm1Value, "\r\n");
    } else {
      sprintf(responseBuffer, statusDisabled, '1', "\r\n");
    }
    if (pwmConfig.pwm2Enabled) {
      sprintf(responseBuffer + strlen(responseBuffer), statusEnabled, '2',
              pwmConfig.pwm2Value, ok);
    } else {
      sprintf(responseBuffer + strlen(responseBuffer), statusDisabled, '2',
              "\r\n");
    }
  } else if (strncmp(args, "1", 1) == 0 || strncmp(args, "2", 1) == 0) {
    const char *pwmArgs = args + 1;
    while (isspace((unsigned char)*pwmArgs)) pwmArgs++;
    if (*pwmArgs == '\0') {
      if (args[0] == '1') {
        if (pwmConfig.pwm1Enabled) {
          sprintf(responseBuffer, statusEnabled, '1', pwmConfig.pwm1Value, ok);
        } else {
          sprintf(responseBuffer, statusDisabled, '1', ok);
        }
      } else {
        if (pwmConfig.pwm2Enabled) {
          sprintf(responseBuffer, statusEnabled, '2', pwmConfig.pwm2Value, ok);
        } else {
          sprintf(responseBuffer, statusDisabled, '2', ok);
        }
      }
    } else if (strncmp(pwmArgs, "enable", 6) == 0) {
      PwmConfiguration newConf;
      memcpy(&newConf, &pwmConfig, sizeof(PwmConfiguration));
      if (args[0] == '1') {
        newConf.pwm1Enabled = 1;
      } else {
        newConf.pwm2Enabled = 1;
      }
      CONFIG_setPwmConfigurations(&newConf);
      PWM_init(pwm1Timer, pwm2Timer);
      sprintf(responseBuffer, "PWM%c is set to Enabled%s", args[0], ok);
    } else if (strncmp(pwmArgs, "disable", 7) == 0) {
      PwmConfiguration newConf;
      memcpy(&newConf, &pwmConfig, sizeof(PwmConfiguration));
      if (args[0] == '1') {
        newConf.pwm1Enabled = 0;
      } else {
        newConf.pwm2Enabled = 0;
      }
      CONFIG_setPwmConfigurations(&newConf);
      PWM_Stop(args[0] == '1' ? PwmChannel1 : PwmChannel2);
      sprintf(responseBuffer, "PWM%c is set to Disabled%s", args[0], ok);
    } else if (strncmp(pwmArgs, "value", 5) == 0) {
      if ((pwmConfig.pwm1Enabled == 0 && args[0] == '1') ||
          (pwmConfig.pwm2Enabled == 0 && args[0] == '2')) {
        sprintf(responseBuffer, "Error: PWM%c is disabled.%s", args[0], no);
        return;
      }
      const char *valueArgs = pwmArgs + 5;
      while (isspace((unsigned char)*valueArgs)) valueArgs++;
      if (*valueArgs == '\0') {
        sprintf(responseBuffer,
                "Error: No PWM value provided. Use a value between 0-999.%s",
                no);
      } else {
        int value = atoi(valueArgs);
        if (value < 0 || value > 999) {
          sprintf(responseBuffer, "%s%s", invalidValue, no);
        } else {
          PwmConfiguration newConf;
          memcpy(&newConf, &pwmConfig, sizeof(PwmConfiguration));
          if (args[0] == '1') {
            newConf.pwm1Value = value;
          } else {
            newConf.pwm2Value = value;
          }
          CONFIG_setPwmConfigurations(&newConf);
          PWM_init(pwm1Timer, pwm2Timer);
          sprintf(responseBuffer, "PWM%c default value set to: %d%s", args[0],
                  value, ok);
        }
      }
    } else {
      sprintf(responseBuffer, "%s%s", unknown, no);
    }
  } else {
    sprintf(responseBuffer, "%s%s", unknown, no);
  }
}