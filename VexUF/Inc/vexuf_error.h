/*
 * vexuf_error.h
 *
 *  Created on: Aug 23, 2024
 *      Author: Aly Badawy
 */

#if !defined(INC_VEXUF_ERROR_H)
#define INC_VEXUF_ERROR_H

#include "iwdg.h"
#include "vexuf_helpers.h"
#include "vexuf_indicators.h"
#include "vexuf_outputs.h"
#include "vexuf_pwm.h"
#include "vexuf_sd_card.h"
#include "vexuf_timers.h"

void Error_Handler(void);
void ERROR_handleNoConfig(void);
void ERROR_handleSdError(void);

#endif  // INC_VEXUF_ERROR_H
