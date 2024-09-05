/**
 ******************************************************************************
 * @file          : vexuf_ttl.c
 * @brief        : VexUF TTL Implementation
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
#include "vexuf_ttl.h"

#include <ctype.h>
#include <stdlib.h>

#include "vexuf_serial.h"

/* TypeDef -------------------------------------------------------------------*/
extern UART_HandleTypeDef *ttlUart;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern SerialConfiguration serialConf;
extern char *ok;
extern char *no;
extern char *invalidBaud;

/* Variables -----------------------------------------------------------------*/

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/

void TTL_handleCli(const char *args, char *responseBuffer) {
  uint32_t baudInt = 0;
  SERIAL_baudToInt(serialConf.ttl_baud, &baudInt);

  if (args == NULL || strlen(args) == 0) {
    sprintf(responseBuffer, "TTL is currently %s%s",
            serialConf.ttl_enabled == 1 ? "Enabled" : "disabled", ok);
  } else {
    if (strncmp(args, "enable", 6) == 0) {
      serialConf.ttl_enabled = 1;
      sprintf(responseBuffer, "TTL enabled%s", ok);
    } else if (strncmp(args, "disable", 7) == 0) {
      serialConf.ttl_enabled = 0;
      sprintf(responseBuffer, "TTL disabled%s", ok);
    } else if (strncmp(args, "baud", 4) == 0) {
      if (serialConf.ttl_enabled == 0) {
        sprintf(responseBuffer, "TTL is currently disabled%s", no);
        return;
      }
      // Move the pointer past "baud" and any following space
      const char *baudArgs = args + 4;
      while (isspace((unsigned char)*baudArgs)) baudArgs++;

      if (*baudArgs == '\0') {
        // Getter: Show current baud rate
        uint32_t currentBaudInt;
        SERIAL_baudToInt(serialConf.ttl_baud, &currentBaudInt);
        sprintf(responseBuffer, "Current Baud Rate: %lu%s", currentBaudInt, ok);
      } else {
        // Setter: Set new baud rate
        uint32_t newBaudInt = atoi(baudArgs);
        if (newBaudInt == 300 || newBaudInt == 600 || newBaudInt == 1200 ||
            newBaudInt == 4800 || newBaudInt == 9600 || newBaudInt == 19200 ||
            newBaudInt == 57600 || newBaudInt == 115200) {
          SerialBaudRate newBaud;
          if (SERIAL_intToBaud(newBaudInt, &newBaud) != UF_OK) {
            sprintf(responseBuffer, "%s%s", invalidBaud, no);
            return;
          }
          serialConf.ttl_baud = newBaud;
          if (SERIAL_setBaudRate(ttlUart, newBaud) == UF_OK) {
            sprintf(responseBuffer, "Baud rate set to: %lu%s", newBaudInt, ok);
          } else {
            sprintf(responseBuffer, "Error: Could not set baud rate.%s", no);
          }
        } else {
          sprintf(responseBuffer, "%s%s", invalidBaud, no);
        }
      }
    } else {
      sprintf(responseBuffer,
              "Error with TTL command. use `help TTL` for help.%s", no);
    }
  }
}
/* Private Methods -----------------------------------------------------------*/