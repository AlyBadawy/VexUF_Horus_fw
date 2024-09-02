/**
 ******************************************************************************
 * @file          : vexuf_avs.c
 * @brief        : VexUF AVs Implementation
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
#include "vexuf_avs.h"

#include <stdlib.h>

#include "vexuf.h"
#include "vexuf_adc.h"
#include "vexuf_config.h"
#include "vexuf_indicators.h"

/* TypeDef -------------------------------------------------------------------*/
typedef enum {
  OPTION_SUCCESS,
  OPTION_ERROR_INVALID_FORMAT,
  OPTION_ERROR_OUT_OF_RANGE,
  OPTION_ERROR_MISSING_VALUES
} OptionStatus;

/* Defines -------------------------------------------------------------------*/
#define adcR1 33000.0
#define adcR2 3300.0
#define adcRatio (adcR2 / (adcR1 + adcR2))

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern IndConfiguration indConf;

/* Variables -----------------------------------------------------------------*/
float AVsVoltages[NUMBER_OF_AVS];
uint32_t AVsRawValues[NUMBER_OF_AVS];
AvSensor avSensors[NUMBER_OF_AVS];

static char *ok = "\r\nOk!";
static char *no = "\r\nNo!";

/* Prototypes ----------------------------------------------------------------*/
UF_STATUS AVS_rawToVoltage(float vref, uint32_t adcValue, float *voltValue);
void handleCliForAv(uint8_t idx, const char *args, char *responseBuffer);
OptionStatus extractAvStatus(char *args, uint16_t *min, uint16_t *max);
void setResponseBuffer(char *responseBuffer, OptionStatus status);

/* Code ----------------------------------------------------------------------*/
UF_STATUS AVS_Init(void) {
  for (uint8_t i = 0; i < NUMBER_OF_AVS; i++) {
    if (CONFIG_getAvSensor(&avSensors[i], i) != UF_OK) return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS AVS_Scan(void) {
  float vref;
  Indicator ind;
  AvSensor av;
  uint32_t value;
  uint32_t adcBuffer[5];
  IndLevelOption option = IndOFF;

  if (ADC_run(adcBuffer, &vref) == UF_ERROR) return UF_ERROR;

  for (uint8_t i = 0; i < NUMBER_OF_AVS; i++) {
    ind = IndAv1 + i;
    value = 0;
    AVsRawValues[i] = 0;
    memcpy(&av, &avSensors[i], sizeof(AvSensor));

    if (av.enabled != 1) {
      IND_setLevel(ind, IndOFF);
      continue;
    }

    value = adcBuffer[2 + i];
    if (AVS_rawToVoltage(vref, adcBuffer[2 + i], &AVsVoltages[i]) == UF_ERROR)
      return UF_ERROR;

    // TODO: Skip if indicator for that AV is disabled
    if (av.statusSlow && (value >= av.minSlow && value <= av.maxSlow)) {
      option = IndSLOW;
    } else if (av.statusFast && (value >= av.minFast && value <= av.maxFast)) {
      option = IndFAST;
    } else if (av.statusOn && (value >= av.minOn && value <= av.maxOn)) {
      option = IndON;
    }
    IND_setLevel(ind, option);

    // TODO: Display output on LCD
    // TODO: Log the output to Logger
  }
  return UF_OK;
}

void AVS_handleCli(const char *args, char *responseBuffer) {
  if (strlen(args) == 0) {
    // TODO: logic to show all AVs
  } else if ((strncmp(args, "1", 1)) == 0 || (strncmp(args, "2", 1)) == 0 ||
             (strncmp(args, "3", 1)) == 0) {
    char *avArgs = args + 1;  // Skip the first character
    trim(&avArgs);
    const char avIdxC = args[0];
    uint8_t avIdx = atoi(&avIdxC) - 1;  //
    handleCliForAv(avIdx, avArgs, responseBuffer);
  } else {
    sprintf(responseBuffer, "Invalid AV number. Please use 1, 2, or 3%s", no);
  }
}

void AVS_Test(void) {
  AVS_Scan();

  printf("\r\n");
  printf("Testing ADC functionality...\r\n");
  printf("  Av1 Raw: %lu\r\n", AVsRawValues[0]);
  printf("  Av1 Volt: %0.03fV\r\n", AVsVoltages[0]);
  printf("  Av2 Raw: %lu\r\n", AVsRawValues[1]);
  printf("  Av2 Volt: %0.03fV\r\n", AVsVoltages[1]);
  printf("  Av3 Raw: %lu\r\n", AVsRawValues[2]);
  printf("  Av4 Volt: %0.03fV\r\n", AVsVoltages[2]);
}

/* Private Methods
 * -----------------------------------------------------------*/
UF_STATUS AVS_rawToVoltage(float vref, uint32_t adcValue, float *voltValue) {
  if (vref == 0) {
    *voltValue = 0;
    return UF_ERROR;
  }
  *voltValue = ((adcValue * vref) / ADC_RESOLUTION) / adcRatio;
  return UF_OK;
}

void handleCliForAv(uint8_t idx, const char *args, char *responseBuffer) {
  uint16_t min = 0, max = 0;
  OptionStatus status = OPTION_ERROR_INVALID_FORMAT;  // Default error status

  if (strlen(args) == 0) {
    // TODO: Show the AV status
    sprintf(responseBuffer, "Showing AV status...");
    return;
  }
  AvSensor av;
  memcpy(&av, &avSensors[idx], sizeof(AvSensor));

  // Determine command type and skip the command word
  if (strncmp(args, "slow", 4) == 0) {
    char *buffer = (char *)args + 4;
    trim(&buffer);
    if (strncmp(buffer, "enable", 6) == 0) {
      av.statusSlow = 1;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d Slow rule enabled.", idx + 1);
      return;
    } else if (strncmp(buffer, "disable", 7) == 0) {
      AvSensor av;
      memcpy(&av, &avSensors[idx], sizeof(AvSensor));
      av.statusSlow = 0;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d Slow rule disabled.", idx + 1);
      return;
    } else {
      if (av.enabled != 1 || av.statusSlow != 1) {
        sprintf(responseBuffer, "AV%d Slow rule is disabled.", idx + 1);
        return;
      }
      status = extractAvStatus(buffer, &min, &max);
      if (status != OPTION_SUCCESS) {
        setResponseBuffer(responseBuffer, status);
        return;
      }
      av.minSlow = min;
      av.maxSlow = max;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d SLOW rule set to Min: %d, Max: %d%s",
              idx + 1, min, max, ok);
      return;
    }
  } else if (strncmp(args, "fast", 4) == 0) {
    char *buffer = (char *)args + 4;
    trim(&buffer);
    if (strncmp(buffer, "enable", 6) == 0) {
      av.statusFast = 1;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d Fast rule enabled.", idx + 1);
      return;
    } else if (strncmp(buffer, "disable", 7) == 0) {
      AvSensor av;
      memcpy(&av, &avSensors[idx], sizeof(AvSensor));
      av.statusFast = 0;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d Fast rule disabled.", idx + 1);
      return;
    } else {
      if (av.enabled != 1 || av.statusFast != 1) {
        sprintf(responseBuffer, "AV%d Fast rule is disabled.", idx + 1);
        return;
      }
      status = extractAvStatus(buffer, &min, &max);
      if (status != OPTION_SUCCESS) {
        setResponseBuffer(responseBuffer, status);
        return;
      }
      av.minFast = min;
      av.maxFast = max;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d FAST rule set to Min: %d, Max: %d%s",
              idx + 1, min, max, ok);
      return;
    }
  } else if (strncmp(args, "on", 2) == 0) {
    char *buffer = (char *)args + 2;
    trim(&buffer);
    if (strncmp(buffer, "enable", 6) == 0) {
      av.statusOn = 1;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d On rule enabled.", idx + 1);
      return;
    } else if (strncmp(buffer, "disable", 7) == 0) {
      AvSensor av;
      memcpy(&av, &avSensors[idx], sizeof(AvSensor));
      av.statusOn = 0;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d On rule disabled.", idx + 1);
      return;
    } else {
      if (av.enabled != 1 || av.statusOn != 1) {
        sprintf(responseBuffer, "AV%d On rule is disabled.", idx + 1);
        return;
      }
      status = extractAvStatus(buffer, &min, &max);
      if (status != OPTION_SUCCESS) {
        setResponseBuffer(responseBuffer, status);
        return;
      }
      av.minOn = min;
      av.maxOn = max;
      CONFIG_SetAvSensor(&av, idx);
      sprintf(responseBuffer, "AV%d ON rule set to Min: %d, Max: %d%s", idx + 1,
              min, max, ok);
      return;
    }
    status = extractAvStatus(buffer, &min, &max);
  } else {
    sprintf(responseBuffer,
            "Invalid AV command. Please use slow, fast, or on.%s", no);
    return;
  }
}

OptionStatus extractAvStatus(char *args, uint16_t *min, uint16_t *max) {
  char *endptr;
  long tempMin, tempMax;

  // First number extraction
  tempMin = strtol(args, &endptr, 10);
  if (endptr == args) return OPTION_ERROR_INVALID_FORMAT;  // No number found

  if (tempMin < 0 || tempMin > 4095)
    return OPTION_ERROR_OUT_OF_RANGE;  // Number out of range
  *min = (uint16_t)tempMin;

  trim(&endptr);  // Trim remaining string after first number

  // Check if there's another number
  if (*endptr == '\0')
    return OPTION_ERROR_MISSING_VALUES;  // Missing second value

  // Second number extraction
  tempMax = strtol(endptr, &endptr, 10);
  if (endptr == args || *endptr != '\0')
    return OPTION_ERROR_INVALID_FORMAT;  // No second number found
  if (tempMax < 0 || tempMax > 4095)
    return OPTION_ERROR_OUT_OF_RANGE;  // Number out of range
  *max = (uint16_t)tempMax;

  return OPTION_SUCCESS;  // Success
}
void setResponseBuffer(char *responseBuffer, OptionStatus status) {
  static char *no = "\r\nNo!";
  switch (status) {
    case OPTION_SUCCESS:
      sprintf(responseBuffer, "\r\nSuccess!");
      break;
    case OPTION_ERROR_INVALID_FORMAT:
      sprintf(responseBuffer,
              "Invalid format. Expected two numbers separated by space.%s", no);
      break;
    case OPTION_ERROR_OUT_OF_RANGE:
      sprintf(responseBuffer, "One or more values are out of range (0-4095).%s",
              no);
      break;
    case OPTION_ERROR_MISSING_VALUES:
      sprintf(responseBuffer, "Missing values. Expected two numbers.%s", no);
      break;
  }
}