/*
 * vexuf_i2c_checker.c
 *
 *  Created on: Jul 26, 2024
 *      Author: alybadawy
 */

#include "vexuf_i2c_checker.h"

extern I2C_HandleTypeDef hi2c1;

// Function to scan I2C bus for devices and store found addresses in the provided buffer
void I2C_Scan(uint8_t *foundDevices, uint8_t *foundDevicesCount) {
    uint8_t i2c_address;
    HAL_StatusTypeDef result;
    uint8_t count = 0;

    for (i2c_address = 1; i2c_address < 128; i2c_address++) {
        result = HAL_I2C_IsDeviceReady(&hi2c1, (i2c_address << 1), 1, 10);
        if (result == HAL_OK) {
        	foundDevices[count++] = i2c_address;
        }
    }

    *foundDevicesCount = count;
}

void I2C_ScanTest(void) {
	uint8_t foundDevices[128];
	uint8_t foundDevicesCount;

	char tempString[8]; // Temporary string to hold the formatted addres
	char formattedString[1024]; // Ensure this is large enough to hold all addresses

	formattedString[0] = '\0';

	// Scan for I2C devices
	I2C_Scan(foundDevices, &foundDevicesCount);

	for (uint8_t i = 0; i < foundDevicesCount; i++) {
		// Format the address as a hex string and store it in tempString
		sprintf(tempString, "0x%02X\r\n", foundDevices[i]);
		// Append the formatted address to the final formatted string
		strcat(formattedString, tempString);
	}
	printf("%s", formattedString);
}
