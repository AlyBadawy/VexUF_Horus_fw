/*
 * actuators.c
 *
 *  Created on: Jul 22, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_actuators.h"

void ACTUATORS_setPin(ActuatorPin pin);
void ACTUATORS_resetPin(ActuatorPin pin);

uint8_t actuatorsData = 0;

void ACTUATORS_updateShiftReg(void) {
    for (int i = 0; i < 8; i++) {
        // Write the data bit by bit
        HAL_GPIO_WritePin(ActDa_GPIO_Port, ActDa_Pin, (actuatorsData & (1 << (7 - i))) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        // Toggle the clock pin
        HAL_GPIO_WritePin(ActCk_GPIO_Port, ActCk_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ActCk_GPIO_Port, ActCk_Pin, GPIO_PIN_RESET);
    }
    
    // Toggle the latch pin to output the data
    HAL_GPIO_WritePin(ActLa_GPIO_Port, ActLa_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ActLa_GPIO_Port, ActLa_Pin, GPIO_PIN_RESET);
}

void ACTUATORS_setPin(ActuatorPin pin) {
    	actuatorsData |= (1 << (pin));
}

void ACTUATORS_resetPin(ActuatorPin pin) {
    	actuatorsData &= ~(1 << (pin));
}

void ACTUATORS_lights(GPIO_PinState state) {
	HAL_GPIO_WritePin(ActInd_GPIO_Port, ActInd_Pin, state);
}

void ACTUATORS_trigegr(ActuatorsValues values) {
	// Array of ActuatorPin values corresponding to each act1, act2, ..., act8
	const ActuatorPin pins[] = {
			ACT_PIN_A1, ACT_PIN_A2, ACT_PIN_A3, ACT_PIN_A4,
      ACT_PIN_A5, ACT_PIN_A6, ACT_PIN_A7, ACT_PIN_A8
	};

	// Array of the actN fields in the values struct
	uint16_t actValues[] = {
			values.act1, values.act2, values.act3, values.act4,
	        values.act5, values.act6, values.act7, values.act8
	};

	for (int i = 0; i < 8; i++) {
		if (actValues[i] == ActOn) {
			ACTUATORS_setPin(pins[i]);
		} else if (actValues[i] == ActOff) {
			ACTUATORS_resetPin(pins[i]);
		}
	}
}

void ACTUATORS_Test(void) { // TODO: Remove before release
	ACTUATORS_lights(GPIO_PIN_SET);
	for(ActuatorPin pin = ACT_PIN_A1; pin <= ACT_PIN_A8; pin++) {
		ACTUATORS_setPin(pin);
		ACTUATORS_updateShiftReg();
		HAL_Delay(50);
		ACTUATORS_resetPin(pin);
		ACTUATORS_updateShiftReg();
		HAL_Delay(50);
	}
	ACTUATORS_lights(GPIO_PIN_RESET);
}