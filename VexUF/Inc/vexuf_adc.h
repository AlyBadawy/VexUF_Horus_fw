#ifndef VEXUF_ADC_H
#define VEXUF_ADC_H

#include "vexuf.h"

#define ADC_RESOLUTION 4095.0  // 12-bit ADC

UF_STATUS ADC_run(uint32_t* adcBuffer, float* vref);

#endif  // VEXUF_ADC_H