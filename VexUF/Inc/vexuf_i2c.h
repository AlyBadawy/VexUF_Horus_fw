#ifndef VEXUF_I2C_H
#define VEXUF_I2C_H

#include "vexuf.h"

typedef enum { NoI2C = 0, AHT20 = 1, AHT21 = 2 } I2CType;

typedef struct {
  uint16_t i2cAdd;
  I2CType i2cType;  // Use the enum type for lcdType
} I2CConfiguration;

#endif  // VEXUF_I2C_H