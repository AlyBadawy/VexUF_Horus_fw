/**
 ******************************************************************************
 * @file          : vexuf_error.c
 * @brief        : Error handling functions for VexUF
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
#include "vexuf_error.h"

#include "iwdg.h"
#include "vexuf.h"
#include "vexuf_indicators.h"
#include "vexuf_outputs.h"
#include "vexuf_pwm.h"
#include "vexuf_sdcard.h"
#include "vexuf_timers.h"

/* TypeDef -------------------------------------------------------------------*/
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim10;  // Servo 2 Timer
extern TIM_HandleTypeDef htim11;  // Servo 1 Timer
/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern VexufStatus vexufStatus;
extern IndConfiguration indConf;
extern OutputConfiguration outputConfig;

/* Variables -----------------------------------------------------------------*/

/* Prototypes ----------------------------------------------------------------*/
void ERROR_ConfigLoop(void);
void ERROR_SdCardLoop(uint16_t delay);

/* Code ----------------------------------------------------------------------*/
void Error_Handler(void) {
  __disable_irq();
  while (1);
}

void ERROR_handleNoConfig(void) {
  if (vexufStatus.isConfigured == 1) return;

  indConf.globalIndicatorEnabled = 1;
  indConf.statusIndicatorsEnabled = 1;

  TIMERS_Stop();
  PWM_deinit();
  IND_buzzOnError();

  ERROR_ConfigLoop();
}

void ERROR_handleSdError(void) {
  if (!vexufStatus.sdCardError && !vexufStatus.sdCardEjected &&
      !vexufStatus.sdCardFull) {
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

/* Private Methods -----------------------------------------------------------*/

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
  UF_STATUS status = UF_ERROR;
  IND_buzzOnError();
  TIMERS_Stop();
  PWM_deinit();

  while (status != UF_OK) {
    HAL_IWDG_Refresh(&hiwdg);
    while (HAL_GPIO_ReadPin(SDIO_DET_GPIO_Port, SDIO_DET_Pin) ==
           GPIO_PIN_RESET) {
      printf("SD card error detected while inserted\n");
      if (SDCard_MountFS() == UF_OK) {
        printf("SD card mounted successfully\n");
        status = UF_OK;
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
      status = UF_OK;
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
  PWM_init(&htim11, &htim10);
}
