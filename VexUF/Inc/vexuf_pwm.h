/*
 * vexuf_pwm.h
 *
 *  Created on: Aug 12, 2024
 *      Author: Aly Badawy
 */

#ifndef VEXUF_VEXUF_PWM_H_
#define VEXUF_VEXUF_PWM_H_

#include "vexuf_helpers.h"

typedef enum {
  PwmChannel1 = 0,
  PwmChannel2 = 1,
} PwmChannel;

typedef struct {
  uint8_t pwm1Enabled;
  uint8_t pwm2Enabled;
  uint16_t pwm1Value;
  uint16_t pwm2Value;
} PwmConfiguration;

void PWM_init(void);
void PWM_enable(PwmChannel channel);
void PWM_setDutyCycle(PwmChannel channel, uint16_t dutyCycle);
void PWM_disable(PwmChannel channel);
void PWM_deinit(void);

#endif /* VEXUF_VEXUF_PWM_H_ */