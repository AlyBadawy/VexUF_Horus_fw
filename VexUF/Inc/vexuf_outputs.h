#ifndef VEXUF_OUTPUTS_H_
#define VEXUF_OUTPUTS_H_

#include "vexuf_helpers.h"

typedef struct {
  uint16_t haltOnSdCardErrors : 1;
  uint16_t log_info_to_sd : 1;
  uint16_t log_warn_to_sd : 1;
  uint16_t log_error_to_sd : 1;
  uint16_t reserved : 12;

} OutputConfiguration;

#endif  // VEXUF_OUTPUTS_H_