/*
 * indicators.h
 *
 *  Created on: Jul 29, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_VEXUF_INDICATORS_H_
#define INC_VEXUF_INDICATORS_H_

#include "vexuf_helpers.h"

typedef enum { IND_OK, IND_DISABLED, IND_OVERWRITTEN, IND_ERROR } IND_STATUS;

typedef struct {
  GPIO_TypeDef* port;
  uint16_t pin;
} IndicatorPin;

typedef enum {
  IndError = 0x00,
  IndWarn = 0x01,
  IndInfo = 0x02,
  IndAv1 = 0x03,
  IndAv2 = 0x04,
  IndAv3 = 0x05,
  IndBuzzer = 0x06,
  IndSdio = 0x07
} Indicator;

typedef enum {
  IndOFF = 0x00,
  IndSLOW = 0x01,
  IndFAST = 0x02,
  IndON = 0x03
} IndLevelOption;

typedef struct {
  IndLevelOption indErrorLevel : 2;
  IndLevelOption indWarnLevel : 2;
  IndLevelOption indInfoLevel : 2;
  IndLevelOption indAv1Level : 2;
  IndLevelOption indAv2Level : 2;
  IndLevelOption indAv3Level : 2;
  IndLevelOption indBuzzerLevel : 2;
  IndLevelOption indSdioLevel : 2;
} IndLevels;

typedef struct {
  uint16_t globalIndicatorEnabled : 1;
  uint16_t buzzerEnabled : 1;
  uint16_t buzzer1sEnabled : 1;
  uint16_t buzzerHoldOnError : 1;
  uint16_t statusIndicatorsEnabled : 1;
  uint16_t sdCardIndicatorEnabled : 1;
  uint16_t AvGlobalIndEnabled : 1;
  uint16_t Av1IndEnabled : 1;
  uint16_t Av2IndEnabled : 1;
  uint16_t Av3IndEnabled : 1;
  uint16_t reserved : 6;
} IndConfiguration;

IndLevelOption IND_getCurrentLevel(Indicator ind);
IND_STATUS IND_setLevel(Indicator ind, IndLevelOption option);
IND_STATUS IND_toggleIndWithLevelOption(IndLevelOption option);

IND_STATUS IND_buzzOnError(void);
IND_STATUS IND_BuzzOnStartUp(void);

#endif /* INC_VEXUF_INDICATORS_H_ */