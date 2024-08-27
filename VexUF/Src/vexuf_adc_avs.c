/*
 * adc_avs.c
 *
 *  Created on: Jul 27, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_adc_avs.h"

#include "vexuf_helpers.h"
#include "vexuf_indicators.h"

extern ADC_HandleTypeDef hadc1;

float cpuTempC;
float AVsVoltages[3];
uint32_t AVsRawValues[3];
uint32_t adcBuffer[5];
AvSensor avSensors[NUMBER_OF_AVS];

/*
 * avsBuffer:
 * 0: VrefValue
 * 1: CPU raw value
 * 2: AV1 Raw Value
 * 3: AV2 Raw Value
 * 4: AV3 Raw Value
 */

UF_STATUS ADC_getVref(float* vref);
UF_STATUS ADC_rawToVoltage(float vref, uint32_t adcValue, float* rawValue);
UF_STATUS ADC_getCpuTempC(float vref, float* cpuTempC);

UF_STATUS ADC_getVref(float* vref) {
  if (adcBuffer[0] == 0) {
    *vref = 0;
    return UF_ERROR;
  }
  *vref = (VREFINT * ADC_RESOLUTION) / adcBuffer[0];
  return UF_OK;
}
UF_STATUS ADC_rawToVoltage(float vref, uint32_t adcValue, float* rawValue) {
  *rawValue = ((adcValue * vref) / ADC_RESOLUTION) / adcRatio;
  return UF_OK;
}
UF_STATUS ADC_getCpuTempC(float vref, float* cpuTempC) {
  if (vref == 0 || adcBuffer[1] == 0) return UF_ERROR;
  float temp_sense = (vref / ADC_RESOLUTION) * adcBuffer[1];
  *cpuTempC = (((V25 - temp_sense) / CPU_TEMP_AVG_SLOPE) + 25.0);

  return UF_OK;
}

UF_STATUS ADC_Scan(float* cpuTempC, uint32_t* AVsRawValues,
                   float* AVsVoltages) {
  float vref;
  Indicator ind;
  AvSensor av;
  uint32_t value;

  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, 5) != HAL_OK)
    return UF_ERROR;
  HAL_Delay(50);
  if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) return UF_ERROR;

  if (ADC_getVref(&vref) == UF_ERROR) return UF_ERROR;
  if (ADC_getCpuTempC(vref, cpuTempC) == UF_ERROR) return UF_ERROR;

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
      IND_setLevel(ind, IndSLOW);
    } else if (av.statusFast && (value >= av.minFast && value <= av.maxFast)) {
      IND_setLevel(ind, IndFAST);
    } else if (av.statusOn && (value >= av.minOn && value <= av.maxOn)) {
      IND_setLevel(ind, IndON);
    } else {
      IND_setLevel(ind, IndOFF);
    }

    // TODO: Display output on LCD
    // TODO: Log the output to Logger
  }
  return UF_OK;
}

void ADC_Test(void) {
  ADC_Scan(&cpuTempC, AVsRawValues, AVsVoltages);

  printf("\r\n");
  printf("Testing ADC functionality...\r\n");
  printf("  Temperature CPU Raw: %lu\r\n", adcBuffer[1]);
  printf("  Temperature CPU C: %.2f\r\n", cpuTempC);
  printf("  Temperature CPU F: %0.2f\r\n", cToF(cpuTempC));
  printf("  Av1 Raw: %lu\r\n", AVsRawValues[0]);
  printf("  Av1 Volt: %0.03fV\r\n", AVsVoltages[0]);
  printf("  Av2 Raw: %lu\r\n", AVsRawValues[1]);
  printf("  Av2 Volt: %0.03fV\r\n", AVsVoltages[1]);
  printf("  Av3 Raw: %lu\r\n", AVsRawValues[2]);
  printf("  Av4 Volt: %0.03fV\r\n", AVsVoltages[2]);
}
