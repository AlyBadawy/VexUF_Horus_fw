/*
 * aht20.h
 *
 *  Created on: Jul 26, 2024
 *      Author: alybadawy
 */

#ifndef INC_VEXUF_I2C_AHT20_H_
#define INC_VEXUF_I2C_AHT20_H_

#include "vexuf.h"

UF_STATUS AHT20_Init(I2C_HandleTypeDef *i2c, uint16_t i2cAddress);
UF_STATUS AHT20_ReadTemperatureHumidity(float *temperature, float *humidity);

#endif /* INC_VEXUF_I2C_AHT20_H_ */