/*
 * adc_avs.h
 *
 *  Created on: Jul 27, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_VEXUF_ADC_AVS_H_
#define INC_VEXUF_ADC_AVS_H_

#include "vexuf_helpers.h"

#define VREFINT 1.22
#define ADC_RESOLUTION 4096  // 12-bit ADC resolution

#define V25 0.76
#define CPU_TEMP_AVG_SLOPE 0.0025

#define adcR1 33000.0
#define adcR2 3300.0
#define adcRatio (adcR2 / (adcR1 + adcR2))

#define NUMBER_OF_AVS 3

typedef enum { ADC_OK, ADC_DISABLED, ADC_ERROR } ADC_STATUS;

typedef struct {
  uint16_t enabled : 1;
  uint16_t indicatorEnabled : 1;
  uint16_t statusSlow : 1;
  uint16_t statusFast : 1;
  uint16_t statusOn : 1;
  uint16_t reserved : 11;
  uint16_t minSlow;
  uint16_t maxSlow;
  uint16_t minFast;
  uint16_t maxFast;
  uint16_t minOn;
  uint16_t maxOn;
} AvSensor;

ADC_STATUS ADC_Scan(float* cpuTempC, uint32_t* AVsRawValues[NUMBER_OF_AVS],
                    float* AVsVoltages[NUMBER_OF_AVS]);

#if defined(DEBUG)

void ADC_Test(void);

#endif  // DEBUG

#endif /* INC_VEXUF_ADC_AVS_H_ */