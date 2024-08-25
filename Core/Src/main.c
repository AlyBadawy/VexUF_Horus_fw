#include "main.h"

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
#include "vexuf_actuators.h"
#include "vexuf_adc_avs.h"
#include "vexuf_error.h"
#include "vexuf_helpers.h"
#include "vexuf_i2c_checker.h"
#include "vexuf_indicators.h"
#include "vexuf_pwm.h"
#include "vexuf_timers.h"

// TODO: Remove before release
extern UART_HandleTypeDef huart1;
int _write(int file, char *ptr, int len) {
  UNUSED(file);
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 200);
  return len;
}

extern IWDG_HandleTypeDef hiwdg;
extern VexufStatus vexufStatus;
extern IndConfiguration indConfig;

int main(void) {
  /*
    Enable indicators for the duration of the startup routine.
    This makes sure that the initial WARN is turned on, and you
    can show the NO-CONF error sequence.
  */
  indConfig.globalIndicatorEnabled = 1;
  indConfig.statusIndicatorsEnabled = 1;

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

  VexUF_GenerateSerialNumber();
  PWM_init();
  TIMERS_Start();

  // TODO: Start listening to TTL and TNC interrupts.

  HAL_Delay(500);
  IND_BuzzOnStartUp();

  IND_setLevel(IndWarn, IndON);
  // TODO: Apply configurations
  // TODO: Init the screen with the number of rows as configured.

  // TODO: remove the following tests before release
  ADC_Test();
  ACTUATORS_Test();
  I2C_ScanTest();

  // TODO: Enable before release
  // MX_IWDG_Init();

  while (1) {
    if (vexufStatus.timer_10hz_ticked == 1) {
      IND_toggleIndWithLevelOption(IndFAST);
      vexufStatus.timer_10hz_ticked = 0;
    }

    if (vexufStatus.timer_1hz_ticked == 1) {
      IND_toggleIndWithLevelOption(IndSLOW);
      vexufStatus.timer_1hz_ticked = 0;
    }

    // TODO: Enable before release
    // HAL_IWDG_Refresh(&hiwdg);
  }
}