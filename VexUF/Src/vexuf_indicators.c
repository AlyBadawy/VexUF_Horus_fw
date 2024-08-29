/*
 * indicators.c
 *
 *  Created on: Jul 29, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_indicators.h"

#include "vexuf.h"
#include "vexuf_avs.h"

void IND_applyOnOffLevelsToGPIO(void);
IndLevelOption IND_getCurrentLevel(Indicator ind);

IndLevels indLevels;
IndConfiguration indConf;

static IndicatorPin indicatorPins[] = {
    {ErrorInd_GPIO_Port, ErrorInd_Pin},
    {WarnInd_GPIO_Port, WarnInd_Pin},
    {InfoInd_GPIO_Port, InfoInd_Pin},
    {Av1Indicator_GPIO_Port, Av1Indicator_Pin},
    {Av2Indicator_GPIO_Port, Av2Indicator_Pin},
    {Av3Indicator_GPIO_Port, Av3Indicator_Pin},
    {Buzzer_GPIO_Port, Buzzer_Pin},
    {SdioInd_GPIO_Port, SdioInd_Pin}};

void IND_applyOnOffLevelsToGPIO(void) {
  for (Indicator ind = IndError; ind <= IndSdio; ind++) {
    IndLevelOption level = IND_getCurrentLevel(ind);
    if (level == IndON || level == IndOFF) {
      HAL_GPIO_WritePin(indicatorPins[ind].port, indicatorPins[ind].pin,
                        (level == IndON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
  }
}

IndLevelOption IND_getCurrentLevel(Indicator ind) {
  if (indConf.globalIndicatorEnabled == 0) return IndOFF;

  if ((ind == IndError || ind == IndWarn || ind == IndInfo) &&
      indConf.statusIndicatorsEnabled != 1)
    return IndOFF;

  if ((ind == IndAv1 || ind == IndAv2 || ind == IndAv3) &&
      indConf.AvGlobalIndEnabled != 1)
    return IndOFF;

  // Indicator-specific checks and return levels
  switch (ind) {
    case IndError:
      return indLevels.indErrorLevel;
    case IndWarn:
      return indLevels.indWarnLevel;
    case IndInfo:
      return indLevels.indInfoLevel;
    case IndAv1:
      return indConf.Av1IndEnabled ? indLevels.indAv1Level : IndOFF;
    case IndAv2:
      return indConf.Av2IndEnabled ? indLevels.indAv2Level : IndOFF;
    case IndAv3:
      return indConf.Av3IndEnabled ? indLevels.indAv3Level : IndOFF;
    case IndBuzzer:
      return indConf.buzzer1sEnabled ? indLevels.indBuzzerLevel : IndOFF;
    case IndSdio:
      return indConf.sdCardIndicatorEnabled ? indLevels.indSdioLevel : IndOFF;
    default:
      return IndOFF;
  }
}

UF_STATUS IND_setLevel(Indicator ind, IndLevelOption option) {
  // Check if global indicator is disabled or current level is the same as
  // requested
  if (indConf.globalIndicatorEnabled != 1 || IND_getCurrentLevel(ind) == option)
    return UF_DISABLED;

  // Return early if the specific indicator is disabled
  if ((ind == IndError || ind == IndWarn || ind == IndInfo) &&
      !indConf.statusIndicatorsEnabled)
    return UF_DISABLED;
  if (ind == IndBuzzer && !indConf.buzzerEnabled) return UF_DISABLED;
  if (ind == IndSdio && !indConf.sdCardIndicatorEnabled) return UF_DISABLED;
  if ((ind == IndAv1 || ind == IndAv2 || ind == IndAv3) &&
      !indConf.AvGlobalIndEnabled)
    return UF_DISABLED;

  UF_STATUS status = UF_OK;

  // Handle mutual exclusivity for status indicators
  if ((option != IndOFF) &&
      (ind == IndError || ind == IndWarn || ind == IndInfo)) {
    indLevels.indErrorLevel = IndOFF;
    indLevels.indWarnLevel = IndOFF;
    indLevels.indInfoLevel = IndOFF;
    status = UF_OVERWRITTEN;
  }

  switch (ind) {
    case IndError:
      indLevels.indErrorLevel = option;
      break;
    case IndWarn:
      indLevels.indWarnLevel = option;
      break;
    case IndInfo:
      indLevels.indInfoLevel = option;
      break;
    case IndAv1:
      indLevels.indAv1Level = option;
      break;
    case IndAv2:
      indLevels.indAv2Level = option;
      break;
    case IndAv3:
      indLevels.indAv3Level = option;
      break;
    case IndBuzzer:
      if (option == IndFAST) {
        indLevels.indBuzzerLevel = IndSLOW;
        status = UF_OVERWRITTEN;
      } else {
        indLevels.indBuzzerLevel = option;
      }
      break;
    case IndSdio:
      if (option == IndFAST || option == IndSLOW) {
        indLevels.indSdioLevel = IndON;
        status = UF_OVERWRITTEN;
      } else {
        indLevels.indSdioLevel = option;
      }
      break;
    default:
      return UF_ERROR;
  }
  IND_applyOnOffLevelsToGPIO();
  return status;
}

UF_STATUS IND_toggleIndWithLevelOption(IndLevelOption option) {
  if (option == IndON || option == IndOFF) return UF_ERROR;  // Invalid status

  for (Indicator ind = IndError; ind <= IndAv3; ind++) {
    if (IND_getCurrentLevel(ind) == option) {
      HAL_GPIO_TogglePin(indicatorPins[ind].port, indicatorPins[ind].pin);
    }
  }
  return UF_OK;
}

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