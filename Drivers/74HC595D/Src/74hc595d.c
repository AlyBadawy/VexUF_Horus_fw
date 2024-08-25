#include "74hc595d.h"

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