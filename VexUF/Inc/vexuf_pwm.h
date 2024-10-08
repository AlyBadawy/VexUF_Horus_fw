/*
 * vexuf_pwm.h
 *
 *  Created on: Aug 12, 2024
 *      Author: Aly Badawy
 */

#ifndef VEXUF_VEXUF_PWM_H_
#define VEXUF_VEXUF_PWM_H_

#include "vexuf.h"

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

UF_STATUS PWM_init(TIM_HandleTypeDef *pwm1, TIM_HandleTypeDef *pwm2);
UF_STATUS PWM_enable(PwmChannel channel);
UF_STATUS PWM_setDutyCycle(PwmChannel channel, uint16_t dutyCycle);
UF_STATUS PWM_disable(PwmChannel channel);
UF_STATUS PWM_deinit(void);

void PWM_handleCli(const char *args, char *responseBuffer);

#endif /* VEXUF_VEXUF_PWM_H_ */