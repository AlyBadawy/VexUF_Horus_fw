#include "vexuf_temperature.h"

#include "vexuf_avs.h"

extern uint32_t adcBuffer[5];

float cpuTempC;

float TEMPERATURE_cToF(float c) { return (c * (9.0 / 5.0)) + 32.0; }
float TEMPERATURE_fToC(float f) { return (f - 32.0) * (5.0 / 9.0); }

UF_STATUS TEMPERATURE_getCpuTempC(float* cpuTempC) {
  float vref = 0;
  ADC_getVref(&vref);

  if (vref == 0 || adcBuffer[1] == 0) return UF_ERROR;
  // float temp_sense = (vref / ADC_RESOLUTION) * adcBuffer[1];
  float temp_sense = (adcBuffer[1] / ADC_RESOLUTION) * vref;
  *cpuTempC = ((temp_sense - VOLT_AT_25C) / TEMP_SLOPE) + 25.0;

  return UF_OK;
}