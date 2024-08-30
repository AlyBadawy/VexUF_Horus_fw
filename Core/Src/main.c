/**
 ******************************************************************************
 * @file          : main.c
 * @brief         : Main program body
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
#include "main.h"

#include "93c86.h"
#include "adc.h"
#include "aht20.h"
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
#include "vexuf_avs.h"
#include "vexuf_cli.h"
#include "vexuf_config.h"
#include "vexuf_error.h"
#include "vexuf_i2c_checker.h"
#include "vexuf_indicators.h"
#include "vexuf_lcd.h"
#include "vexuf_pwm.h"
#include "vexuf_sdcard.h"
#include "vexuf_spi.h"
#include "vexuf_timers.h"
#include "vexuf_tnc.h"

/* TypeDef -------------------------------------------------------------------*/
extern IWDG_HandleTypeDef hiwdg;
extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

/* Defines -------------------------------------------------------------------*/
#define AHT20_ADDRESS 0x38

#define TIMER_10HZ_HANDLER htim4
#define TIMER_1HZ_HANDLER htim9
#define TIMER_0D1HZ_HANDLER htim5

#define TIMER_PWM1_HANDLER htim11
#define TIMER_PWM2_HANDLER htim10

#define UART_TTL_HANDLER huart1
#define UART_TNC_HANDLER huart6

/* Macros --------------------------------------------------------------------*/
// Define macro functions here

/* Extern Variables ----------------------------------------------------------*/
extern VexufStatus vexufStatus;
extern IndConfiguration indConf;
extern OutputConfiguration outputConfig;
extern PwmConfiguration pwmConfig;
extern ActuatorsConfiguration actConf;
extern ActuatorsValues actValues;
extern SerialConfiguration serialConf;
extern I2CConfiguration i2cConf;
extern LcdConfiguration lcdConf;
extern SpiType spiType;

extern unsigned char regNumber[REGISTRATION_NUMBER_LENGTH];
extern unsigned char ttlRxData[SERIAL_BUFFER_SIZE];
extern unsigned char tncRxData[SERIAL_BUFFER_SIZE];
extern unsigned char callsign[CALLSIGN_LENGTH];

/* Variables -----------------------------------------------------------------*/
// Declare static or global variables here

/* Prototypes ----------------------------------------------------------------*/
// Declare function prototypes here

/* Code ----------------------------------------------------------------------*/
int main(void) {
  /*
    Enable indicators for the duration of the startup routine.
    This makes sure that the initial WARN is turned on, and you
    can show the NO-CONF error sequence.
    TODO: Review the need for this before release.
  */
  indConf.globalIndicatorEnabled = 1;
  indConf.sdCardIndicatorEnabled = 1;
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
  EEPROM_93C86_init(&hspi1, EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);

  if (CONFIG_IsConfigured() != UF_OK) ERROR_handleNoConfig();
  if (CONFIG_WriteSerialNumber() != UF_OK) Error_Handler();
  // TODO: Check for registration number and handle it.
  if (CONFIG_getCallSign(&callsign) != UF_OK) Error_Handler();
  if (CONFIG_getPwmConfigurations(&pwmConfig) != UF_OK) Error_Handler();
  if (CONFIG_getActuators(&actConf, &actValues) != UF_OK) Error_Handler();
  if (CONFIG_getSerialConf(&serialConf) != UF_OK) Error_Handler();
  if (CONFIG_getI2cConf(&i2cConf) != UF_OK) Error_Handler();
  if (CONFIG_getLcdConf(&lcdConf) != UF_OK) Error_Handler();
  if (CONFIG_getSPIType(&spiType) != UF_OK) Error_Handler();
  // TODO: Implement Output Configuration
  if (CONFIG_getIndicatorsConf(&indConf) != UF_OK) Error_Handler();
  // TODO: Load AV Sensors configurations
  // TODO: Load Alarm configurations
  // TODO: Load Trigger configurations

  AHT20_Init(&hi2c1, AHT20_ADDRESS);
  // TODO: Init actuators.
  // TODO: Init AV Sensors
  // TODO: Init Alarms
  // TODO: Init Triggers

  CLI_init(&UART_TTL_HANDLER, &UART_TNC_HANDLER);
  SERIAL_init(&UART_TTL_HANDLER, &UART_TNC_HANDLER);

  PWM_init(&TIMER_PWM1_HANDLER, &TIMER_PWM2_HANDLER);

  TIMERS_init(&TIMER_10HZ_HANDLER, &TIMER_1HZ_HANDLER, &TIMER_0D1HZ_HANDLER);
  TIMERS_Start();

  // TODO: change number of rows to as configured
  LCD_Init();

  HAL_Delay(500);
  IND_BuzzOnStartUp();

  // TODO: remove the following tests before release
  ADC_Test();
  ACTUATORS_Test();
  I2C_ScanTest();
  // END OF TESTS

  // Start listening to the Serial interfaces
  HAL_UARTEx_ReceiveToIdle_IT(&UART_TTL_HANDLER, ttlRxData, SERIAL_BUFFER_SIZE);
  HAL_UARTEx_ReceiveToIdle_IT(&UART_TNC_HANDLER, tncRxData, SERIAL_BUFFER_SIZE);

  HAL_Delay(500);

  printf("VexUF Horus is ready.\n");
  HAL_GPIO_WritePin(WarnInd_GPIO_Port, WarnInd_Pin, GPIO_PIN_RESET);

#ifndef DEBUG
  MX_IWDG_Init();
#endif

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
      if (CLI_handleCommand(TTL) == UF_ERROR) {
        // todo: handle error
      }
      vexufStatus.ttlBuffered = 0;
    }
    if (vexufStatus.tncBuffered == 1) {
      if (CLI_handleCommand(TNC) == UF_ERROR) {
        // todo: handle error
      }
      vexufStatus.ttlBuffered = 0;
    }

    // Run this routine every iteration.
    HAL_IWDG_Refresh(&hiwdg);
  }
}