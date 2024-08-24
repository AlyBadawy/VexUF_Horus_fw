/*
 * hd44780.c
 *
 *  Created on: Aug 18, 2024
 *      Author: Aly Badawy
 */

#include "hd44780.h"

uint8_t dpFunction;
uint8_t dpControl;
uint8_t dpMode;
uint8_t dpRows;
uint8_t dpBacklight;

static void SendCommand(I2C_HandleTypeDef *hi2c, uint8_t);
static void SendChar(I2C_HandleTypeDef *hi2c, uint8_t);
static void Send(I2C_HandleTypeDef *hi2c, uint8_t, uint8_t);
static void Write4Bits(I2C_HandleTypeDef *hi2c, uint8_t);
static void ExpanderWrite(I2C_HandleTypeDef *hi2c, uint8_t);
static void PulseEnable(I2C_HandleTypeDef *hi2c, uint8_t);
static void DelayInit(void);
static void DelayUS(uint32_t);

uint8_t one[8] = {0x1f, 0x1b, 0x13, 0x1b, 0x1b, 0x1b, 0x11, 0x1f};
uint8_t two[8] = {0x1f, 0x1b, 0x15, 0x1d, 0x1b, 0x17, 0x11, 0x1f};
uint8_t three[8] = {0x1f, 0x11, 0x1d, 0x11, 0x1d, 0x1d, 0x11, 0x1f};

uint8_t degree[8] = { 0x08, 0x14, 0x08, 0x06, 0x09, 0x08, 0x09, 0x06 };
uint8_t bell[8] = {0x04, 0x0e, 0x0e, 0x0e, 0x1f, 0x00, 0x04, 0x00};


void HD44780_Init(I2C_HandleTypeDef *hi2c, uint8_t rows)
{
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

  ExpanderWrite(hi2c, dpBacklight);
  HAL_Delay(1000);

  /* 4bit Mode */
  Write4Bits(hi2c, 0x03 << 4);
  DelayUS(4500);

  Write4Bits(hi2c, 0x03 << 4);
  DelayUS(4500);

  Write4Bits(hi2c, 0x03 << 4);
  DelayUS(4500);

  Write4Bits(hi2c, 0x02 << 4);
  DelayUS(100);

  /* Display Control */
  SendCommand(hi2c, HD44780_FUNCTIONSET | dpFunction);

  dpControl = HD44780_DISPLAYON | HD44780_CURSOROFF | HD44780_BLINKOFF;
  HD44780_Display(hi2c);
  HD44780_Clear(hi2c);

  /* Display Mode */
  dpMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
  SendCommand(hi2c, HD44780_ENTRYMODESET | dpMode);
  DelayUS(4500);

  HD44780_CreateSpecialChar(hi2c, 0, one);
  HD44780_CreateSpecialChar(hi2c, 1, two);
  HD44780_CreateSpecialChar(hi2c, 2, three);
  HD44780_CreateSpecialChar(hi2c, 3, bell);
  HD44780_CreateSpecialChar(hi2c, 4, degree);

  HD44780_Home(hi2c);
}

void HD44780_Clear(I2C_HandleTypeDef *hi2c)
{
  SendCommand(hi2c, HD44780_CLEARDISPLAY);
  DelayUS(2000);
}

void HD44780_Home(I2C_HandleTypeDef *hi2c)
{
  SendCommand(hi2c, HD44780_RETURNHOME);
  DelayUS(2000);
}

void HD44780_SetCursor(I2C_HandleTypeDef *hi2c, uint8_t row, uint8_t col)

{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if (row >= dpRows)
  {
    row = dpRows-1;
  }
  SendCommand(hi2c, HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

void HD44780_NoDisplay(I2C_HandleTypeDef *hi2c)
{
  dpControl &= ~HD44780_DISPLAYON;
  SendCommand(hi2c, HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_Display(I2C_HandleTypeDef *hi2c)
{
  dpControl |= HD44780_DISPLAYON;
  SendCommand(hi2c, HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_NoCursor(I2C_HandleTypeDef *hi2c)
{
  dpControl &= ~HD44780_CURSORON;
  SendCommand(hi2c, HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_Cursor(I2C_HandleTypeDef *hi2c)
{
  dpControl |= HD44780_CURSORON;
  SendCommand(hi2c, HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_NoBlink(I2C_HandleTypeDef *hi2c)
{
  dpControl &= ~HD44780_BLINKON;
  SendCommand(hi2c, HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_Blink(I2C_HandleTypeDef *hi2c)
{
  dpControl |= HD44780_BLINKON;
  SendCommand(hi2c, HD44780_DISPLAYCONTROL | dpControl);
}

void HD44780_ScrollDisplayLeft(I2C_HandleTypeDef *hi2c)
{
  SendCommand(hi2c, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void HD44780_ScrollDisplayRight(I2C_HandleTypeDef *hi2c)
{
  SendCommand(hi2c, HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void HD44780_LeftToRight(I2C_HandleTypeDef *hi2c)
{
  dpMode |= HD44780_ENTRYLEFT;
  SendCommand(hi2c, HD44780_ENTRYMODESET | dpMode);
}

void HD44780_RightToLeft(I2C_HandleTypeDef *hi2c)
{
  dpMode &= ~HD44780_ENTRYLEFT;
  SendCommand(hi2c, HD44780_ENTRYMODESET | dpMode);
}

void HD44780_AutoScroll(I2C_HandleTypeDef *hi2c)
{
  dpMode |= HD44780_ENTRYSHIFTINCREMENT;
  SendCommand(hi2c, HD44780_ENTRYMODESET | dpMode);
}

void HD44780_NoAutoScroll(I2C_HandleTypeDef *hi2c)
{
  dpMode &= ~HD44780_ENTRYSHIFTINCREMENT;
  SendCommand(hi2c, HD44780_ENTRYMODESET | dpMode);
}

void HD44780_CreateSpecialChar(I2C_HandleTypeDef *hi2c, uint8_t location, uint8_t charmap[])
{
  location &= 0x7;
  SendCommand(hi2c, HD44780_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++)
  {
    SendChar(hi2c, charmap[i]);
  }
}

void HD44780_PrintSpecialChar(I2C_HandleTypeDef *hi2c, uint8_t index)
{
  SendChar(hi2c, index);
}

void HD44780_LoadCustomCharacter(I2C_HandleTypeDef *hi2c, uint8_t char_num, uint8_t *rows)
{
  HD44780_CreateSpecialChar(hi2c, char_num, rows);
}

void HD44780_PrintStr(I2C_HandleTypeDef *hi2c, const char c[])
{
  while(*c) SendChar(hi2c, *c++);
}

void HD44780_SetBacklight(I2C_HandleTypeDef *hi2c, uint8_t new_val)
{
  if(new_val) HD44780_Backlight(hi2c);
  else HD44780_NoBacklight(hi2c);
}

void HD44780_NoBacklight(I2C_HandleTypeDef *hi2c)
{
  dpBacklight=HD44780_NOBACKLIGHT;
  ExpanderWrite(hi2c, 0);
}

void HD44780_Backlight(I2C_HandleTypeDef *hi2c)
{
  dpBacklight=HD44780_BACKLIGHT;
  ExpanderWrite(hi2c, 0);
}

static void SendCommand(I2C_HandleTypeDef *hi2c, uint8_t cmd)
{
  Send(hi2c, cmd, 0);
}

static void SendChar(I2C_HandleTypeDef *hi2c, uint8_t ch)
{
  Send(hi2c, ch, RS);
}

static void Send(I2C_HandleTypeDef *hi2c, uint8_t value, uint8_t mode)
{
  uint8_t highnib = value & 0xF0;
  uint8_t lownib = (value<<4) & 0xF0;
  Write4Bits(hi2c, (highnib)|mode);
  Write4Bits(hi2c, (lownib)|mode);
}

static void Write4Bits(I2C_HandleTypeDef *hi2c, uint8_t value)
{
  ExpanderWrite(hi2c, value);
  PulseEnable(hi2c, value);
}

static void ExpanderWrite(I2C_HandleTypeDef *hi2c, uint8_t _data)
{
  uint8_t data = _data | dpBacklight;
  HAL_I2C_Master_Transmit(hi2c, (0x27 << 1), (uint8_t*)&data, 1, 10);
}

static void PulseEnable(I2C_HandleTypeDef *hi2c, uint8_t _data)
{
  ExpanderWrite(hi2c, _data | ENABLE);
  DelayUS(20);

  ExpanderWrite(hi2c, _data & ~ENABLE);
  DelayUS(20);
}

static void DelayInit(void)
{
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