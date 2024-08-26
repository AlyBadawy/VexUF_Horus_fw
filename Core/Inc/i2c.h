#ifndef __I2C_H__
#define __I2C_H__

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

void MX_I2C1_Init(void);

HAL_StatusTypeDef I2C_Reset();

#endif /* __I2C_H__ */
