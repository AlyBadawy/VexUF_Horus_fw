#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx_hal.h"

void Error_Handler(void);

#define Tamper_Pin GPIO_PIN_13
#define Tamper_GPIO_Port GPIOC
#define LSEin_Pin GPIO_PIN_14
#define LSEin_GPIO_Port GPIOC
#define LSEout_Pin GPIO_PIN_15
#define LSEout_GPIO_Port GPIOC
#define HSEin_Pin GPIO_PIN_0
#define HSEin_GPIO_Port GPIOH
#define HSEout_Pin GPIO_PIN_1
#define HSEout_GPIO_Port GPIOH
#define ErrorInd_Pin GPIO_PIN_0
#define ErrorInd_GPIO_Port GPIOC
#define WarnInd_Pin GPIO_PIN_1
#define WarnInd_GPIO_Port GPIOC
#define InfoInd_Pin GPIO_PIN_2
#define InfoInd_GPIO_Port GPIOC
#define SdioInd_Pin GPIO_PIN_3
#define SdioInd_GPIO_Port GPIOC
#define Av3Indicator_Pin GPIO_PIN_0
#define Av3Indicator_GPIO_Port GPIOA
#define Av2Indicator_Pin GPIO_PIN_1
#define Av2Indicator_GPIO_Port GPIOA
#define Av1Indicator_Pin GPIO_PIN_2
#define Av1Indicator_GPIO_Port GPIOA
#define EEPROM_CS_Pin GPIO_PIN_3
#define EEPROM_CS_GPIO_Port GPIOA
#define SPI_CS_Pin GPIO_PIN_4
#define SPI_CS_GPIO_Port GPIOA
#define SPI_CK_Pin GPIO_PIN_5
#define SPI_CK_GPIO_Port GPIOA
#define SPI_MISO_Pin GPIO_PIN_6
#define SPI_MISO_GPIO_Port GPIOA
#define SPI_MOSI_Pin GPIO_PIN_7
#define SPI_MOSI_GPIO_Port GPIOA
#define EepromMemOrg_Pin GPIO_PIN_4
#define EepromMemOrg_GPIO_Port GPIOC
#define Av1_Pin GPIO_PIN_5
#define Av1_GPIO_Port GPIOC
#define Av2_Pin GPIO_PIN_0
#define Av2_GPIO_Port GPIOB
#define Av3_Pin GPIO_PIN_1
#define Av3_GPIO_Port GPIOB
#define Boot1_Pin GPIO_PIN_2
#define Boot1_GPIO_Port GPIOB
#define Buzzer_Pin GPIO_PIN_10
#define Buzzer_GPIO_Port GPIOB
#define SHIFTREG_74HC595D_Da_Pin GPIO_PIN_12
#define SHIFTREG_74HC595D_Da_GPIO_Port GPIOB
#define SHIFTREG_74HC595D_La_Pin GPIO_PIN_13
#define SHIFTREG_74HC595D_La_GPIO_Port GPIOB
#define SHIFTREG_74HC595D_Ck_Pin GPIO_PIN_14
#define SHIFTREG_74HC595D_Ck_GPIO_Port GPIOB
#define ActInd_Pin GPIO_PIN_15
#define ActInd_GPIO_Port GPIOB
#define MCO_Pin GPIO_PIN_8
#define MCO_GPIO_Port GPIOA
#define SWDio_Pin GPIO_PIN_13
#define SWDio_GPIO_Port GPIOA
#define SWCk_Pin GPIO_PIN_14
#define SWCk_GPIO_Port GPIOA
#define SDIO_DET_Pin GPIO_PIN_15
#define SDIO_DET_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define TTL_LED_Pin GPIO_PIN_4
#define TTL_LED_GPIO_Port GPIOB
#define LCD_pwm_Pin GPIO_PIN_5
#define LCD_pwm_GPIO_Port GPIOB
#define Servo2Pwm_Pin GPIO_PIN_8
#define Servo2Pwm_GPIO_Port GPIOB
#define Servo1PWM_Pin GPIO_PIN_9
#define Servo1PWM_GPIO_Port GPIOB

#endif /* __MAIN_H */
