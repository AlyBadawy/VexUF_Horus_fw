/**
 ******************************************************************************
 * @file          : filename.c
 * @brief        : Brief description
 ******************************************************************************
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @copyright     : Aly Badawy
 * @author website: https://alybadawy.com
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "vexuf_tamper.h"

#include "rtc.h"

/* TypeDef -------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern VexufStatus vexufStatus;

/* Variables -----------------------------------------------------------------*/

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef* hrtc) {
  UNUSED(hrtc);
  // Handle tamper event: log, trigger actions, etc.
  // Example action:
  vexufStatus.tampered = 1;

  // HAL clears the tamper flag automatically; additional handling can be done
  // here
}

/* Private Methods -----------------------------------------------------------*/