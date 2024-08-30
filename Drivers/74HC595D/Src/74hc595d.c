/**
 ******************************************************************************
 * @file          : 74hc595d.c
 * @brief         : 74HC595D Shift Register Implementation
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
#include "74hc595d.h"

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
void SHIFTREG_74HC595D_update(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    uint8_t pinValue = (data >> (7 - i)) & 0x01;  // Normalize to 0 or 1
    GPIO_PinState state = pinValue ? GPIO_PIN_SET : GPIO_PIN_RESET;
    // Write the data bit by bit

    HAL_GPIO_WritePin(SHIFTREG_74HC595D_Da_GPIO_Port, SHIFTREG_74HC595D_Da_Pin,
                      state);

    // Toggle the clock pin
    HAL_GPIO_WritePin(SHIFTREG_74HC595D_Ck_GPIO_Port, SHIFTREG_74HC595D_Ck_Pin,
                      GPIO_PIN_SET);
    HAL_GPIO_WritePin(SHIFTREG_74HC595D_Ck_GPIO_Port, SHIFTREG_74HC595D_Ck_Pin,
                      GPIO_PIN_RESET);
  }

  // Toggle the latch pin to output the data
  HAL_GPIO_WritePin(SHIFTREG_74HC595D_La_GPIO_Port, SHIFTREG_74HC595D_La_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(SHIFTREG_74HC595D_La_GPIO_Port, SHIFTREG_74HC595D_La_Pin,
                    GPIO_PIN_RESET);
}