/*
 * vexuf_error.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_error.h"

#include "iwdg.h"
#include "vexuf_indicators.h"
#include "vexuf_pwm.h"
#include "vexuf_timers.h"

extern IWDG_HandleTypeDef hiwdg;

extern VexufStatus vexufStatus;
extern IndConfiguration indConfig;

void Error_Handler(void) {
  __disable_irq();
  while (1);
}

void ERROR_handleNoConfig(void) {
  if (vexufStatus.isConfigured == 1) return;

  indConfig.globalIndicatorEnabled = 1;
  indConfig.statusIndicatorsEnabled = 1;

  TIMERS_Stop();
  PWM_deinit();
  IND_buzzOnError();

  while (1) {
    HAL_IWDG_Refresh(&hiwdg);

    IND_setLevel(IndError, IndON);
    HAL_Delay(300);
    IND_setLevel(IndWarn, IndON);
    HAL_Delay(300);
  }
}