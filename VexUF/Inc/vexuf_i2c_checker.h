/*
 * vexuf_i2c_checker.h
 *
 *  Created on: Jul 26, 2024
 *      Author: alybadawy
 */

#ifndef INC_VEXUF_I2C_CHECKER_H_
#define INC_VEXUF_I2C_CHECKER_H_

#include "vexuf.h"

UF_STATUS I2C_Scan(uint8_t *foundDevices, uint8_t *foundDevicesCount);
void I2C_ScanTest(void);

#endif /* INC_VEXUF_I2C_CHECKER_H_ */