/**
 ******************************************************************************
 * @file          : vexuf_buzzer.c
 * @brief        : VexUF Buzzer Implementation
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
#include "vexuf_buzzer.h"

#include "vexuf_config.h"

/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern IndConfiguration indConf;
/* Variables -----------------------------------------------------------------*/
const char *ok = "\r\nOk!";
const char *no = "\r\nNo!";
const char *enabledStr = "Enabled";
const char *disabledStr = "Disabled";
const char *buzzerStr = "Buzzer";
const char *startBeepStr = "start beep";
const char *errorStr = "beep on error";
const char *isStr = "is";
const char *okStr = "\r\nOK!";
const char *noStr = "\r\nNo!";
static char *buzzerDisabled = "Buzzer is disabled. Can't configure buzzer.";

/* Prototypes ----------------------------------------------------------------*/
void setResponse(const char *feature, uint8_t enabled, char *responseBuffer);

/* Code ----------------------------------------------------------------------*/
UF_STATUS IND_buzzOnError(void) {
  if (indConf.globalIndicatorEnabled && indConf.buzzerEnabled &&
      indConf.buzzerHoldOnError) {
    IND_setLevel(IndBuzzer, IndON);
    return UF_OK;
  }
  return UF_DISABLED;
}

UF_STATUS IND_BuzzOnStartUp(void) {
  if (indConf.globalIndicatorEnabled && indConf.buzzerEnabled &&
      indConf.buzzer1sEnabled) {
    for (uint8_t i = 0; i < 3; i++) {
      if (IND_setLevel(IndBuzzer, IndON) != UF_OK) return UF_ERROR;
      HAL_Delay(20);
      if (IND_setLevel(IndBuzzer, IndOFF) != UF_OK) return UF_ERROR;
      HAL_Delay(40);
    }
    return UF_OK;
  }
  return UF_DISABLED;
}
void BUZZ_handleCli(const char *args, char *responseBuffer) {
  // TODO: Refactor this function.
  IndConfiguration newIndConfig;
  memcpy(&newIndConfig, &indConf, sizeof(indConf));

  if ((strncmp(args, "enable", 6)) == 0) {
    newIndConfig.buzzerEnabled = 1;
    CONFIG_setIndicatorsConf(&newIndConfig);
    setResponse("", 1, responseBuffer);
  } else if ((strncmp(args, "disable", 7)) == 0) {
    newIndConfig.buzzerEnabled = 0;
    CONFIG_setIndicatorsConf(&newIndConfig);
    setResponse("", 0, responseBuffer);
  } else if (strncmp(args, startBeepStr, 10) == 0) {
    if (newIndConfig.buzzerEnabled) {
      newIndConfig.buzzer1sEnabled = strncmp(args + 11, "on", 2) == 0 ? 1 : 0;
      CONFIG_setIndicatorsConf(&newIndConfig);
      setResponse(startBeepStr, newIndConfig.buzzer1sEnabled, responseBuffer);
    } else {
      sprintf(responseBuffer, "%s%s", buzzerDisabled, no);
    }
  } else if (strncmp(args, "error", 5) == 0) {
    if (newIndConfig.buzzerEnabled) {
      newIndConfig.buzzerHoldOnError = strncmp(args + 6, "on", 2) == 0 ? 1 : 0;
      CONFIG_setIndicatorsConf(&newIndConfig);
      setResponse(errorStr, newIndConfig.buzzerHoldOnError, responseBuffer);
    } else {
      sprintf(responseBuffer, "%s%s", buzzerDisabled, no);
    }
  } else if (strlen(args) == 0) {
    if (indConf.buzzerEnabled) {
      sprintf(responseBuffer, "%s %s %s %s. %s %s.%s", buzzerStr, isStr,
              enabledStr,
              indConf.buzzer1sEnabled ? "start beep is Enabled"
                                      : "start beep is Disabled",
              errorStr, indConf.buzzerHoldOnError ? enabledStr : disabledStr,
              okStr);
    } else {
      setResponse(isStr, 0, responseBuffer);
    }
  }
}

/* Private Methods -----------------------------------------------------------*/
void setResponse(const char *feature, uint8_t enabled, char *responseBuffer) {
  sprintf(responseBuffer, "%s %s%s%s", buzzerStr, feature,
          enabled ? enabledStr : disabledStr, okStr);
}
