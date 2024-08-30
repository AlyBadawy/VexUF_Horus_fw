/**
 ******************************************************************************
 * @file          : vexuf.c
 * @brief        : VexUF Implementation
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
#include "vexuf.h"

#include <ctype.h>

#include "vexuf_config.h"

/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
static const char custom_base32_alphabet[] = "23456789ABCDEFGHJKLMNPQRTUVWXYZ";
char serialNumber[24];
char regNumber[REGISTRATION_NUMBER_LENGTH];
VexufStatus vexufStatus;

/* Prototypes ----------------------------------------------------------------*/
void base32_encode(const uint8_t *data, size_t length, char *output);

/* Code ----------------------------------------------------------------------*/

/* Private Methods -----------------------------------------------------------*/

extern UART_HandleTypeDef huart1;
int _write(int file, char *ptr, int len) {
  UNUSED(file);
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 200);
  return len;
}

uint16_t getSerialBytes(void) { return *(uint16_t *)0x08007FFD; }

void base32_encode(const uint8_t *data, size_t length, char *output) {
  int bitBuffer = 0, bitsInBuffer = 0;
  size_t index = 0;

  for (size_t i = 0; i < length; ++i) {
    bitBuffer = (bitBuffer << 8) | data[i];
    bitsInBuffer += 8;
    while (bitsInBuffer >= 5) {
      output[index++] =
          custom_base32_alphabet[(bitBuffer >> (bitsInBuffer - 5)) & 0x1F];
      bitsInBuffer -= 5;
    }
  }
  if (bitsInBuffer > 0) {
    output[index++] =
        custom_base32_alphabet[(bitBuffer << (5 - bitsInBuffer)) & 0x1F];
  }
  output[index] = '\0';
}

void VexUF_GenerateSerialNumber() {
  uint32_t uid[3] = {HAL_GetUIDw2(), HAL_GetUIDw1(), HAL_GetUIDw0()};
  uint8_t uid_bytes[12];
  char serial[20];

  for (int i = 0; i < 3; ++i) {
    uid_bytes[4 * i + 0] = (uid[i] >> 24) & 0xFF;
    uid_bytes[4 * i + 1] = (uid[i] >> 16) & 0xFF;
    uid_bytes[4 * i + 2] = (uid[i] >> 8) & 0xFF;
    uid_bytes[4 * i + 3] = uid[i] & 0xFF;
  }

  base32_encode(uid_bytes, 12, serial);

  int i = 0, j = 0;
  for (i = 0; serial[i] != '\0'; ++i) {
    if (i > 0 && i % 5 == 0) serialNumber[j++] = '-';
    serialNumber[j++] = serial[i];
  }
  serialNumber[j] = '\0';
}

void trim(char **str) {
  char *start = *str;
  char *end;

  // Skip leading spaces
  while (isspace((unsigned char)*start)) start++;

  // If the string is all spaces or empty
  if (*start == '\0') {
    *str = start;
    return;
  }

  // Find the last non-space character
  end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char)*end)) end--;

  // Set the character after the last non-space character to '\0'
  *(end + 1) = '\0';

  // Update the str pointer to the new start
  *str = start;
}