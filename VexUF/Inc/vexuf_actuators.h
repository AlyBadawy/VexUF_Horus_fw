/*
 * vexuf_actuators.h
 *
 *  Created on: Jul 22, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_ACTUATORS_H_
#define INC_ACTUATORS_H_

#include "vexuf.h"

typedef enum { ActOff = 0, ActOn = 1, ActUnchanged = 2 } ActLevel;

// Define an enum for the shift register pins
typedef enum {
  ACT_PIN_A1 = 0,
  ACT_PIN_A2,
  ACT_PIN_A3,
  ACT_PIN_A4,
  ACT_PIN_A5,
  ACT_PIN_A6,
  ACT_PIN_A7,
  ACT_PIN_A8
} ActuatorPin;

// disabled, enabled on, enabled off,

typedef struct {
  uint16_t act1 : 2;
  uint16_t act2 : 2;
  uint16_t act3 : 2;
  uint16_t act4 : 2;
  uint16_t act5 : 2;
  uint16_t act6 : 2;
  uint16_t act7 : 2;
  uint16_t act8 : 2;
} ActuatorsValues;

typedef struct {
  uint16_t actuators_enabled : 1;
  uint16_t actuators_lights_enabled : 1;
  uint16_t reserved : 14;
} ActuatorsConfiguration;

// Function prototypes
UF_STATUS ACT_Init(void);

UF_STATUS ACTUATORS_Update(void);

UF_STATUS ACTUATORS_setLights(uint8_t state);

UF_STATUS ACTUATORS_trigger(ActuatorsValues values);

UF_STATUS ACT_DeInit(void);

void ACTUATORS_Test(void);

#endif /* INC_VEXUF_ACTUATORS_H_ */