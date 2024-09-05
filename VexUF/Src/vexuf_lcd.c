/*
 * vexuf_lcd.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_lcd.h"

#include "hd44780.h"
#include "vexuf_config.h"

extern I2C_HandleTypeDef hi2c1;

LcdConfiguration lcdConf = {0};

uint8_t specialChars[5][8] = {
    {0x1f, 0x1b, 0x13, 0x1b, 0x1b, 0x1b, 0x11, 0x1f},  // one
    {0x1f, 0x1b, 0x15, 0x1d, 0x1b, 0x17, 0x11, 0x1f},  // two
    {0x1f, 0x11, 0x1d, 0x11, 0x1d, 0x1d, 0x11, 0x1f},  // three
    {0x08, 0x14, 0x08, 0x06, 0x09, 0x08, 0x09, 0x06},  // degree
    {0x04, 0x0e, 0x0e, 0x0e, 0x1f, 0x00, 0x04, 0x00}   // bell
};

// Initialize the LCD
UF_STATUS LCD_Init(void) {
  switch (lcdConf.lcdType) {
    case NoLCD:
      return UF_DISABLED;
    case LCD2004:
      HD44780_Init(&hi2c1, 4);
      break;
    case LCD1602:
      HD44780_Init(&hi2c1, 2);
      break;
    default:
      return UF_DISABLED;  // TODO: use error when default case is implemented
  }

  if (lcdConf.lcdType == NoLCD) return UF_DISABLED;
  if (lcdConf.lcdType == LCD2004) HD44780_Init(&hi2c1, 4);
  if (lcdConf.lcdType == LCD1602) HD44780_Init(&hi2c1, 2);

  // Create special characters
  for (uint8_t i = 0; i < 5; i++) {
    HD44780_CreateSpecialChar(i, specialChars[i]);
  }
  HD44780_Home();

  return UF_OK;
}