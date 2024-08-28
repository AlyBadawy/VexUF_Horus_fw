/*
 * vexuf_lcd.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Aly Badawy
 */

#if !defined(INC_VEXUF_LCD_H)
#define INC_VEXUF_LCD_H

#include "vexuf.h"

typedef enum { NoLCD = 0, LCD2004 = 1, LCD1602 = 2 } LcdType;

typedef struct {
  uint16_t lcdAdd;
  LcdType lcdType;  // Use the enum type for lcdType
  uint16_t lcdPwm;
} LcdConfiguration;

#endif  // INC_VEXUF_LCD_H