#include "vexuf_timers.h"

#include "vexuf_indicators.h"

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim9;

extern VexufStatus vexufStatus;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  } else if (htim->Instance == TIM4) {
    // every 100ms (10Hz);
    vexufStatus.timer_10hz_ticked = 1;
  } else if (htim->Instance == TIM9) {
    // every 1s (1Hz)
    vexufStatus.timer_1hz_ticked = 1;
  } else if (htim->Instance == TIM5) {
    // every 10 seconds (0.1Hz)
    vexufStatus.timer_0d1hz_ticked = 1;
  }
}

void TIMERS_Start(void) {
  // Start TIM4, TIM9, TIM5 in interrupt mode
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_Base_Start_IT(&htim5);
  HAL_TIM_Base_Start_IT(&htim9);
}

void TIMERS_Stop(void) {
  HAL_TIM_Base_Stop_IT(&htim4);
  HAL_TIM_Base_Stop_IT(&htim5);
  HAL_TIM_Base_Stop_IT(&htim9);
  vexufStatus.timer_0d1hz_ticked = 0;
  vexufStatus.timer_1hz_ticked = 0;
  vexufStatus.timer_10hz_ticked = 0;
}