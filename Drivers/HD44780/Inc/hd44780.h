/*
 * hd44780_.h
 *
 *  Created on: Aug 18, 2024
 *      Author: Aly Badawy
 */

#ifndef HD44780_H_
#define HD44780_H_

#include "main.h"

void HD44780_Init(I2C_HandleTypeDef *i2c, uint8_t rows);
void HD44780_Clear(void);
void HD44780_Home(void);
void HD44780_MoveCursor(uint8_t row, uint8_t col);
void HD44780_SetDisplay(uint8_t isDisplayOn);
void HD44780_SetCursor(uint8_t isCursorOn);
void HD44780_SetBlink(uint8_t isBlinkOn);
void HD44780_SetBacklight(uint8_t isBacklightOn);
void HD44780_SetAutoScroll(uint8_t isAutoScroll);
void HD44780_ScrollDisplayLeft(void);
void HD44780_ScrollDisplayRight(void);
void HD44780_LeftToRight(void);
void HD44780_RightToLeft(void);
void HD44780_CreateSpecialChar(uint8_t, uint8_t[]);
void HD44780_PrintSpecialChar(uint8_t);
void HD44780_PrintStr(const char[]);

#endif /* HD44780_H_ */