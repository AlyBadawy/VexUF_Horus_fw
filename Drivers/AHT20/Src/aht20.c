/*
 * i2c_aht20.c
 *
 *  Created on: Jul 26, 2024
 *      Author: alybadawy
 */

#include "aht20.h"

extern I2C_HandleTypeDef hi2c1;

#define AHT20_INIT_CMD 0xBE
#define AHT20_TRIGGER_MEASUREMENT_CMD 0xAC
#define AHT20_MEASUREMENT_DELAY 100 // in milliseconds


// AHT20 initialization
HAL_StatusTypeDef AHT20_Init(void) {
    uint8_t initCmd[] = {AHT20_INIT_CMD, 0x08, 0x00};
    HAL_StatusTypeDef status = HAL_BUSY;

    // Try to reset the I2C peripheral if it is busy
    if (hi2c1.State == HAL_I2C_STATE_BUSY) {
        status = I2C_Reset();
        if (status != HAL_OK) {
            return status;
        }
    }

    // Transmit initialization command
    status = HAL_I2C_Master_Transmit(&hi2c1, 0x38 << 1, initCmd, sizeof(initCmd), HAL_MAX_DELAY);
    return status;
}

// Read temperature and humidity from AHT20
HAL_StatusTypeDef AHT20_ReadTemperatureHumidity(float *temperature, float *humidity) {
	HAL_Delay(AHT20_MEASUREMENT_DELAY);
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t data[6];
    uint8_t triggerCmd[] = {AHT20_TRIGGER_MEASUREMENT_CMD, 0x33, 0x00};

    // Try to reset the I2C peripheral if it is busy
    if (hi2c1.State == HAL_I2C_STATE_BUSY) {
        status = I2C_Reset();
        if (status != HAL_OK) {
            return status;
        }
    }

    // Trigger measurement
    status = HAL_I2C_Master_Transmit(&hi2c1, 0x38 << 1, triggerCmd, sizeof(triggerCmd), HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return status;
    }

    // Wait for the measurement to complete
    HAL_Delay(AHT20_MEASUREMENT_DELAY);

    // Read measurement data
    status = HAL_I2C_Master_Receive(&hi2c1, 0x38 << 1, data, sizeof(data), HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return status;
    }

    // Parse data
    uint32_t rawHumidity = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4);
    uint32_t rawTemperature = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];

    // Convert to human-readable values
    *humidity = ((float)rawHumidity / 1048576) * 100;
    *temperature = ((float)rawTemperature / 1048576) * 200 - 50;

    return HAL_OK;
}