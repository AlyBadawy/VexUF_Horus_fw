#include "vexuf_cli.h"

#include <ctype.h>
#include <string.h>

#include "aht20.h"
#include "vexuf_buzzer.h"
#include "vexuf_config.h"
#include "vexuf_rtc.h"
#include "vexuf_temperature.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

void handle_get_callsign(const char *args);
void handle_set_callsign(const char *args);
void handle_get_temperature(const char *args);
void handle_get_time(const char *args);
void handle_set_time(const char *args);
void handle_buzzer(const char *args);

extern unsigned char ttlRxData[SERIAL_BUFFER_SIZE];
extern unsigned char tncRxData[SERIAL_BUFFER_SIZE];
extern uint16_t ttlRxIdx;
extern uint16_t tncRxIdx;

extern OutputConfiguration outputConfig;
extern IndConfiguration indConf;

static char serialTxBuffer[SERIAL_BUFFER_SIZE];

static char *prompt = "\r\nVexUF:Horus > ";
static char *ok = "\r\nOk!";
static char *no = "\r\nNo!";
static char *invalid =
    "Invalid Command!\r\nType 'help' or '?' for list of commands.";

const Command commands[] = {
    {"get callsign", handle_get_callsign},
    {"set callsign", handle_set_callsign},
    {"get temperature", handle_get_temperature},
    {"get time", handle_get_time},
    {"set time", handle_set_time},
    {"buzzer", handle_buzzer}
    // ... add more commands as needed ...
};

UF_STATUS CLI_handleCommand(SerialInterface interface) {
  char command[SERIAL_BUFFER_SIZE];
  unsigned char *rxData;
  uint16_t *rxIdx;
  UART_HandleTypeDef *uartHandle;

  memset(serialTxBuffer, 0, sizeof(serialTxBuffer));
  memset(command, 0, sizeof(command));

  switch (interface) {
    case TtlUart:
      rxData = ttlRxData;
      rxIdx = &ttlRxIdx;
      break;
    case TncUart:
      rxData = tncRxData;
      rxIdx = &tncRxIdx;
      break;
    default:
      return UF_ERROR;
  }
  while (rxData[*rxIdx - 1] == '\r' || rxData[*rxIdx - 1] == '\n')
    rxData[--(*rxIdx)] = '\0';

  memcpy(command, rxData, *rxIdx + 1);

  char tempCommand[SERIAL_BUFFER_SIZE];
  for (int i = 0; command[i] != '\0'; i++) {
    tempCommand[i] = tolower(command[i]);
  }

  for (uint8_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    if (strncmp(tempCommand, commands[i].command_name,
                strlen(commands[i].command_name)) == 0) {
      char *args = command + strlen(commands[i].command_name);
      trim(&args);
      commands[i].handler(args);
    }
  }
  // TODO: handle unknown commands

  switch (interface) {
    case TtlUart:
      uartHandle = &huart1;
      break;
    case TncUart:
      uartHandle = &huart6;
      break;
    default:
      return UF_ERROR;
  }

  if (strlen(serialTxBuffer) > 0) {
    HAL_UART_Transmit_DMA(uartHandle, (uint8_t *)serialTxBuffer,
                          strlen(serialTxBuffer));
    HAL_Delay(100);
  }
  HAL_UART_Transmit_DMA(uartHandle, (uint8_t *)prompt, strlen(prompt));
  return UF_OK;
}

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

void handle_get_temperature(const char *args) {
  float temp, humid;

  if (strlen(args) == 0) {
    // TODO: logic to show all temperatures
  } else if ((strncmp(args, "internal", 8)) == 0) {
    AHT20_ReadTemperatureHumidity(&temp, &humid);
    sprintf(serialTxBuffer,
            "Temperature (Internal): %0.2fC (%0.2fF) - Humidity: %0.2f%%%s",
            temp, TEMPERATURE_cToF(temp), humid, ok);
  } else if ((strncmp(args, "external", 8)) == 0) {
    // TODO: implement external temperature
  } else if ((strncmp(args, "cpu", 3)) == 0) {
    float cpuTempC;
    TEMPERATURE_getCpuTempC(&cpuTempC);  // todo: implement this function
    sprintf(serialTxBuffer, "Temperature (CPU): %0.2fC (%0.2fF)%s", cpuTempC,
            TEMPERATURE_cToF(cpuTempC), ok);
  }
}

void handle_get_callsign(const char *args) {
  UNUSED(args);
  char callsign[CALLSIGN_LENGTH];
  CONFIG_getCallSign(callsign);
  sprintf(serialTxBuffer, "Callsign: %s%s", callsign, ok);
}
void handle_set_callsign(const char *args) {
  CONFIG_setCallSign(args);
  sprintf(serialTxBuffer, "%s", ok);
}

void handle_buzzer(const char *args) { BUZZ_handleCli(args, serialTxBuffer); }