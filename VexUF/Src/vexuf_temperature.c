#include "vexuf_temperature.h"

#include "aht20.h"
#include "vexuf_avs.h"

extern uint32_t adcBuffer[5];

float cpuTempC = 0, internalTempC = 0;
float internalHumidity;

float TEMPERATURE_cToF(float c) { return (c * (9.0 / 5.0)) + 32.0; }
float TEMPERATURE_fToC(float f) { return (f - 32.0) * (5.0 / 9.0); }

UF_STATUS TEMPERATURE_getCpuTempC(void) {
  float vref = 0;
  uint32_t adcBuffer[5];

  if (ADC_run(adcBuffer, &vref) == UF_ERROR) return UF_ERROR;

  float temp_sense = (adcBuffer[1] / ADC_RESOLUTION) * vref;
  cpuTempC = ((temp_sense - VOLT_AT_25C) / TEMP_SLOPE) + 25.0;

  return UF_OK;
}

UF_STATUS TEMPERATURE_getInternalTempC(void) {
  return AHT20_ReadTemperatureHumidity(&internalTempC, &internalHumidity);
}

void TEMPERATURE_handleCli(const char *args, char *responseBuffer) {
  static char *ok = "\r\nOk!";

  if (strlen(args) == 0) {
    // TODO: logic to show all temperatures
  } else if ((strncmp(args, "internal", 8)) == 0) {
    TEMPERATURE_getInternalTempC();
    sprintf(responseBuffer,
            "Temperature (Internal): %0.2fC (%0.2fF) - Humidity: %0.2f%%%s",
            internalTempC, TEMPERATURE_cToF(internalTempC), internalHumidity,
            ok);
  } else if ((strncmp(args, "external", 8)) == 0) {
    // TODO: implement external temperature
  } else if ((strncmp(args, "cpu", 3)) == 0) {
    TEMPERATURE_getCpuTempC();  // todo: implement this function
    sprintf(responseBuffer, "Temperature (CPU): %0.2fC (%0.2fF)%s", cpuTempC,
            TEMPERATURE_cToF(cpuTempC), ok);
  }
}
