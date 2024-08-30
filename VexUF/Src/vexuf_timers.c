/**
 ******************************************************************************
 * @file          : vexuf_timers.c
 * @brief        : VexUF Timers Implementation
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
#include "vexuf_timers.h"

#include "vexuf_indicators.h"

/* TypeDef -------------------------------------------------------------------*/
static TIM_HandleTypeDef* tim10hz;
static TIM_HandleTypeDef* tim1hz;
static TIM_HandleTypeDef* tim0d1hz;
/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern VexufStatus vexufStatus;

/* Variables -----------------------------------------------------------------*/

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
void TIMERS_init(TIM_HandleTypeDef* t10hz, TIM_HandleTypeDef* t1hz,
                 TIM_HandleTypeDef* t0d1hz) {
  tim10hz = t10hz;
  tim1hz = t1hz;
  tim0d1hz = t0d1hz;
}

UF_STATUS TIMERS_Start(void) {
  if (HAL_TIM_Base_Start_IT(tim10hz) != HAL_OK) return UF_ERROR;
  if (HAL_TIM_Base_Start_IT(tim1hz) != HAL_OK) return UF_ERROR;
  if (HAL_TIM_Base_Start_IT(tim0d1hz) != HAL_OK) return UF_ERROR;

  return UF_OK;
}

UF_STATUS TIMERS_Stop(void) {
  if (HAL_TIM_Base_Stop_IT(tim10hz) != HAL_OK) return UF_ERROR;
  if (HAL_TIM_Base_Stop_IT(tim1hz) != HAL_OK) return UF_ERROR;
  if (HAL_TIM_Base_Stop_IT(tim0d1hz) != HAL_OK) return UF_ERROR;
  vexufStatus.timer_0d1hz_ticked = 0;
  vexufStatus.timer_1hz_ticked = 0;
  vexufStatus.timer_10hz_ticked = 0;

  return UF_OK;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  } else if (htim == tim10hz) {
    vexufStatus.timer_10hz_ticked = 1;
  } else if (htim == tim1hz) {
    vexufStatus.timer_1hz_ticked = 1;
  } else if (htim == tim0d1hz) {
    vexufStatus.timer_0d1hz_ticked = 1;
  }
}

/* Private Methods -----------------------------------------------------------*/