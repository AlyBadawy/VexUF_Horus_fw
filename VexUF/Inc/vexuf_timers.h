#if !defined(INC_VEXUF_TIMERS_H_)
#define INC_VEXUF_TIMERS_H_

#include "vexuf.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);

void TIMERS_init(TIM_HandleTypeDef* t10hz, TIM_HandleTypeDef* t1hz,
                 TIM_HandleTypeDef* t0d1hz);

UF_STATUS TIMERS_Start(void);
UF_STATUS TIMERS_Stop(void);

#endif  // INC_VEXUF_TIMERS_H_
