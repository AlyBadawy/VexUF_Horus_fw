/*
 * vexuf_error.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_error.h"

extern IWDG_HandleTypeDef hiwdg;

extern VexufStatus vexufStatus;
extern IndConfiguration indConfig;
extern OutputConfiguration outputConfig;

void ERROR_ConfigLoop(void);
void ERROR_SdCardLoop(uint16_t delay);

void ERROR_ConfigLoop() {
  while (1) {
    HAL_IWDG_Refresh(&hiwdg);
    IND_setLevel(IndError, IndON);
    HAL_Delay(300);
    IND_setLevel(IndWarn, IndON);
    HAL_Delay(300);
  }
}
void ERROR_SdCardLoop(uint16_t delay) {
  printf("SD card error detected\n");
  SDCARD_STATUS status = SDCARD_ERROR;
  IND_buzzOnError();
  TIMERS_Stop();
  PWM_deinit();
  while (status != SDCARD_OK) {
    HAL_IWDG_Refresh(&hiwdg);
    while (status != SDCARD_OK &&
           HAL_GPIO_ReadPin(SDIO_DET_GPIO_Port, SDIO_DET_Pin) ==
               GPIO_PIN_RESET) {
      printf("SD card error detected while inserted\n");
      if (SDCard_MountFS() == SDCARD_OK) {
        printf("SD card mounted successfully\n");
        status = SDCARD_OK;
      }
    }
    IND_setLevel(IndSdio, IndON);
    IND_setLevel(IndError, IndOFF);
    HAL_Delay(delay);
    IND_setLevel(IndError, IndON);
    IND_setLevel(IndSdio, IndOFF);
    HAL_Delay(delay);
    if (!vexufStatus.sdCardEjected &&
        HAL_GPIO_ReadPin(SDIO_DET_GPIO_Port, SDIO_DET_Pin) == GPIO_PIN_SET) {
      vexufStatus.sdCardEjected = 1;
    }
    if (vexufStatus.sdCardEjected &&
        HAL_GPIO_ReadPin(SDIO_DET_GPIO_Port, SDIO_DET_Pin) == GPIO_PIN_RESET) {
      status = SDCARD_OK;
    }
  }
  printf("SD card error resolved\n");
  IND_setLevel(IndError, IndOFF);
  IND_setLevel(IndSdio, IndOFF);
  IND_setLevel(IndBuzzer, IndOFF);
  vexufStatus.sdCardEjected = 0;
  vexufStatus.sdCardError = 0;
  vexufStatus.sdCardFull = 0;
  vexufStatus.sdCardMounted = 0;
  TIMERS_Start();
  PWM_init();
}

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

  ERROR_ConfigLoop();
}

void ERROR_handleSdError(void) {
  if (vexufStatus.sdCardError == 0 && vexufStatus.sdCardEjected == 0 &&
      vexufStatus.sdCardFull == 0) {
    IND_setLevel(IndSdio, IndOFF);
    return;
  }

  // Handle SD card ejection
  if (vexufStatus.sdCardEjected == 1) {
    if (outputConfig.haltOnSdCardErrors == 1) {
      ERROR_SdCardLoop(300);
      return;
    }
    IND_setLevel(IndSdio, IndFAST);
    return;
  }

  // Handle general SD card error
  if (vexufStatus.sdCardError == 1) {
    if (outputConfig.haltOnSdCardErrors == 1) {
      ERROR_SdCardLoop(300);
      return;
    }
    IND_setLevel(IndSdio, IndFAST);
    return;
  }

  // Handle SD card full error
  if (vexufStatus.sdCardFull == 1) {
    if (outputConfig.haltOnSdCardErrors == 1) {
      ERROR_SdCardLoop(1000);
    }
    IND_setLevel(IndSdio, IndSLOW);
    return;
  }
  // Fallback in case of unexpected behavior
  return;
}
