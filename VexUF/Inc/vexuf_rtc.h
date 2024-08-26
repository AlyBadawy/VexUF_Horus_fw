#ifndef VEXUF_RTC_H
#define VEXUF_RTC_H

#include "vexuf_actuators.h"
#include "vexuf_helpers.h"
#include "vexuf_trigs.h"

typedef struct {
  ActuatorsValues actuators;
  AlarmOrTrigOutput output;
  uint16_t pwm1;  // 16 bits, 0xffff = no change
  uint16_t pwm2;  // 16 bits, 0xffff = no change
  uint16_t enabled : 1;
  uint16_t tncEnabled : 1;
  uint16_t tncMessage : 4;
  uint16_t tncPath : 3;
  uint16_t reserved : 7;

} AlarmConfiguration;

void REALTIME_GetDateTime(char *datetime);
int REALTIME_SetDateTime(const char *datetime);

#endif  // VEXUF_RTC_H