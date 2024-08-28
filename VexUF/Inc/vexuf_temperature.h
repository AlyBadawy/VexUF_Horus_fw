#ifndef VEXUF_TEMPERATURE_H_
#define VEXUF_TEMPERATURE_H_

#include "vexuf.h"

float TEMPERATURE_cToF(float c);
float TEMPERATURE_fToC(float f);

UF_STATUS TEMPERATURE_getCpuTempC(float* cpuTempC);

#endif  // VEXUF_TEMPERATURE_H_