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

char *ok = "\r\nOk!";
char *no = "\r\nNo!";

/* Prototypes ----------------------------------------------------------------*/
void handle_temperature(const char *args);
void handle_time(const char *args);
void handle_buzzer(const char *args);
void handle_avs(const char *args);
void handle_tnc(const char *args);
void handle_pwm(const char *args);
void handle_help(const char *args);

/* Code ----------------------------------------------------------------------*/

const Command commands[] = {
    {"time", handle_time},                // CLI command to handle time
    {"temperature", handle_temperature},  // CLI command to handle temperature
    {"buzzer", handle_buzzer},            // CLI command to handle buzzer
    {"av", handle_avs},                   // CLI command to handle AVs
    {"tnc", handle_tnc},                  // CLI command to handle TNC
    {"pwm", handle_pwm},                  // CLI command to handle PWM
    {"help", handle_help}                 // CLI command to handle help
                                          // ... add more commands as needed ...
};

UF_STATUS CLI_init(UART_HandleTypeDef *ttl, UART_HandleTypeDef *tnc) {
  ttlUart = *ttl;
  tncUart = *tnc;

  if (HAL_UART_Transmit(&ttlUart, (uint8_t *)prompt, strlen(prompt), 200) !=
      HAL_OK)
    return UF_ERROR;
  if (HAL_UART_Transmit(&tncUart, (uint8_t *)prompt, strlen(prompt), 200) !=
      HAL_OK)
    return UF_ERROR;

  return UF_OK;
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
  uint8_t found = 0;
  for (uint8_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    if (strncmp(tempCommand, commands[i].command_name,
                strlen(commands[i].command_name)) == 0) {
      char *args = tempCommand + strlen(commands[i].command_name);
      args = trim(args);
      found = 1;
      commands[i].handler(args);
    }
  }
  if (found == 0 && strlen(tempCommand) > 0) {
    sprintf(serialTxBuffer,
            "Unknown command. Use 'help' to list all commands.%s", no);
  }

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
void handle_time(const char *args) { RTC_handleCli(args, serialTxBuffer); }
void handle_buzzer(const char *args) { BUZZ_handleCli(args, serialTxBuffer); }
void handle_avs(const char *args) { AVS_handleCli(args, serialTxBuffer); }
void handle_tnc(const char *args) { TNC_handleCli(args, serialTxBuffer); }
void handle_pwm(const char *args) { PWM_handleCli(args, serialTxBuffer); }
void handle_temperature(const char *args) {
  TEMPERATURE_handleCli(args, serialTxBuffer);
}

void handle_help(const char *args) {
  // Array of command details to provide specific help
  const char *command_help[] = {
      "Time: Manage date and time.\r\n"
      "  - Use 'Time' to display the current date and time.\r\n"
      "  - Use 'Time <date> <time>' to set the current date and time.\r\n"
      "    Example: 'Time 2024-09-02 14:30:00' sets the date and time.\r\n",

      "Temperature: Manage temperature sensors.\r\n"
      "  - Use 'Temperature' to get readings from all sensors.\r\n"
      "  - Use 'Temperature CPU' to get the CPU temperature.\r\n"
      "  - Use 'Temperature Internal' to get the internal sensor's temperature "
      "and humidity.\r\n"
      "  - Use 'Temperature External' to get the external I2C sensor's "
      "temperature and humidity.\r\n",

      "Buzzer: Control the buzzer functionality.\r\n"
      "  - Use 'Buzzer <enable|disable>' to enable or disable the buzzer.\r\n"
      "  - Use 'Buzzer start beep <on/off>' to enable/disable the start-up  "
      "beep.\r\n"
      "  - Use 'Buzzer error <on|off>' to enable/disable sounding the buzzer "
      "on "
      "errors.\r\n",

      "AV: Manage AV peripherals.\r\n"
      "  - Use 'AV' to show statuses of all AVs.\r\n"
      "  - Use 'AV <n>' where <n> is 1, 2, or 3 to get details about a "
      "specific AV.\r\n"
      "  - Use 'AV <n> <enable|disable>' to enable or disable a specific "
      "AV.\r\n"
      "  - Use 'AV <n> <slow|fast|on> <enable|disable>' to control LED "
      "blinking "
      "rules.\r\n"
      "  - Use 'AV <n> <slow|fast|on> <min> <max>' to set min and max values "
      "for "
      "blinking rules.\r\n",

      "TNC: Manage TNC.\r\n"
      "  - Use 'TNC callsign' to get current callsign.\r\n"
      "  - Use 'TNC callsign <new_callsign>' to set the callsign.\r\n"
      "  - Use 'TNC baud' to get current Baud rate for the TNC interface.\r\n"
      "  - Use 'TNC baud <baud_rate>' to set the Baud rate.\r\n"
      "    Available rates: 300, 600, 1200, 4800, 9600, 19200, 57600, "
      "115200.\r\n"
      "  - Use 'TNC message <n>' to get a message. <n> is between 0 to 9.\r\n"
      "  - Use 'TNC message <n> <message>' to set a message.\r\n"
      "  - Use 'TNC path <n>' to get a path. <n> is between 0 to 4.\r\n"
      "  - Use 'TNC path <n> <path>' to set a path.\r\n",

      "PWM: Control PWM channels.\r\n"
      "  - Use 'PWM' to display the status of both PWM channels.\r\n"
      "  - Use 'PWM 1' or 'PWM 2' to display the status of a specific PWM "
      "channel.\r\n"
      "  - Use 'PWM <n> <enable|disable>' to enable or "
      "disable a specific PWM channel. <n> should be 1 or 2.\r\n"
      "  - Use 'PWM <n> <value> to set the default value "
      "for the PWM channel. <value> should be between 0 and 999.\r\n",

      "Help: Shows the list of available commands or help for a specific "
      "command.\r\n"
      "  - Use 'help' to list all available commands.\r\n"
      "  - Use 'help <command>' to get detailed help for a specific "
      "command.\r\n"};

  // Check if args is empty
  if (strlen(args) == 0) {
    sprintf(serialTxBuffer, "\r\nAvailable commands:\r\n");
    for (uint8_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
      sprintf(serialTxBuffer + strlen(serialTxBuffer), "  - %s\r\n",
              commands[i].command_name);
    }
    sprintf(serialTxBuffer + strlen(serialTxBuffer),
            "\r\nUse 'help <command>' for more information.\r\n");
    sprintf(serialTxBuffer + strlen(serialTxBuffer), "\r\nOk!\r\n");
  } else {
    // Search for the command in the commands array
    uint8_t found = 0;
    for (uint8_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
      if (strcasecmp(args, commands[i].command_name) == 0) {
        // Found the command, provide detailed help
        sprintf(serialTxBuffer, "\r\nHelp for '%s':\r\n%s\r\n",
                commands[i].command_name, command_help[i]);
        found = 1;
        break;
      }
    }
    if (found == 0) {
      // Command not found
      sprintf(serialTxBuffer,
              "Unknown command '%s'. Use 'help' to list all commands.\r\n",
              args);
    }
  }
}