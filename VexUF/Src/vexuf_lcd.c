/**
 ******************************************************************************
 * @file          : vexuf_lcd.c
 * @brief        : VexUF LCD Implementation
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
#include "vexuf_lcd.h"

#include <stdlib.h>

#include "hd44780.h"
#include "vexuf_config.h"

/* TypeDef -------------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern char *ok;
extern char *no;

/* Variables -----------------------------------------------------------------*/
LcdConfiguration lcdConf = {0};
uint8_t specialChars[5][8] = {
    {0x1f, 0x1b, 0x13, 0x1b, 0x1b, 0x1b, 0x11, 0x1f},  // one
    {0x1f, 0x1b, 0x15, 0x1d, 0x1b, 0x17, 0x11, 0x1f},  // two
    {0x1f, 0x11, 0x1d, 0x11, 0x1d, 0x1d, 0x11, 0x1f},  // three
    {0x08, 0x14, 0x08, 0x06, 0x09, 0x08, 0x09, 0x06},  // degree
    {0x04, 0x0e, 0x0e, 0x0e, 0x1f, 0x00, 0x04, 0x00}   // bell
};

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/

UF_STATUS LCD_Init(void) {
  switch (lcdConf.lcdType) {
    case NoLCD:
      return UF_DISABLED;
    case LCD2004:
      HD44780_Init(&hi2c1, 4, lcdConf.lcdAdd);
      break;
    case LCD1602:
      return UF_NOT_CONFIGURED;  // TODO: Implement this case
      HD44780_Init(&hi2c1, 2, lcdConf.lcdAdd);
      break;
    default:
      return UF_ERROR;
  }

  if (lcdConf.lcdType == NoLCD) return UF_DISABLED;
  if (lcdConf.lcdType == LCD2004) HD44780_Init(&hi2c1, 4, lcdConf.lcdAdd);
  if (lcdConf.lcdType == LCD1602) HD44780_Init(&hi2c1, 2, lcdConf.lcdAdd);

  // Create special characters
  for (uint8_t i = 0; i < 5; i++) {
    HD44780_CreateSpecialChar(i, specialChars[i]);
  }
  HD44780_Home();

  HD44780_MoveCursor(1, 4);
  HD44780_PrintStr("VexUF: Horus");

  return UF_OK;
}

void LCD_handleCli(const char *args, char *responseBuffer) {
  if (strlen(args) == 0) {
    // Show current type of LCD
    switch (lcdConf.lcdType) {
      case NoLCD:
        sprintf(responseBuffer, "LCD is disabled. %s", ok);
        break;
      case LCD2004:
        sprintf(responseBuffer, "LCD type: 2004. %s", ok);
        break;
      case LCD1602:
        sprintf(responseBuffer, "LCD type: 1602. %s", ok);
        break;
      default:
        sprintf(responseBuffer, "Unknown LCD type. %s", no);
        break;
    }
  } else {
    // Set LCD type based on args
    if (strcmp(args, "2004") == 0) {
      lcdConf.lcdType = LCD2004;
      HD44780_Init(&hi2c1, 4, lcdConf.lcdAdd);
      sprintf(responseBuffer, "LCD type set to 2004. %s", ok);
    } else if (strcmp(args, "1602") == 0) {
      lcdConf.lcdType = LCD1602;
      HD44780_Init(&hi2c1, 2, lcdConf.lcdAdd);
      sprintf(responseBuffer, "LCD type set to 1602. %s", ok);
    } else if (strcmp(args, "disable") == 0) {
      lcdConf.lcdType = NoLCD;
      sprintf(responseBuffer, "LCD disabled. %s", ok);
    } else if (strncmp(args, "address", 7) == 0) {
      if (strlen(args) < 8) {
        sprintf(responseBuffer, "I2C address set to 0x%02X. %s", lcdConf.lcdAdd,
                ok);
        return;
      }
      uint8_t i2cAddress = (uint8_t)strtol(args + 7, NULL, 16);

      // Set the I2C address for the LCD
      if (i2cAddress <= 0x7F) {
        lcdConf.lcdAdd = i2cAddress;
        sprintf(responseBuffer, "I2C address set to 0x%02X. %s", i2cAddress,
                ok);
      } else {
        sprintf(responseBuffer, "Invalid I2C address");
      }
      // Set the I2C address for the LCD
      if (i2cAddress <= 0x7F) {
        lcdConf.lcdAdd = i2cAddress;
        sprintf(responseBuffer, "I2C address set to 0x%02X. %s", i2cAddress,
                ok);
      } else {
        sprintf(responseBuffer, "Invalid I2C address");
      }
      // Set the I2C address for the LCD
      if (i2cAddress <= 0x7F) {
        lcdConf.lcdAdd = i2cAddress;
        sprintf(responseBuffer, "I2C address set to 0x%02X. %s", i2cAddress,
                ok);
      } else {
        sprintf(responseBuffer, "Invalid I2C address");
      }
    } else {
      sprintf(responseBuffer, "Invalid argument");
    }
  }
}

/* Private Methods -----------------------------------------------------------*/
