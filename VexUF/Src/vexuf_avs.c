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
extern OutputConfiguration outputConf;
extern char *ok;
extern char *no;

/* Variables -----------------------------------------------------------------*/
float AVsVoltages[NUMBER_OF_AVS];
uint32_t AVsRawValues[NUMBER_OF_AVS];
AvSensor avSensors[NUMBER_OF_AVS];

/* Prototypes ----------------------------------------------------------------*/
UF_STATUS AVS_rawToVoltage(float vref, uint32_t adcValue, float *voltValue);
void handleCliForAv(uint8_t idx, const char *args, char *responseBuffer);
OptionStatus extractAvStatus(const char *args, uint16_t *min, uint16_t *max);
void setResponseBuffer(char *responseBuffer, OptionStatus status);

void setAvSensorStatus(AvSensor *av, uint8_t idx, uint16_t enable,
                       const char *ruleType, uint16_t ruleFlag,
                       char *responseBuffer);
void setAvSensorLimits(AvSensor *av, uint8_t idx, const char *ruleType,
                       char *responseBuffer, uint16_t min, uint16_t max,
                       uint16_t ruleFlag);

/* Code ----------------------------------------------------------------------*/
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
    // Display the status of the AV sensors
    sprintf(responseBuffer,
            "AV1 is %s, AV2 is %s, AV3 is %s.\r\nProvide an AV number (like "
            "`AV 1`) for more details.%s",
            avSensors[0].enabled == 1 ? "enabled" : "disabled",
            avSensors[1].enabled == 1 ? "enabled" : "disabled",
            avSensors[2].enabled == 1 ? "enabled" : "disabled", ok);
    return;
  } else if ((strncmp(args, "1", 1)) == 0 || (strncmp(args, "2", 1)) == 0 ||
             (strncmp(args, "3", 1)) == 0) {
    const char avIdxC = args[0];
    uint8_t avIdx = atoi(&avIdxC) - 1;  //
    const char *avArgs = trim(&args[1]);
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

/* Private Methods -----------------------------------------------------------*/
UF_STATUS AVS_rawToVoltage(float vref, uint32_t adcValue, float *voltValue) {
  if (vref == 0) {
    *voltValue = 0;
    return UF_ERROR;
  }
  *voltValue = ((adcValue * vref) / ADC_RESOLUTION) / adcRatio;
  return UF_OK;
}

void setAvSensorStatus(AvSensor *av, uint8_t idx, uint16_t enable,
                       const char *ruleType, uint16_t ruleFlag,
                       char *responseBuffer) {
  if (ruleFlag == 1)
    av->statusSlow = enable;
  else if (ruleFlag == 2)
    av->statusFast = enable;
  else if (ruleFlag == 3)
    av->statusOn = enable;

  sprintf(responseBuffer, "AV%d %s rule %s.", idx + 1, ruleType,
          enable ? "enabled" : "disabled");
  sprintf(responseBuffer + strlen(responseBuffer), "%s", ok);
}

void setAvSensorLimits(AvSensor *av, uint8_t idx, const char *ruleType,
                       char *responseBuffer, uint16_t min, uint16_t max,
                       uint16_t ruleFlag) {
  if (ruleFlag == 1) {
    av->minSlow = min;
    av->maxSlow = max;
  } else if (ruleFlag == 2) {
    av->minFast = min;
    av->maxFast = max;
  } else if (ruleFlag == 3) {
    av->minOn = min;
    av->maxOn = max;
  }

  sprintf(responseBuffer, "AV%d %s rule set to Min: %d, Max: %d%s", idx + 1,
          ruleType, min, max, ok);
}

void handleCliForAv(uint8_t idx, const char *args, char *responseBuffer) {
  uint16_t min = 0, max = 0;
  OptionStatus status = OPTION_ERROR_INVALID_FORMAT;

  const struct {
    const char *command;
    size_t commandLength;
    const char *ruleType;
    uint16_t ruleFlag;  // 1 for slow, 2 for fast, 3 for on
  } rules[] = {
      {"slow", 4, "SLOW", 1},
      {"fast", 4, "FAST", 2},
      {"on", 2, "ON", 3},
  };

  if (strlen(args) == 0) {
    if (avSensors[idx].enabled != 1) {
      sprintf(responseBuffer, "AV%d is disabled.%s", idx + 1, ok);
    } else {
      sprintf(responseBuffer, "AV%d is enabled with the following rules:\r\n",
              idx + 1);
      if (avSensors[idx].statusSlow) {
        sprintf(responseBuffer + strlen(responseBuffer),
                "  Slow Rule Enabled. Min: %d, Max: %d\r\n",
                avSensors[idx].minSlow, avSensors[idx].maxSlow);
      } else {
        sprintf(responseBuffer + strlen(responseBuffer),
                "  Slow Rule Disabled.\r\n");
      }
      if (avSensors[idx].statusFast) {
        sprintf(responseBuffer + strlen(responseBuffer),
                "  Fast Rule Enabled. Min: %d, Max: %d\r\n",
                avSensors[idx].minFast, avSensors[idx].maxFast);
      } else {
        sprintf(responseBuffer + strlen(responseBuffer),
                "  Fast Rule Disabled.\r\n");
      }
      if (avSensors[idx].statusOn) {
        sprintf(responseBuffer + strlen(responseBuffer),
                "  On Rule Enabled. Min: %d, Max: %d", avSensors[idx].minOn,
                avSensors[idx].maxOn);
      } else {
        sprintf(responseBuffer + strlen(responseBuffer), "  On Rule Disabled.");
      }
    }
    sprintf(responseBuffer + strlen(responseBuffer), "%s", ok);
    return;
  }

  for (size_t i = 0; i < sizeof(rules) / sizeof(rules[0]); i++) {
    if (strncmp(args, "enable", 6) == 0) {
      avSensors[idx].enabled = 1;
      sprintf(responseBuffer, "AV%d is set to enabled.%s", idx + 1, ok);
      return;
    } else if (strncmp(args, "disable", 7) == 0) {
      avSensors[idx].enabled = 0;
      sprintf(responseBuffer, "AV%d is set to disabled.%s", idx + 1, ok);
      return;
    } else if (strncmp(args, rules[i].command, rules[i].commandLength) == 0) {
      if (avSensors[idx].enabled != 1) {
        sprintf(responseBuffer, "AV%d %s rule is disabled.%s", idx + 1,
                rules[i].ruleType, ok);
        return;
      }
      const char *buffer = trim((char *)args + rules[i].commandLength);
      if (strlen(buffer) == 0) {
        sprintf(responseBuffer, "AV%d %s rule is %s.%s", idx + 1,
                rules[i].ruleType,
                avSensors[idx].enabled ? "enabled" : "disabled", ok);
        return;
      } else if (strncmp(buffer, "enable", 6) == 0) {
        setAvSensorStatus(&avSensors[idx], idx, 1, rules[i].ruleType,
                          rules[i].ruleFlag, responseBuffer);
        return;
      } else if (strncmp(buffer, "disable", 7) == 0) {
        setAvSensorStatus(&avSensors[idx], idx, 0, rules[i].ruleType,
                          rules[i].ruleFlag, responseBuffer);
        return;
      } else {
        uint16_t enabled = 0;
        if (rules[i].ruleFlag == 1)
          enabled = avSensors[idx].statusSlow;
        else if (rules[i].ruleFlag == 2)
          enabled = avSensors[idx].statusFast;
        else if (rules[i].ruleFlag == 3)
          enabled = avSensors[idx].statusOn;

        if (avSensors[idx].enabled != 1 || enabled != 1) {
          sprintf(responseBuffer, "AV%d %s rule is disabled.%s", idx + 1,
                  rules[i].ruleType, no);
          return;
        }

        status = extractAvStatus(buffer, &min, &max);
        if (status != OPTION_SUCCESS) {
          setResponseBuffer(responseBuffer, status);
          return;
        }

        setAvSensorLimits(&avSensors[idx], idx, rules[i].ruleType,
                          responseBuffer, min, max, rules[i].ruleFlag);
        return;
      }
    }
  }

  sprintf(responseBuffer,
          "Error with AV command. Please use `help AV` for help.%s", no);
}

OptionStatus extractAvStatus(const char *args, uint16_t *min, uint16_t *max) {
  char *endptr;
  long tempMin, tempMax;

  // First number extraction
  tempMin = strtol(args, &endptr, 10);
  if (endptr == args) return OPTION_ERROR_INVALID_FORMAT;  // No number found

  if (tempMin < 0 || tempMin > 4095)
    return OPTION_ERROR_OUT_OF_RANGE;  // Number out of range
  *min = (uint16_t)tempMin;

  endptr = trim(endptr);  // Trim remaining string after first number

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