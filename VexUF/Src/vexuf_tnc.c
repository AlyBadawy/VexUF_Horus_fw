/**
 ******************************************************************************
 * @file          : vexuf_tnc.c
 * @brief        : VexUF TNC Implementation
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
#include "vexuf_tnc.h"

#include <ctype.h>
#include <stdlib.h>

#include "vexuf_config.h"
#include "vexuf_serial.h"

/* TypeDef -------------------------------------------------------------------*/
static UART_HandleTypeDef *tncUart;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern SerialConfiguration serialConf;
extern char *ok;
extern char *no;

/* Variables -----------------------------------------------------------------*/
char callsign[CALLSIGN_LENGTH];
char tncMessages[TNC_MESSAGE_COUNT][TNC_MESSAGE_LENGTH];
char tncPaths[TNC_PATH_COUNT][TNC_PATH_LENGTH];

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
UF_STATUS TNC_init(UART_HandleTypeDef *tnc) {
  tncUart = tnc;
  return UF_OK;
}

void TNC_handleCli(const char *args, char *responseBuffer) {
  static char *invalidBaud =
      "Invalid baud rate.\r\n Allowed rates are: 300, 600, 1200, 4800, "
      "9600, 19200, 57600, or 115200.";

  uint32_t baudInt = 0;
  SERIAL_baudToInt(serialConf.tnc__baud, &baudInt);

  char tncEnabled[70];
  sprintf(tncEnabled, "enabled. \r\n  Callsign: %s\r\n  Baud    : %lu",
          callsign, baudInt);

  if (args == NULL || strlen(args) == 0) {
    sprintf(responseBuffer, "TNC is currently %s%s",
            serialConf.tnc_enabled == 1 ? tncEnabled : "disabled", ok);
  } else {
    if (strncmp(args, "enable", 6) == 0) {
      serialConf.tnc_enabled = 1;
      sprintf(responseBuffer, "TNC enabled%s", ok);
    } else if (strncmp(args, "disable", 7) == 0) {
      serialConf.tnc_enabled = 0;
      sprintf(responseBuffer, "TNC disabled%s", ok);
    } else if (strncmp(args, "callsign", 8) == 0) {
      if (serialConf.tnc_enabled == 0) {
        sprintf(responseBuffer, "TNC is currently disabled%s", no);
        return;
      }
      // Move the pointer past "callsign" and any following space
      const char *callsignArgs = args + 8;
      while (isspace((unsigned char)*callsignArgs)) callsignArgs++;

      if (*callsignArgs == '\0') {
        // Getter: Show current callsign
        sprintf(responseBuffer, "Current Callsign: %s%s", callsign, ok);
      } else {
        // Setter: Set new callsign
        if (strlen(callsignArgs) <= CALLSIGN_LENGTH) {
          memset(callsign, 0, CALLSIGN_LENGTH);
          memcpy(callsign, callsignArgs, strlen(callsignArgs));
          sprintf(responseBuffer, "Callsign set to: %s%s", callsignArgs, ok);
        } else {
          sprintf(responseBuffer, "Error: Callsign is too long.%s", no);
        }
      }
    } else if (strncmp(args, "baud", 4) == 0) {
      if (serialConf.tnc_enabled == 0) {
        sprintf(responseBuffer, "TNC is currently disabled%s", no);
        return;
      }
      // Move the pointer past "baud" and any following space
      const char *baudArgs = args + 4;
      while (isspace((unsigned char)*baudArgs)) baudArgs++;

      if (*baudArgs == '\0') {
        // Getter: Show current baud rate
        uint32_t currentBaudInt;
        SERIAL_baudToInt(serialConf.tnc__baud, &currentBaudInt);
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
          serialConf.tnc__baud = newBaud;
          if (SERIAL_setBaudRate(tncUart, newBaud) == UF_OK) {
            sprintf(responseBuffer, "Baud rate set to: %lu%s", newBaudInt, ok);
          } else {
            sprintf(responseBuffer, "Error: Could not set baud rate.%s", no);
          }
        } else {
          sprintf(responseBuffer, "%s%s", invalidBaud, no);
        }
      }
    } else if (strncmp(args, "message", 7) == 0) {
      if (serialConf.tnc_enabled == 0) {
        sprintf(responseBuffer, "TNC is currently disabled%s", no);
        return;
      }
      // Move the pointer past "message" and any following space
      const char *messageArgs = args + 7;
      while (isspace((unsigned char)*messageArgs)) messageArgs++;

      if (*messageArgs == '\0') {
        sprintf(responseBuffer,
                "Error: No message number provided. Use 'TNC message [0-9]' "
                "to get or set a message.%s",
                no);
      } else {
        int messageNumber = atoi(messageArgs);
        if (messageNumber < 0 || messageNumber > TNC_MESSAGE_COUNT - 1) {
          sprintf(responseBuffer,
                  "Error: Message number out of range. Use 'TNC message [0-9]' "
                  "to get or set a message.%s",
                  no);
        } else {
          // Move the pointer past the message number and any following space
          const char *message = messageArgs + 1;
          while (isspace((unsigned char)*message)) message++;

          if (*message == '\0') {
            // Getter: Show current message
            sprintf(responseBuffer, "Message %d: %s%s", messageNumber,
                    tncMessages[messageNumber], ok);
          } else {
            // Setter: Set new message
            if (strlen(message) < TNC_MESSAGE_LENGTH) {
              memset(tncMessages[messageNumber], 0, TNC_MESSAGE_LENGTH);
              memcpy(tncMessages[messageNumber], message, strlen(message));
              sprintf(responseBuffer, "Message %d set to: %s%s", messageNumber,
                      message, ok);
            } else {
              sprintf(responseBuffer,
                      "Error: Message is too long. Maximum 64 character.%s",
                      no);
            }
          }
        }
      }
    } else if (strncmp(args, "path", 4) == 0) {
      if (serialConf.tnc_enabled == 0) {
        sprintf(responseBuffer, "TNC is currently disabled%s", no);
        return;
      }
      // Move the pointer past "path" and any following space
      const char *pathArgs = args + 4;
      while (isspace((unsigned char)*pathArgs)) pathArgs++;

      if (*pathArgs == '\0') {
        sprintf(responseBuffer,
                "Error: No path number provided. Use 'TNC path [0-4]' to get "
                "or set a path.%s",
                no);
      } else {
        int pathNumber = atoi(pathArgs);
        if (pathNumber < 0 || pathNumber > TNC_PATH_COUNT - 1) {
          sprintf(
              responseBuffer,
              "Error: Path number out of range. Use 'TNC path [0-4]' to get "
              "or set a path.%s",
              no);
        } else {
          // Move the pointer past the path number and any following space
          const char *path = pathArgs + 1;
          while (isspace((unsigned char)*path)) path++;

          if (*path == '\0') {
            // Getter: Show current path
            sprintf(responseBuffer, "Path %d: %s%s", pathNumber,
                    tncPaths[pathNumber], ok);
          } else {
            // Setter: Set new path
            if (strlen(path) < TNC_PATH_LENGTH) {
              memset(tncPaths[pathNumber], 0, TNC_PATH_LENGTH);
              memcpy(tncPaths[pathNumber], path, strlen(path));
              sprintf(responseBuffer, "Path %d set to: %s%s", pathNumber, path,
                      ok);
            } else {
              sprintf(responseBuffer,
                      "Error: Path is too long. Maximum 32 character.%s", no);
            }
          }
        }
      }
    } else {
      sprintf(responseBuffer,
              "Error with TNC command. use `help TNC` for help.%s", no);
    }
  }
}

/* Private Methods
 * -----------------------------------------------------------*/
