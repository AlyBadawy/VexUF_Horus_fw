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

#include "93c86.h"
#include "vexuf_config.h"

/* TypeDef -------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern char *ok;
extern char *no;
extern uint16_t configVersion, configCount;

/* Variables -----------------------------------------------------------------*/
static const char *major = "0";
static const char *minor = "8";
static const char *patch = "7";

static const char custom_base32_alphabet[] = "23456789ABCDEFGHJKLMNPQRTUVWXYZ";
char regNumber[REGISTRATION_NUMBER_LENGTH];
VexufStatus vexufStatus;

/* Prototypes ----------------------------------------------------------------*/
void base32_encode(const uint8_t *data, size_t length, char *output);

/* Code ----------------------------------------------------------------------*/
int _write(int file, char *ptr, int len) {
  UNUSED(file);
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 200);
  return len;
}

uint16_t getSerialBytes(void) { return *(uint16_t *)0x08007FFD; }

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

void VexUF_handleCliInfo(const char *args, char *responseBuffer) {
  if (strlen(args) == 0) {
    static char serialNumber[SERIAL_NUMBER_LENGTH];
    VexUF_GenerateSerialNumber(serialNumber);

    uint16_t vexufSerial = getSerialBytes();

    char *status =
        vexufStatus.isConfigured == 1 ? "Configured" : "Not Configured";

    if (vexufStatus.isConfigured == 1) CONFIG_loadConfigValues();

    sprintf(responseBuffer,
            "Information: \r\n"
            "  Serial Number       : %s\r\n"
            "  VexUF FW Version    : %s.%s.%s\r\n"
            "  VexUF               : %04X\r\n"
            "  Registration Number : %s\r\n"
            "  Config Status       : %s\r\n"
            "  Config Version      : %d\r\n"
            "  Config Count        : %d"
            "%s",
            serialNumber, major, minor, patch, vexufSerial, regNumber, status,
            configVersion, configCount, ok);
  } else if (strncmp(args, "dump", 4) == 0) {
    if (strlen(args + 4) == 0) {
      sprintf(responseBuffer,
              "Missing region number. use 'Info dump <n>' where n is between 1 "
              "and 5%s",
              no);
      return;
    } else {
      const char *buffer = trim((char *)args + 4);
      uint8_t region = atoi(buffer);
      if (region >= 1 && region <= 5) {
        uint16_t startAddress = 0x0000;
        uint16_t endAddress = 0x0000;
        switch (region) {
          case 1:
            startAddress = 0x0000;
            endAddress = 0x00DF;  // 14 * 16 = 224 address. 448 bytes
            break;
          case 2:
            startAddress = 0x00E0;
            endAddress = 0x01CF;  // 15 * 16 = 240 address. 480 bytes
            break;
          case 3:
            startAddress = 0x01D0;
            endAddress = 0x026F;  // 10 * 16 = 160 address. 320 bytes
            break;
          case 4:
            startAddress = 0x0270;
            endAddress = 0x03AF;  // 20 * 16 = 160 address. 640 bytes
            break;
          case 5:
            startAddress = 0x03B0;
            endAddress = 0x03FF;  // 10 * 16 = 160 address. 320 bytes
            break;
        }

        uint16_t regionBuffer[0x3FF];
        for (uint16_t address = startAddress; address <= endAddress;
             address++) {
          EEPROM_93C86_Read(address, &regionBuffer[address]);
        }
        for (uint16_t address = startAddress; address <= endAddress;
             address += 16) {
          sprintf(responseBuffer + strlen(responseBuffer),
                  " %04X:   ", address);
          // Print first column (8 words)
          for (uint16_t i = 0; i < 16; i++) {
            sprintf(responseBuffer + strlen(responseBuffer), "%04X ",
                    regionBuffer[address + i]);
          }
          // New line
          sprintf(responseBuffer + strlen(responseBuffer), "\r\n");
        }
        sprintf(responseBuffer + strlen(responseBuffer), "%s", ok);
      } else {
        sprintf(
            responseBuffer,
            "Invalid region number. use 'Info dump <n>' where n is between 1 "
            "and 5%s",
            no);
      }
    }
  } else {
    sprintf(responseBuffer,
            "Error with Info command. Please use `help Info` for "
            "help.%s",
            no);
  }
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

/* Private Methods -----------------------------------------------------------*/
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
