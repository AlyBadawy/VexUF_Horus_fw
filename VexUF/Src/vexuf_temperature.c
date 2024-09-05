/**
 ******************************************************************************
 * @file          : vexuf_temperature.c
 * @brief        : VexUF Temperature Implementation
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
#include "vexuf_temperature.h"

#include "aht20.h"
#include "vexuf_adc.h"
#include "vexuf_avs.h"

/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/
#define ADC_RESOLUTION 4095.0  // 12-bit ADC
#define ADC_TEMP_SLOPE 0.0025  // 2.5mV/°C
#define ADC_VOLT_AT_25C 0.76   // 0.76V at 25°C
/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern char *ok;
extern char *no;

/* Variables -----------------------------------------------------------------*/
float cpuTempC = 0, internalTempC = 0;
float internalHumidity;

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
float TEMPERATURE_cToF(float c) { return (c * (9.0 / 5.0)) + 32.0; }
float TEMPERATURE_fToC(float f) { return (f - 32.0) * (5.0 / 9.0); }

UF_STATUS TEMPERATURE_getCpuTempC(void) {
  float vref = 0;
  uint32_t adcBuffer[5];

  if (ADC_run(adcBuffer, &vref) == UF_ERROR) return UF_ERROR;

  float temp_sense = (adcBuffer[1] / ADC_RESOLUTION) * vref;
  cpuTempC = ((temp_sense - ADC_VOLT_AT_25C) / ADC_TEMP_SLOPE) + 25.0;

  return UF_OK;
}

UF_STATUS TEMPERATURE_getInternalTempC(void) {
  return AHT20_ReadTemperatureHumidity(&internalTempC, &internalHumidity);
}

void TEMPERATURE_handleCli(const char *args, char *responseBuffer) {
  if (strlen(args) == 0) {
    TEMPERATURE_getCpuTempC();
    TEMPERATURE_getInternalTempC();
    sprintf(responseBuffer,
            "Temperature (CPU): %0.2fC (%0.2fF)\r\n"
            "Temperature (Internal): %0.2fC (%0.2fF) - Humidity: %0.2f%%\r\n"
            "Temperature (External): not configured"
            "%s",
            cpuTempC, TEMPERATURE_cToF(cpuTempC), internalTempC,
            TEMPERATURE_cToF(internalTempC), internalHumidity, ok);
  } else if ((strncmp(args, "internal", 8)) == 0) {
    TEMPERATURE_getInternalTempC();
    sprintf(responseBuffer,
            "Temperature (Internal): %0.2fC (%0.2fF) - Humidity: %0.2f%%%s",
            internalTempC, TEMPERATURE_cToF(internalTempC), internalHumidity,
            ok);
  } else if ((strncmp(args, "external", 8)) == 0) {
    sprintf(responseBuffer, "Temperature (External): not configured%s", ok);
  } else if ((strncmp(args, "cpu", 3)) == 0) {
    TEMPERATURE_getCpuTempC();  // todo: implement this function
    sprintf(responseBuffer, "Temperature (CPU): %0.2fC (%0.2fF)%s", cpuTempC,
            TEMPERATURE_cToF(cpuTempC), ok);
  } else {
    sprintf(responseBuffer,
            "Error with Temperature command. Please use `help Temperature` for "
            "help.%s",
            no);
  }
}

/* Private Methods -----------------------------------------------------------*/