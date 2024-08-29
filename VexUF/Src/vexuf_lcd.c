/*
 * vexuf_lcd.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_lcd.h"

#include "hd44780.h"

LcdConfiguration lcdConf = {0};

// Initialize the LCD
UF_STATUS LCD_Init(void) {
  lcdConf.lcdAdd = 0x27;
  lcdConf.lcdType = LCD2004;
  lcdConf.lcdPwm = 100;

  HD44780_Init(4);

  return UF_OK;
}