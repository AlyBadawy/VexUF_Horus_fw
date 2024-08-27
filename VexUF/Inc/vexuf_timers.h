#if !defined(INC_VEXUF_TIMERS_H_)
#define INC_VEXUF_TIMERS_H_

#include "vexuf.h"
#include "vexuf_helpers.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

UF_STATUS TIMERS_Start(void);
UF_STATUS TIMERS_Stop(void);

#endif  // INC_VEXUF_TIMERS_H_
