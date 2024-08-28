#include "main.h"

#include "93c86.h"
#include "adc.h"
#include "clock.h"
#include "dma.h"
#include "fatfs.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "vexuf.h"
#include "vexuf_actuators.h"
#include "vexuf_adc_avs.h"
#include "vexuf_config.h"
#include "vexuf_error.h"
#include "vexuf_i2c_checker.h"
#include "vexuf_indicators.h"
#include "vexuf_pwm.h"
#include "vexuf_sd_card.h"
#include "vexuf_timers.h"

extern IWDG_HandleTypeDef hiwdg;
extern VexufStatus vexufStatus;
extern IndConfiguration indConfig;
extern OutputConfiguration outputConfig;

int main(void) {
  /*
    Enable indicators for the duration of the startup routine.
    This makes sure that the initial WARN is turned on, and you
    can show the NO-CONF error sequence.
    TODO: Review the need for this before release.
  */
  indConfig.globalIndicatorEnabled = 1;
  indConfig.sdCardIndicatorEnabled = 1;
  outputConfig.haltOnSdCardErrors = 1;

  HAL_Init();

  SystemClock_Config();

  IND_setLevel(IndWarn, IndON);

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_FATFS_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();

  // TODO: Enable before release
  // MX_USB_DEVICE_Init();

  HAL_GPIO_WritePin(WarnInd_GPIO_Port, WarnInd_Pin, GPIO_PIN_SET);
  EEPROM_93C86_init(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);

  VexUF_GenerateSerialNumber();
  CONFIG_SetIsConfigured();
  if (CONFIG_IsConfigured() != UF_OK) ERROR_handleNoConfig();

  // TODO: Apply configurations
  if (CONFIG_WriteSerialNumber() != UF_OK) Error_Handler();

  PWM_init();
  TIMERS_Start();

  // TODO: Start listening to TTL and TNC interrupts.

  HAL_Delay(500);
  IND_BuzzOnStartUp();

  // TODO: Init the screen with the number of rows as configured.

  // TODO: remove the following tests before release
  ADC_Test();
  ACTUATORS_Test();
  I2C_ScanTest();
  // END OF TESTS

  HAL_Delay(500);
  printf("VexUF Horus is ready.\n");
  HAL_GPIO_WritePin(WarnInd_GPIO_Port, WarnInd_Pin, GPIO_PIN_RESET);

  MX_IWDG_Init();

  while (1) {
    SDCard_checkCard();
    ERROR_handleSdError();

    // Run this routine every 100ms
    if (vexufStatus.timer_10hz_ticked == 1) {
      IND_toggleIndWithLevelOption(IndFAST);
      vexufStatus.timer_10hz_ticked = 0;
      if (vexufStatus.sdCardError == 1 || vexufStatus.sdCardEjected == 1) {
      }
    }

    // Run this routine every 1s
    if (vexufStatus.timer_1hz_ticked == 1) {
      IND_toggleIndWithLevelOption(IndSLOW);
      // TODO: Run ADC Scan

      // TODO: toggle SDCARD indicator if full and no halt on error
      vexufStatus.timer_1hz_ticked = 0;
    }

    // Run this routine every 10s
    if (vexufStatus.timer_0d1hz_ticked == 1) {
      TRIGS_runAll();
      vexufStatus.timer_0d1hz_ticked = 0;
    }

    if (vexufStatus.ttlBuffered == 1) {
      // if (COMMANDS_handleCommand(TtlUart) == UF_ERROR) {
      //   // todo: handle error
      // }
      vexufStatus.ttlBuffered = 0;
    }
    if (vexufStatus.tncBuffered == 1) {
      // if (COMMANDS_handleCommand(TncUart) == UF_ERROR) {
      //   // todo: handle error
      // }
      vexufStatus.ttlBuffered = 0;
    }

    // Run this routine every iteration.
    HAL_IWDG_Refresh(&hiwdg);
  }
}