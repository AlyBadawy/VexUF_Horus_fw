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
#include "vexuf_error.h"
#include "vexuf_helpers.h"
#include "vexuf_i2c_checker.h"
#include "vexuf_indicators.h"
#include "vexuf_pwm.h"

// TODO: Remove before release
extern UART_HandleTypeDef huart1;
int _write(int file, char *ptr, int len) {
  UNUSED(file);
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 200);
  return len;
}

int main(void) {
  HAL_Init();
  SystemClock_Config();

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
  // MX_USB_DEVICE_Init(); // TODO: Enable before release

  VexUF_GenerateSerialNumber();
  PWM_init();

  ACTUATORS_Test();  // TODO: remove before release
  I2C_ScanTest();    // TODO: remove before release

  // MX_IWDG_Init(); // TODO: Enable before release
  HAL_Delay(500);
  IND_BuzzOnStartUp();

  while (1);
}