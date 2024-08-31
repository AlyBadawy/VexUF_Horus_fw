/**
 ******************************************************************************
 * @file          : vexuf_adc.c
 * @brief        : VexUF ADC Implementation
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
#include "vexuf_adc.h"

/* TypeDef -------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;

/* Defines -------------------------------------------------------------------*/
#define VREFINT 1.22  // VREFINT is 1.22V

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
uint32_t adcBuffer[5];
float vref;

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
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

/* Private Methods -----------------------------------------------------------*/