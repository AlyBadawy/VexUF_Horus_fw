/*
 * vexuf_error.c
 *
 *  Created on: Aug 23, 2024
 *      Author: Aly Badawy
 */

#include "vexuf_error.h"

void Error_Handler(void) {
  __disable_irq();
  while(1);
}