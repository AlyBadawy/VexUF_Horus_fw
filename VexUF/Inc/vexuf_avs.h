/*
 * adc_avs.h
 *
 *  Created on: Jul 27, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_VEXUF_AVS_H_
#define INC_VEXUF_AVS_H_

#include "vexuf.h"

#define ADC_RESOLUTION 4095.0  // 12-bit ADC
#define VREFINT 1.22           // VREFINT is 1.22V
#define TEMP_SLOPE 0.0025      // 2.5mV/°C
#define VOLT_AT_25C 0.76       // 0.76V at 25°C

#define adcR1 33000.0
#define adcR2 3300.0
#define adcRatio (adcR2 / (adcR1 + adcR2))

#define NUMBER_OF_AVS 3

typedef struct {
  uint16_t enabled : 1;
  uint16_t statusSlow : 1;
  uint16_t statusFast : 1;
  uint16_t statusOn : 1;
  uint16_t reserved : 12;
  uint16_t minSlow;
  uint16_t maxSlow;
  uint16_t minFast;
  uint16_t maxFast;
  uint16_t minOn;
  uint16_t maxOn;
} AvSensor;

UF_STATUS ADC_run(uint32_t* adcBuffer, float* vref);

UF_STATUS ADC_Scan(uint32_t* AVsRawValues, float* AVsVoltages);

void ADC_Test(void);

#endif /* INC_VEXUF_AVS_H_ */