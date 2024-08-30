/**
 ******************************************************************************
 * @file          : hd44780.c
 * @brief        : HD44780 LCD Driver Implementation
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
#include "hd44780.h"

/* TypeDef -------------------------------------------------------------------*/
static I2C_HandleTypeDef hi2c;

/* Defines -------------------------------------------------------------------*/
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

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
uint8_t dpFunction;
uint8_t dpControl;
uint8_t dpMode;
uint8_t dpRows;
uint8_t dpBacklight;

/* Prototypes ----------------------------------------------------------------*/
static void SendCommand(uint8_t);
static void SendChar(uint8_t);
static void Send(uint8_t, uint8_t);
static void Write4Bits(uint8_t);
static void ExpanderWrite(uint8_t);
static void PulseEnable(uint8_t);
static void DelayInit(void);
static void DelayUS(uint32_t);
/* Code ----------------------------------------------------------------------*/

void HD44780_Init(I2C_HandleTypeDef *i2c, uint8_t rows) {
  hi2c = *i2c;
  dpRows = rows;
  dpBacklight = HD44780_BACKLIGHT;
  dpFunction = HD44780_4BITMODE | HD44780_1LINE | HD44780_5x8DOTS;

  dpFunction |= (dpRows > 1) ? HD44780_2LINE : HD44780_5x10DOTS;

  /* Wait for initialization */
  DelayInit();
  HAL_Delay(50);
  ExpanderWrite(dpBacklight);
  HAL_Delay(200);

  // 4-bit mode initialization
  for (int i = 0; i < 3; i++) {
    Write4Bits(0x03 << 4);
    DelayUS(4500);
  }
  Write4Bits(0x02 << 4);
  DelayUS(100);

  /* Display Control */
  SendCommand(HD44780_FUNCTIONSET | dpFunction);
  dpControl = HD44780_DISPLAYON | HD44780_CURSOROFF | HD44780_BLINKOFF;
  HD44780_SetDisplay(1);
  HD44780_Clear();

  /* Display Mode */
  dpMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
  SendCommand(HD44780_ENTRYMODESET | dpMode);
  DelayUS(4500);
}
void HD44780_Clear(void) {
  SendCommand(HD44780_CLEARDISPLAY);
  DelayUS(2000);
}
void HD44780_Home(void) {
  SendCommand(HD44780_RETURNHOME);
  DelayUS(2000);
}
void HD44780_MoveCursor(uint8_t row, uint8_t col) {
  int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row >= dpRows) row = dpRows - 1;
  SendCommand(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}
void HD44780_SetDisplay(uint8_t isDisplayOn) {
  dpControl = isDisplayOn ? (dpControl | HD44780_DISPLAYON)
                          : (dpControl & ~HD44780_DISPLAYON);
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}
void HD44780_SetCursor(uint8_t isCursorOn) {
  dpControl = isCursorOn ? (dpControl | HD44780_CURSORON)
                         : (dpControl & ~HD44780_CURSORON);
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}
void HD44780_SetBlink(uint8_t isBlinkOn) {
  dpControl = isBlinkOn ? (dpControl | HD44780_BLINKON)
                        : (dpControl & ~HD44780_BLINKON);
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}
void HD44780_SetBacklight(uint8_t isBacklightOn) {
  dpBacklight = isBacklightOn ? HD44780_BACKLIGHT : HD44780_NOBACKLIGHT;
  ExpanderWrite(0);
}
void HD44780_SetAutoScroll(uint8_t isAutoScroll) {
  dpMode = isAutoScroll ? (dpMode | HD44780_ENTRYSHIFTINCREMENT)
                        : (dpMode & ~HD44780_ENTRYSHIFTINCREMENT);
  SendCommand(HD44780_ENTRYMODESET | dpMode);
}
void HD44780_ScrollDisplayLeft(void) {
  SendCommand(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}
void HD44780_ScrollDisplayRight(void) {
  SendCommand(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}
void HD44780_LeftToRight(void) {
  dpMode |= HD44780_ENTRYLEFT;
  SendCommand(HD44780_ENTRYMODESET | dpMode);
}
void HD44780_RightToLeft(void) {
  dpMode &= ~HD44780_ENTRYLEFT;
  SendCommand(HD44780_ENTRYMODESET | dpMode);
}
void HD44780_CreateSpecialChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7;
  SendCommand(HD44780_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) {
    SendChar(charmap[i]);
  }
}
void HD44780_PrintSpecialChar(uint8_t index) { SendChar(index); }
void HD44780_PrintStr(const char c[]) {
  while (*c) SendChar(*c++);
}

/* Private Methods -----------------------------------------------------------*/
static void SendCommand(uint8_t cmd) { Send(cmd, 0); }
static void SendChar(uint8_t ch) { Send(ch, RS); }
static void Send(uint8_t value, uint8_t mode) {
  for (int i = 0; i < 2; i++) {
    Write4Bits(((value << (i * 4)) & 0xF0) | mode);
  }
}
static void Write4Bits(uint8_t value) {
  ExpanderWrite(value);
  PulseEnable(value);
}
static void ExpanderWrite(uint8_t _data) {
  uint8_t data = _data | dpBacklight;
  HAL_I2C_Master_Transmit(&hi2c, (0x27 << 1), (uint8_t *)&data, 1, 10);
}
static void PulseEnable(uint8_t _data) {
  ExpanderWrite(_data | ENABLE);
  DelayUS(20);

  ExpanderWrite(_data & ~ENABLE);
  DelayUS(20);
}
static void DelayInit(void) {
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;

  // 3 NO OPERATION instructions
  __ASM volatile("NOP; NOP; NOP");
}
static void DelayUS(uint32_t us) {
  uint32_t cycles = (SystemCoreClock / 1000000L) * us;
  uint32_t start = DWT->CYCCNT;
  volatile uint32_t cnt;

  while ((DWT->CYCCNT - start) < cycles);
}
