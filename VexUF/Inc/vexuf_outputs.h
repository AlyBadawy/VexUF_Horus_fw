#ifndef VEXUF_OUTPUTS_H_
#define VEXUF_OUTPUTS_H_

#include "vexuf.h"

typedef struct {
  uint16_t haltOnSdCardErrors : 1;
  uint16_t log_info_to_sd : 1;
  uint16_t log_warn_to_sd : 1;
  uint16_t log_error_to_sd : 1;
  uint16_t reserved : 12;

} OutputConfiguration;

typedef struct {
  uint16_t buzz : 1;
  uint16_t info : 1;
  uint16_t log : 1;
  uint16_t pwm1_change : 1;
  uint16_t pwm2_change : 1;
  uint16_t reserved : 11;

} AlarmOrTrigOutput;

#endif  // VEXUF_OUTPUTS_H_