#include "vexuf_buzzer.h"

#include "vexuf_config.h"

extern IndConfiguration indConf;

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

void setResponse(const char *feature, uint8_t enabled, char *responseBuffer) {
  sprintf(responseBuffer, "%s %s%s%s", buzzerStr, feature,
          enabled ? enabledStr : disabledStr, okStr);
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