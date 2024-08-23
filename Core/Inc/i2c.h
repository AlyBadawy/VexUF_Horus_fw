#ifndef __I2C_H__
#define __I2C_H__

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

typedef enum {NoI2C = 0, AHT20 = 1, AHT21 = 2} I2CType;

typedef struct {
    uint16_t i2cAdd;
    I2CType i2cType;  // Use the enum type for lcdType
} I2CConfiguration;

void MX_I2C1_Init(void);

#endif /* __I2C_H__ */

