/*
 * hd44780_.h
 *
 *  Created on: Aug 18, 2024
 *      Author: Aly Badawy
 */

#ifndef HD44780_H_
#define HD44780_H_

#include "main.h"

/* Command */
#define HD44780_CLEARDISPLAY 0x01
#define HD44780_RETURNHOME 0x02
#define HD44780_ENTRYMODESET 0x04
#define HD44780_DISPLAYCONTROL 0x08
#define HD44780_CURSORSHIFT 0x10
#define HD44780_FUNCTIONSET 0x20
#define HD44780_SETCGRAMADDR 0x40
#define HD44780_SETDDRAMADDR 0x80

/* Entry Mode */
#define HD44780_ENTRYRIGHT 0x00
#define HD44780_ENTRYLEFT 0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

/* Display On/Off */
#define HD44780_DISPLAYON 0x04
#define HD44780_DISPLAYOFF 0x00
#define HD44780_CURSORON 0x02
#define HD44780_CURSOROFF 0x00
#define HD44780_BLINKON 0x01
#define HD44780_BLINKOFF 0x00

/* Cursor Shift */
#define HD44780_DISPLAYMOVE 0x08
#define HD44780_CURSORMOVE 0x00
#define HD44780_MOVERIGHT 0x04
#define HD44780_MOVELEFT 0x00

/* Function Set */
#define HD44780_8BITMODE 0x10
#define HD44780_4BITMODE 0x00
#define HD44780_2LINE 0x08
#define HD44780_1LINE 0x00
#define HD44780_5x10DOTS 0x04
#define HD44780_5x8DOTS 0x00

/* Backlight */
#define HD44780_BACKLIGHT 0x08
#define HD44780_NOBACKLIGHT 0x00

/* Enable Bit */
#define ENABLE 0x04

/* Read Write Bit */
#define RW 0x0

/* Register Select Bit */
#define RS 0x01

void HD44780_Init(uint8_t rows);
void HD44780_Clear(void);
void HD44780_Home(void);
void HD44780_NoDisplay(void);
void HD44780_Display(void);
void HD44780_NoBlink(void);
void HD44780_Blink(void);
void HD44780_NoCursor(void);
void HD44780_Cursor(void);
void HD44780_ScrollDisplayLeft(void);
void HD44780_ScrollDisplayRight(void);
void HD44780_PrintLeft(void);
void HD44780_PrintRight(void);
void HD44780_LeftToRight(void);
void HD44780_RightToLeft(void);
void HD44780_ShiftIncrement(void);
void HD44780_ShiftDecrement(void);
void HD44780_NoBacklight(void);
void HD44780_Backlight(void);
void HD44780_AutoScroll(void);
void HD44780_NoAutoScroll(void);
void HD44780_CreateSpecialChar(uint8_t, uint8_t[]);
void HD44780_PrintSpecialChar(uint8_t);
void HD44780_SetCursor(uint8_t, uint8_t);
void HD44780_SetBacklight(uint8_t new_val);
void HD44780_LoadCustomCharacter(uint8_t char_num, uint8_t *rows);
void HD44780_PrintStr(const char[]);

#endif /* HD44780_H_ */