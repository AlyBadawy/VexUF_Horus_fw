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
#include <stdlib.h>

#include "vexuf_config.h"

/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
static const char custom_base32_alphabet[] = "23456789ABCDEFGHJKLMNPQRTUVWXYZ";
// char serialNumber[24];
uint32_t regNumber;

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

void VexUF_GenerateSerialNumber(char *serialNumberString) {
  uint32_t uid[3] = {HAL_GetUIDw2(), HAL_GetUIDw1(), HAL_GetUIDw0()};
  uint8_t uid_bytes[12];
  char serial[24];

  for (int i = 0; i < 3; ++i) {
    uid_bytes[4 * i + 0] = (uid[i] >> 24) & 0xFF;
    uid_bytes[4 * i + 1] = (uid[i] >> 16) & 0xFF;
    uid_bytes[4 * i + 2] = (uid[i] >> 8) & 0xFF;
    uid_bytes[4 * i + 3] = uid[i] & 0xFF;
  }

  base32_encode(uid_bytes, 12, serial);

  int i = 0, j = 0;
  for (i = 0; serial[i] != '\0'; ++i) {
    if (i > 0 && i % 5 == 0) serialNumberString[j++] = '-';
    serialNumberString[j++] = serial[i];
  }
  serialNumberString[j] = '\0';
}

char *trim(const char *str) {
  // Check if the string is NULL or empty
  if (!str || *str == '\0') {
    return (char *)str;
  }

  // Pointer to the start of the string
  const char *start = str;

  // Skip leading spaces
  while (*start && isspace((unsigned char)*start)) start++;

  // If the string is all spaces or empty
  if (*start == '\0') {
    return (char *)start;  // Return pointer to the end of the string
  }

  // Find the last non-space character
  const char *end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char)*end)) end--;

  // Calculate the length of the trimmed string
  size_t length = end - start + 1;

  // Create a static buffer to hold the trimmed result
  static char trimmedBuffer[1024];
  if (length >= sizeof(trimmedBuffer)) {
    length = sizeof(trimmedBuffer) - 1;  // Ensure it fits in the buffer
  }

  // Copy the trimmed content into the buffer
  strncpy(trimmedBuffer, start, length);
  trimmedBuffer[length] = '\0';  // Null-terminate the trimmed string

  return trimmedBuffer;
}