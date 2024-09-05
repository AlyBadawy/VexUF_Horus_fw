#ifndef VEXUF_RTC_H
#define VEXUF_RTC_H

#include "vexuf.h"
#include "vexuf_actuators.h"
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

UF_STATUS RTC_getDateTime(char *datetime);
UF_STATUS RTC_setDateTime(const char *datetime);

void RTC_handleCli(const char *args, char *responseBuffer);

#endif  // VEXUF_RTC_H