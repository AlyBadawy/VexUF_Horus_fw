/*
 * hd44780.c
 *
 *  Created on: Aug 18, 2024
 *      Author: Aly Badawy
 */

#include "hd44780.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t dpFunction;
uint8_t dpControl;
uint8_t dpMode;
uint8_t dpRows;
uint8_t dpBacklight;

static void SendCommand(uint8_t);
static void SendChar(uint8_t);
static void Send(uint8_t, uint8_t);
static void Write4Bits(uint8_t);
static void ExpanderWrite(uint8_t);
static void PulseEnable(uint8_t);
static void DelayInit(void);
static void DelayUS(uint32_t);

uint8_t one[8] = {0x1f, 0x1b, 0x13, 0x1b, 0x1b, 0x1b, 0x11, 0x1f};
uint8_t two[8] = {0x1f, 0x1b, 0x15, 0x1d, 0x1b, 0x17, 0x11, 0x1f};
uint8_t three[8] = {0x1f, 0x11, 0x1d, 0x11, 0x1d, 0x1d, 0x11, 0x1f};

uint8_t degree[8] = { 0x08, 0x14, 0x08, 0x06, 0x09, 0x08, 0x09, 0x06 };
uint8_t bell[8] = {0x04, 0x0e, 0x0e, 0x0e, 0x1f, 0x00, 0x04, 0x00};


void HD44780_Init(uint8_t rows) {
  dpRows = rows;

  dpBacklight = HD44780_BACKLIGHT;

  dpFunction = HD44780_4BITMODE | HD44780_1LINE | HD44780_5x8DOTS;

  if (dpRows > 1)
  {
    dpFunction |= HD44780_2LINE;
  }
  else
  {
    dpFunction |= HD44780_5x10DOTS;
  }

  /* Wait for initialization */
  DelayInit();
  HAL_Delay(50);

  ExpanderWrite(dpBacklight);
  HAL_Delay(1000);

  /* 4bit Mode */
  Write4Bits(0x03 << 4);
  DelayUS(4500);

  Write4Bits(0x03 << 4);
  DelayUS(4500);

  Write4Bits(0x03 << 4);
  DelayUS(4500);

  Write4Bits(0x02 << 4);
  DelayUS(100);

  /* Display Control */
  SendCommand(HD44780_FUNCTIONSET | dpFunction);

  dpControl = HD44780_DISPLAYON | HD44780_CURSOROFF | HD44780_BLINKOFF;
  HD44780_Display();
  HD44780_Clear();

  /* Display Mode */
  dpMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
  SendCommand(HD44780_ENTRYMODESET | dpMode);
  DelayUS(4500);

  HD44780_CreateSpecialChar(0, one);
  HD44780_CreateSpecialChar(1, two);
  HD44780_CreateSpecialChar(2, three);
  HD44780_CreateSpecialChar(3, bell);
  HD44780_CreateSpecialChar(4, degree);

  HD44780_Home();
}

void HD44780_Clear(void) {
  SendCommand(HD44780_CLEARDISPLAY);
  DelayUS(2000);
}

void HD44780_Home(void) {
  SendCommand(HD44780_RETURNHOME);
  DelayUS(2000);
}

void HD44780_SetCursor(uint8_t row, uint8_t col) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if (row >= dpRows)
  {
    row = dpRows-1;
  }
  SendCommand(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

void HD44780_NoDisplay(void) {
  dpControl &= ~HD44780_DISPLAYON;
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_Display(void) {
  dpControl |= HD44780_DISPLAYON;
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_NoCursor(void) {
  dpControl &= ~HD44780_CURSORON;
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_Cursor(void) {
  dpControl |= HD44780_CURSORON;
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_NoBlink(void) {
  dpControl &= ~HD44780_BLINKON;
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_Blink(void) {
  dpControl |= HD44780_BLINKON;
  SendCommand(HD44780_DISPLAYCONTROL | dpControl);
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

void HD44780_AutoScroll(void) {
  dpMode |= HD44780_ENTRYSHIFTINCREMENT;
  SendCommand(HD44780_ENTRYMODESET | dpMode);
}

void HD44780_NoAutoScroll(void) {
  dpMode &= ~HD44780_ENTRYSHIFTINCREMENT;
  SendCommand(HD44780_ENTRYMODESET | dpMode);
}

void HD44780_CreateSpecialChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7;
  SendCommand(HD44780_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++)
  {
    SendChar(charmap[i]);
  }
}

void HD44780_PrintSpecialChar(uint8_t index) {
  SendChar(index);
}

void HD44780_LoadCustomCharacter(uint8_t char_num, uint8_t *rows) {
  HD44780_CreateSpecialChar(char_num, rows);
}

void HD44780_PrintStr(const char c[]) {
  while(*c) SendChar(*c++);
}

void HD44780_SetBacklight(uint8_t new_val) {
  if(new_val) HD44780_Backlight();
  else HD44780_NoBacklight();
}

void HD44780_NoBacklight(void) {
  dpBacklight=HD44780_NOBACKLIGHT;
  ExpanderWrite(0);
}

void HD44780_Backlight(void) {
  dpBacklight=HD44780_BACKLIGHT;
  ExpanderWrite(0);
}

static void SendCommand(uint8_t cmd) {
  Send(cmd, 0);
}

static void SendChar(uint8_t ch) {
  Send(ch, RS);
}

static void Send(uint8_t value, uint8_t mode) {
  uint8_t highnib = value & 0xF0;
  uint8_t lownib = (value<<4) & 0xF0;
  Write4Bits((highnib)|mode);
  Write4Bits((lownib)|mode);
}

static void Write4Bits(uint8_t value) {
  ExpanderWrite(value);
  PulseEnable(value);
}

static void ExpanderWrite(uint8_t _data) {
  uint8_t data = _data | dpBacklight;
  HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1), (uint8_t*)&data, 1, 10);
}

static void PulseEnable(uint8_t _data) {
  ExpanderWrite(_data | ENABLE);
  DelayUS(20);

  ExpanderWrite(_data & ~ENABLE);
  DelayUS(20);
}

static void DelayInit(void) {
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;

  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

  DWT->CYCCNT = 0;

  /* 3 NO OPERATION instructions */
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
}

static void DelayUS(uint32_t us) {
  uint32_t cycles = (SystemCoreClock/1000000L)*us;
  uint32_t start = DWT->CYCCNT;
  volatile uint32_t cnt;

  do
  {
    cnt = DWT->CYCCNT - start;
  } while(cnt < cycles);
}