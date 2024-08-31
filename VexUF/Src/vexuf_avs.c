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

#include "vexuf.h"
#include "vexuf_config.h"
#include "vexuf_indicators.h"
#include "vexuf_temperature.h"

/* TypeDef -------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;

/* Defines -------------------------------------------------------------------*/
#define ADC_RESOLUTION 4095.0  // 12-bit ADC
#define VREFINT 1.22           // VREFINT is 1.22V

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

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
UF_STATUS ADC_Init(void) {
  for (uint8_t i = 0; i < NUMBER_OF_AVS; i++) {
    if (CONFIG_getAvSensor(&avSensors[i], i) != UF_OK) return UF_ERROR;
  }
  return UF_OK;
}

UF_STATUS ADC_rawToVoltage(float vref, uint32_t adcValue, float* voltValue) {
  *voltValue = ((adcValue * vref) / ADC_RESOLUTION) / adcRatio;
  return UF_OK;
}

UF_STATUS ADC_run(uint32_t* adcBuffer, float* vref) {
  if (HAL_ADC_Start_DMA(&hadc1, adcBuffer, 5) == HAL_OK) {
    HAL_Delay(50);
    if (HAL_ADC_Stop_DMA(&hadc1) == HAL_OK && adcBuffer[0] != 0) {
      *vref = (VREFINT * ADC_RESOLUTION) / adcBuffer[0];
      return UF_OK;
    }
  }
  *vref = 0;
  return UF_ERROR;
}

UF_STATUS ADC_Scan(uint32_t* AVsRawValues, float* AVsVoltages) {
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
    ADC_rawToVoltage(vref, adcBuffer[2 + i], &AVsVoltages[i]);
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

void ADC_Test(void) {
  ADC_Scan(AVsRawValues, AVsVoltages);

  printf("\r\n");
  printf("Testing ADC functionality...\r\n");
  printf("  Av1 Raw: %lu\r\n", AVsRawValues[0]);
  printf("  Av1 Volt: %0.03fV\r\n", AVsVoltages[0]);
  printf("  Av2 Raw: %lu\r\n", AVsRawValues[1]);
  printf("  Av2 Volt: %0.03fV\r\n", AVsVoltages[1]);
  printf("  Av3 Raw: %lu\r\n", AVsRawValues[2]);
  printf("  Av4 Volt: %0.03fV\r\n", AVsVoltages[2]);
}
