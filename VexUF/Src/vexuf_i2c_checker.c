/**
 ******************************************************************************
 * @file          : vexuf_i2c_checker.c
 * @brief        : I2C bus scanner for VexUF
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
#include "vexuf_i2c_checker.h"

/* TypeDef -------------------------------------------------------------------*/
I2C_HandleTypeDef hi2c;
/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/

/* Private Methods -----------------------------------------------------------*/

// Function to scan I2C bus for devices and store found addresses in the
// provided buffer
UF_STATUS I2C_Scan(I2C_HandleTypeDef *hi2c, uint8_t *foundDevices,
                   uint8_t *foundDevicesCount) {
  uint8_t count = 0;

  for (uint8_t i2c_address = 1; i2c_address < 128; i2c_address++) {
    if (HAL_I2C_IsDeviceReady(hi2c, (i2c_address << 1), 1, 10) == HAL_OK) {
      foundDevices[count++] = i2c_address;
    }
  }
  *foundDevicesCount = count;
  return UF_OK;
}

void I2C_ScanTest(void) {
  uint8_t foundDevices[128];
  uint8_t foundDevicesCount;
  extern I2C_HandleTypeDef hi2c1;

  char tempString[8];          // Temporary string to hold the formatted addres
  char formattedString[1024];  // Ensure this is large enough to hold all
                               // addresses

  formattedString[0] = '\0';

  // Scan for I2C devices
  I2C_Scan(&hi2c1, foundDevices, &foundDevicesCount);

  for (uint8_t i = 0; i < foundDevicesCount; i++) {
    // Format the address as a hex string and store it in tempString
    sprintf(tempString, "0x%02X\r\n", foundDevices[i]);
    // Append the formatted address to the final formatted string
    strcat(formattedString, tempString);
  }
  printf("%s", formattedString);
}
