/**
 ******************************************************************************
 * @file          : stm32f4xx_hal_msp.c
 * @brief         : STM32f4xx HAL MSP file
 ******************************************************************************
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @copyright     : STMicroelectronics and Aly Badawy
 * @author website: https://alybadawy.com
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* TypeDef -------------------------------------------------------------------*/
// Define types here

/* Defines -------------------------------------------------------------------*/
// Define macros and constants here

/* Macros --------------------------------------------------------------------*/
// Define macro functions here

/* Extern Variables ----------------------------------------------------------*/
// Declare external variables here

/* Variables -----------------------------------------------------------------*/
// Declare static or global variables here

/* Prototypes ----------------------------------------------------------------*/
// Declare function prototypes here

/* Code ----------------------------------------------------------------------*/
void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriority(RCC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RCC_IRQn);

  HAL_NVIC_SetPriority(FPU_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FPU_IRQn);
}
