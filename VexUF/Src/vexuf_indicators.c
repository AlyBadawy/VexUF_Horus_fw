/*
 * indicators.c
 *
 *  Created on: Jul 29, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_indicators.h"

#include "vexuf_adc_avs.h"
#include "vexuf_helpers.h"

void IND_applyOnOffLevelsToGPIO(void);
IndLevelOption IND_getCurrentLevel(Indicator ind);

IndLevels indLevels;
IndConfiguration indConfig;

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
    if (level == IndON) {
      HAL_GPIO_WritePin(indicatorPins[ind].port, indicatorPins[ind].pin,
                        GPIO_PIN_SET);
    } else if (level == IndOFF) {
      HAL_GPIO_WritePin(indicatorPins[ind].port, indicatorPins[ind].pin,
                        GPIO_PIN_RESET);
    }
  }
}

IndLevelOption IND_getCurrentLevel(Indicator ind) {
  if (indConfig.globalIndicatorEnabled == 0) return IndOFF;

  if ((ind == IndError || ind == IndWarn || ind == IndInfo) &&
      indConfig.statusIndicatorsEnabled != 1)
    return IndOFF;

  if ((ind == IndAv1 || ind == IndAv2 || ind == IndAv3) &&
      indConfig.AvGlobalIndEnabled != 1)
    return IndOFF;

  switch (ind) {
    case (IndError):
      return indLevels.indErrorLevel;
    case (IndWarn):
      return indLevels.indWarnLevel;
    case (IndInfo):
      return indLevels.indInfoLevel;
    case (IndAv1):
      if (indConfig.Av1IndEnabled != 1) return IndOFF;
      return indLevels.indAv1Level;
    case (IndAv2):
      if (indConfig.Av2IndEnabled != 1) return IndOFF;
      return indLevels.indAv2Level;
    case (IndAv3):
      if (indConfig.Av3IndEnabled != 1) return IndOFF;
      return indLevels.indAv3Level;
    case (IndBuzzer):
      if (indConfig.buzzer1sEnabled != 1) return IndOFF;
      return indLevels.indBuzzerLevel;
    case (IndSdio):
      if (indConfig.sdCardIndicatorEnabled != 1) return IndOFF;
      return indLevels.indSdioLevel;
    default:
      return IndOFF;
  }
}

UF_STATUS IND_setLevel(Indicator ind, IndLevelOption option) {
  UF_STATUS status = UF_OK;

  // Return if current status equals new status
  if (IND_getCurrentLevel(ind) == option ||
      indConfig.globalIndicatorEnabled != 1)
    return UF_DISABLED;

  if ((indConfig.statusIndicatorsEnabled != 1) &&
      (ind == IndError || ind == IndWarn || ind == IndInfo))
    return UF_DISABLED;

  if (indConfig.buzzerEnabled != 1 && ind == IndBuzzer) return UF_DISABLED;
  if (indConfig.sdCardIndicatorEnabled != 1 && ind == IndSdio)
    return UF_DISABLED;

  if (indConfig.AvGlobalIndEnabled != 1 &&
      (ind == IndAv1 || ind == IndAv2 || ind == IndAv3))
    return UF_DISABLED;

  // Special handling for mutual exclusivity
  if ((option != IndOFF) &&
      (ind == IndError || ind == IndWarn || ind == IndInfo)) {
    indLevels.indErrorLevel = IndOFF;
    indLevels.indWarnLevel = IndOFF;
    indLevels.indInfoLevel = IndOFF;
    status = UF_OVERWRITTEN;
  }

  switch (ind) {
    case (IndError):
      indLevels.indErrorLevel = option;
      break;
    case (IndWarn):
      indLevels.indWarnLevel = option;
      break;
    case (IndInfo):
      indLevels.indInfoLevel = option;
      break;
    case (IndAv1):
      indLevels.indAv1Level = option;
      break;
    case (IndAv2):
      indLevels.indAv2Level = option;
      break;
    case (IndAv3):
      indLevels.indAv3Level = option;
      break;
    case (IndBuzzer):
      if (option == IndFAST) {
        indLevels.indBuzzerLevel = IndSLOW;
        status = UF_OVERWRITTEN;
      } else {
        indLevels.indBuzzerLevel = option;
      }
      break;
    case (IndSdio):
      if (option == IndFAST || option == IndSLOW) {
        indLevels.indSdioLevel = IndON;
        status = UF_OVERWRITTEN;
      } else {
        indLevels.indSdioLevel = option;
      }
      break;
    default:
      break;
  }
  IND_applyOnOffLevelsToGPIO();
  return status;
}

UF_STATUS IND_toggleIndWithLevelOption(IndLevelOption option) {
  if (option == IndON || option == IndOFF) return UF_ERROR;  // Invalid status

  for (Indicator ind = IndError; ind <= IndAv3; ind++) {
    IndLevelOption lo = IND_getCurrentLevel(ind);
    if (lo == option) {
      HAL_GPIO_TogglePin(indicatorPins[ind].port, indicatorPins[ind].pin);
    }
  }
  return UF_OK;
}

UF_STATUS IND_buzzOnError(void) {
  if (indConfig.globalIndicatorEnabled && indConfig.buzzerEnabled &&
      indConfig.buzzerHoldOnError) {
    IND_setLevel(IndBuzzer, IndON);
    return UF_OK;
  }
  return UF_DISABLED;
}

UF_STATUS IND_BuzzOnStartUp(void) {
  if (indConfig.globalIndicatorEnabled && indConfig.buzzerEnabled &&
      indConfig.buzzer1sEnabled) {
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