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

#include "93c86.h"
#include "iwdg.h"
#include "vexuf.h"
#include "vexuf_cli.h"
#include "vexuf_config.h"
#include "vexuf_indicators.h"
#include "vexuf_outputs.h"
#include "vexuf_pwm.h"
#include "vexuf_sdcard.h"
#include "vexuf_serial.h"
#include "vexuf_timers.h"

/* TypeDef -------------------------------------------------------------------*/
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim10;  // Servo 2 Timer
extern TIM_HandleTypeDef htim11;  // Servo 1 Timer

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

/* Defines -------------------------------------------------------------------*/
#define UART_TTL_HANDLER huart1
#define UART_TNC_HANDLER huart6

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern VexufStatus vexufStatus;
extern IndConfiguration indConf;
extern OutputConfiguration outputConf;
extern SerialConfiguration serialConf;

extern unsigned char ttlRxData[SERIAL_BUFFER_SIZE];

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

  uint16_t buffer = 0;
  EEPROM_93C86_Read(EEPROM_CONFIG_FLAG_ADDRESS, &buffer);
  if (buffer != CONFIG_FLAG || buffer == 0) {
    EEPROM_93C86_WriteAll(0x00);
  }

  serialConf.ttl_enabled = 1;
  serialConf.ttl_led_enabled = 1;
  serialConf.ttl_baud = Baud115200;

  SERIAL_init(&UART_TTL_HANDLER, &UART_TNC_HANDLER);
  CLI_init(&UART_TTL_HANDLER, &UART_TNC_HANDLER);

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
    if (outputConf.haltOnSdCardErrors == 1) {
      ERROR_SdCardLoop(300);
      return;
    }
    IND_setLevel(IndSdio, IndFAST);
    return;
  }

  // Handle general SD card error
  if (vexufStatus.sdCardError == 1) {
    if (outputConf.haltOnSdCardErrors == 1) {
      ERROR_SdCardLoop(300);
      return;
    }
    IND_setLevel(IndSdio, IndFAST);
    return;
  }

  // Handle SD card full error
  if (vexufStatus.sdCardFull == 1) {
    if (outputConf.haltOnSdCardErrors == 1) {
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
  while (vexufStatus.isConfigured == 0) {
    HAL_IWDG_Refresh(&hiwdg);
    IND_setLevel(IndError, IndON);
    HAL_Delay(300);
    IND_setLevel(IndWarn, IndON);
    HAL_Delay(300);
    if (vexufStatus.ttlBuffered == 1) {
      CLI_handleCommand(TTL);
      vexufStatus.ttlBuffered = 0;
    }
  }
}

void ERROR_SdCardLoop(uint16_t delay) {
  printf("SD card error detected\n");
  UF_STATUS status = UF_ERROR;
  // IND_buzzOnError();
  TIMERS_Stop();
  PWM_deinit();

  vexufStatus.sdCardMounted = 0;

  while (status != UF_OK) {
    HAL_IWDG_Refresh(&hiwdg);
    while (HAL_GPIO_ReadPin(SDIO_DET_GPIO_Port, SDIO_DET_Pin) ==
               GPIO_PIN_RESET &&
           vexufStatus.sdCardMounted == 0) {
      printf("SD card error detected while inserted\n");
      if (SDCard_MountFS() == UF_OK) {
        printf("SD card mounted successfully\n");
        vexufStatus.sdCardMounted = 1;
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
