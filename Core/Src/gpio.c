#include "gpio.h"

void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOC, ErrorInd_Pin|WarnInd_Pin|InfoInd_Pin|SdioInd_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, Av3Indicator_Pin|Av2Indicator_Pin|Av1Indicator_Pin|EEPROM_CS_Pin
                          |SPI_CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(EepromMemOrg_GPIO_Port, EepromMemOrg_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, Buzzer_Pin|ActDa_Pin|ActLa_Pin|ActCk_Pin
                          |ActInd_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TTL_LED_GPIO_Port, TTL_LED_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = ErrorInd_Pin|WarnInd_Pin|InfoInd_Pin|SdioInd_Pin
                          |EepromMemOrg_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = Av3Indicator_Pin|Av2Indicator_Pin|Av1Indicator_Pin|EEPROM_CS_Pin
                          |SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = Boot1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Boot1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = Buzzer_Pin|ActDa_Pin|ActLa_Pin|ActCk_Pin
                          |ActInd_Pin|TTL_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = MCO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(MCO_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SDIO_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SDIO_DET_GPIO_Port, &GPIO_InitStruct);
}
