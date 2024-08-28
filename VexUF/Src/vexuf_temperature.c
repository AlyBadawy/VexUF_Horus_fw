#include "vexuf_temperature.h"

#include "vexuf_avs.h"

extern uint32_t adcBuffer[5];

float cpuTempC;

float TEMPERATURE_cToF(float c) { return (c * (9.0 / 5.0)) + 32.0; }
float TEMPERATURE_fToC(float f) { return (f - 32.0) * (5.0 / 9.0); }

UF_STATUS TEMPERATURE_getCpuTempC(float* cpuTempC) {
  float vref = 0;
  uint32_t adcBuffer[5];

  if (ADC_run(adcBuffer, &vref) == UF_ERROR) return UF_ERROR;

  float temp_sense = (adcBuffer[1] / ADC_RESOLUTION) * vref;
  *cpuTempC = ((temp_sense - VOLT_AT_25C) / TEMP_SLOPE) + 25.0;

  return UF_OK;
}