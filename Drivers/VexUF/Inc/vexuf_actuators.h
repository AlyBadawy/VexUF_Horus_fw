/*
 * actuators.h
 *
 *  Created on: Jul 22, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_ACTUATORS_H_
#define INC_ACTUATORS_H_

#include "main.h"
#define  NUMBER_OF_Actuators			8

typedef enum {ActOff = 0, ActOn = 1, ActUnchanged = 2} ActStatus;

// Define an enum for the shift register pins
typedef enum {
    ACT_PIN_A1 = 0,
    ACT_PIN_A2 = 1,
    ACT_PIN_A3 = 2,
    ACT_PIN_A4 = 3,
    ACT_PIN_A5 = 4,
    ACT_PIN_A6 = 5,
    ACT_PIN_A7 = 6,
    ACT_PIN_A8 = 7
} ActuatorPin;

//disabled, enabled on, enabled off,

typedef struct {
	uint16_t act1: 2;
	uint16_t act2: 2;
	uint16_t act3: 2;
	uint16_t act4: 2;
	uint16_t act5: 2;
	uint16_t act6: 2;
	uint16_t act7: 2;
	uint16_t act8: 2;
} ActuatorsValues;

// Function prototypes
void ACTUATORS_updateShiftReg(void);
void ACTUATORS_Test(void); // TODO: Remove before release

#endif /* INC_VEXUF_ACTUATORS_H_ */