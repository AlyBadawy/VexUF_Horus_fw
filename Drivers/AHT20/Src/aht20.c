/**
 ******************************************************************************
 * @file          : aht20.c
 * @brief        : AHT20 Temperature and Humidity Sensor Implementation
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
#include "aht20.h"

#include "i2c.h"

/* TypeDef -------------------------------------------------------------------*/
static I2C_HandleTypeDef hi2c;

/* Defines -------------------------------------------------------------------*/
#define AHT20_INIT_CMD 0xBE
#define AHT20_TRIGGER_MEASUREMENT_CMD 0xAC
#define AHT20_MEASUREMENT_DELAY 100  // in milliseconds

/* Macros --------------------------------------------------------------------*/
#define AHT20_ADDRESS(add) (((add) & 0x7F) << 1)

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
static uint16_t address;

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
UF_STATUS AHT20_Init(I2C_HandleTypeDef *i2c, uint16_t i2cAddress) {
  hi2c = *i2c;
  address = i2cAddress;

  uint8_t initCmd[] = {AHT20_INIT_CMD, 0x08, 0x00};

  // Try to reset the I2C peripheral if it is busy
  if (hi2c.State == HAL_I2C_STATE_BUSY && I2C_Reset() != HAL_OK)
    return UF_ERROR;

  return HAL_I2C_Master_Transmit(&hi2c, AHT20_ADDRESS(address), initCmd,
                                 sizeof(initCmd), 200) == HAL_OK
             ? UF_OK
             : UF_ERROR;
}

// Read temperature and humidity from AHT20
UF_STATUS AHT20_ReadTemperatureHumidity(float *temperature, float *humidity) {
  HAL_Delay(AHT20_MEASUREMENT_DELAY);
  uint8_t data[6];
  uint8_t triggerCmd[] = {AHT20_TRIGGER_MEASUREMENT_CMD, 0x33, 0x00};

  // Reset I2C if busy
  if (hi2c.State == HAL_I2C_STATE_BUSY && I2C_Reset() != HAL_OK)
    return UF_ERROR;

  // Trigger measurement and read data
  if (HAL_I2C_Master_Transmit(&hi2c, AHT20_ADDRESS(address), triggerCmd,
                              sizeof(triggerCmd), HAL_MAX_DELAY) != HAL_OK ||
      HAL_I2C_Master_Receive(&hi2c, AHT20_ADDRESS(address), data, sizeof(data),
                             HAL_MAX_DELAY) != HAL_OK)
    return UF_ERROR;

  // Parse data
  uint32_t rawHumidity = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4);
  uint32_t rawTemperature = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];

  // Convert to human-readable values
  *humidity = ((float)rawHumidity / 1048576) * 100;
  *temperature = ((float)rawTemperature / 1048576) * 200 - 50;

  return UF_OK;
}
