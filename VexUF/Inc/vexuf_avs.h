/*
 * adc_avs.h
 *
 *  Created on: Jul 27, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_VEXUF_AVS_H_
#define INC_VEXUF_AVS_H_

#include "vexuf.h"

#define NUMBER_OF_AVS 3

typedef struct {
  uint16_t enabled : 1;
  uint16_t statusSlow : 1;
  uint16_t statusFast : 1;
  uint16_t statusOn : 1;
  uint16_t reserved : 12;  // TODO: flags for indicators being enabled/disabled
  uint16_t minSlow;
  uint16_t maxSlow;
  uint16_t minFast;
  uint16_t maxFast;
  uint16_t minOn;
  uint16_t maxOn;
} AvSensor;

UF_STATUS AVS_Init(void);

UF_STATUS AVS_Scan(void);

void AVS_Test(void);

void AVS_handleCli(const char *args, char *responseBuffer);

#endif /* INC_VEXUF_AVS_H_ */