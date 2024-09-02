/**
 ******************************************************************************
 * @file          : vexuf_cli.c
 * @brief        : VexUF CLI Implementation
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
#include "vexuf_cli.h"

#include <ctype.h>
#include <string.h>

#include "aht20.h"
#include "vexuf_avs.h"
#include "vexuf_buzzer.h"
#include "vexuf_config.h"
#include "vexuf_rtc.h"
#include "vexuf_temperature.h"

/* TypeDef -------------------------------------------------------------------*/
static UART_HandleTypeDef ttlUart;
static UART_HandleTypeDef tncUart;

typedef void (*CommandHandler)(const char *args);

typedef struct {
  const char *command_name;
  CommandHandler handler;
} Command;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern char ttlRxData[SERIAL_BUFFER_SIZE];
extern char tncRxData[SERIAL_BUFFER_SIZE];
extern uint16_t ttlRxIdx;
extern uint16_t tncRxIdx;
extern SerialConfiguration serialConf;
extern OutputConfiguration outputConfig;
extern IndConfiguration indConf;

/* Variables -----------------------------------------------------------------*/
static char serialTxBuffer[SERIAL_BUFFER_SIZE];
static char *prompt = "\r\nVexUF:Horus >";
static char *ok = "\r\nOk!";

/* Prototypes ----------------------------------------------------------------*/
void handle_get_callsign(const char *args);
void handle_set_callsign(const char *args);
void handle_get_temperature(const char *args);
void handle_get_time(const char *args);
void handle_set_time(const char *args);
void handle_buzzer(const char *args);
void handle_avs(const char *args);

/* Code ----------------------------------------------------------------------*/

const Command commands[] = {
    {"get callsign", handle_get_callsign},
    {"set callsign", handle_set_callsign},
    {"get temperature", handle_get_temperature},
    {"get time", handle_get_time},
    {"set time", handle_set_time},
    {"buzzer", handle_buzzer},
    {"av", handle_avs},
    // ... add more commands as needed ...
};

void CLI_init(UART_HandleTypeDef *ttl, UART_HandleTypeDef *tnc) {
  ttlUart = *ttl;
  tncUart = *tnc;
}

UF_STATUS CLI_handleCommand(const SerialInterface interface) {
  char command[SERIAL_BUFFER_SIZE];
  char *rxData;
  uint16_t *rxIdx;
  UART_HandleTypeDef *uartHandle;

  switch (interface) {
    case TTL:
      if (serialConf.ttl_enabled != 1) return UF_DISABLED;
      rxData = ttlRxData;
      rxIdx = &ttlRxIdx;
      break;
    case TNC:
      if (serialConf.tnc_enabled != 1) return UF_DISABLED;
      rxData = tncRxData;
      rxIdx = &tncRxIdx;
      break;
    default:
      return UF_ERROR;
  }

  memset(serialTxBuffer, 0, sizeof(serialTxBuffer));

  while (rxData[*rxIdx - 1] == '\r' || rxData[*rxIdx - 1] == '\n')
    rxData[--(*rxIdx)] = '\0';

  memset(command, 0, sizeof(command));
  memcpy(command, rxData, *rxIdx + 1);

  char tempCommand[SERIAL_BUFFER_SIZE];
  memset(tempCommand, 0, sizeof(command));
  for (int i = 0; command[i] != '\0'; i++) {
    tempCommand[i] = tolower(command[i]);
  }

  for (uint8_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    if (strncmp(tempCommand, commands[i].command_name,
                strlen(commands[i].command_name)) == 0) {
      char *args = tempCommand + strlen(commands[i].command_name);
      trim(&args);
      commands[i].handler(args);
    }
  }
  // TODO: handle unknown commands

  switch (interface) {
    case TTL:
      uartHandle = &ttlUart;
      memset(ttlRxData, 0, sizeof(ttlRxData));
      break;
    case TNC:
      uartHandle = &tncUart;
      memset(tncRxData, 0, sizeof(tncRxData));
      break;
    default:
      return UF_ERROR;
  }

  if (strlen(serialTxBuffer) > 0) {
    if (HAL_UART_Transmit(uartHandle, (uint8_t *)serialTxBuffer,
                          strlen(serialTxBuffer), 200) != HAL_OK)
      Error_Handler();

    HAL_Delay(100);
  }
  if (HAL_UART_Transmit(uartHandle, (uint8_t *)prompt, strlen(prompt), 200) !=
      HAL_OK)
    Error_Handler();
  return UF_OK;
}

/* Private Methods -----------------------------------------------------------*/

void handle_get_time(const char *args) {
  UNUSED(args);
  char datetime[20];
  RTC_getDateTime(datetime);
  sprintf(serialTxBuffer, "Date/Time: %s%s", datetime, ok);
}
void handle_set_time(const char *args) {
  RTC_setDateTime(args);
  sprintf(serialTxBuffer, "Date and Time set...%s", ok);
}

void handle_get_callsign(const char *args) {
  UNUSED(args);
  char callsignBuffer[CALLSIGN_LENGTH];
  CONFIG_getCallSign(callsignBuffer);
  sprintf(serialTxBuffer, "Callsign: %s%s", callsignBuffer, ok);
}
void handle_set_callsign(const char *args) {
  CONFIG_setCallSign(args);
  sprintf(serialTxBuffer, "%s", ok);
}

void handle_buzzer(const char *args) { BUZZ_handleCli(args, serialTxBuffer); }
void handle_avs(const char *args) { AVS_handleCli(args, serialTxBuffer); }
void handle_get_temperature(const char *args) {
  TEMPERATURE_handleCli(args, serialTxBuffer);
}