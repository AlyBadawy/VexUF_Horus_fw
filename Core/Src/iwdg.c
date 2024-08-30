/**
 ******************************************************************************
 * @file          : iwdg.c
 * @brief         : IWDG Initialization
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
#include "iwdg.h"

/* TypeDef -------------------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;

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
void MX_IWDG_Init(void) {
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = 625;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK) Error_Handler();
}
