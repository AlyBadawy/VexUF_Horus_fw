#if !defined(INC_VEXUF_TIMERS_H_)
#define INC_VEXUF_TIMERS_H_

#include "vexuf_helpers.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void TIMERS_Start(void);
void TIMERS_Stop(void);

#endif  // INC_VEXUF_TIMERS_H_
