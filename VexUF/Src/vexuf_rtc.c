/**
 ******************************************************************************
 * @file          : vexuf_rtc.c
 * @brief        : VexUF RTC Implementation
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
#include "vexuf_rtc.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "vexuf_config.h"

/* TypeDef -------------------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;

/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Extern Variables ----------------------------------------------------------*/
extern char *ok;

/* Variables -----------------------------------------------------------------*/
static const uint8_t list_mth[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
AlarmConfiguration alarms[2];

/* Prototypes ----------------------------------------------------------------*/

/* Code ----------------------------------------------------------------------*/
UF_STATUS RTC_InitAlarms(void) {
  if (CONFIG_getAlarmConf(&alarms[0], 0) != UF_OK) return UF_ERROR;
  if (CONFIG_getAlarmConf(&alarms[1], 1) != UF_OK) return UF_ERROR;

  return UF_OK;
}

UF_STATUS RTC_getDateTime(char *datetime) {
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  // Get the current date
  // Get the current time
  if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) return UF_ERROR;
  if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) return UF_ERROR;

  // Format the date and time as "MM/DD/YYYY HH:MM:SS"
  snprintf(datetime, 26, "%02d/%02d/%04d %02d:%02d:%02d", sDate.Month,
           sDate.Date, 2000 + sDate.Year, sTime.Hours, sTime.Minutes,
           sTime.Seconds);
  return UF_OK;
}

UF_STATUS RTC_setDateTime(const char *datetime) {
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  uint8_t dayOfWeek = 0;
  int tempYear = 0;

  // Ensure the input string is in the correct format (19 characters long)
  if (strlen(datetime) != 19) return UF_ERROR;

  // Parse the input string
  int month = atoi(&datetime[0]);
  int day = atoi(&datetime[3]);
  int year = atoi(&datetime[6]);
  int hours = atoi(&datetime[11]);
  int minutes = atoi(&datetime[14]);
  int seconds = atoi(&datetime[17]);

  // Validate the parsed values
  if (month < 1 || month > 12 || day < 1 || day > 31 || year < 2000 ||
      year > 2099 || hours < 0 || hours > 23 || minutes < 0 || minutes > 59 ||
      seconds < 0 || seconds > 59) {
    return UF_ERROR;  // Return an error if any value is out of range
  }

  tempYear = year;
  if (month < 3) tempYear -= 1;
  dayOfWeek = (uint8_t)((tempYear + (tempYear / 4) - (tempYear / 100) +
                         (tempYear / 400) + list_mth[month - 1] + day) %
                        7);

  // Set the date structure
  sDate.Month = month;
  sDate.Date = day;
  sDate.WeekDay = dayOfWeek;
  sDate.Year = year - 2000;  // RTC expects the year as an offset from 2000

  // Set the time structure
  sTime.Hours = hours;
  sTime.Minutes = minutes;
  sTime.Seconds = seconds;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) return UF_ERROR;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) return UF_ERROR;

  return UF_OK;
}

void RTC_handleCli(const char *args, char *responseBuffer) {
  // Check if args is empty or contains only whitespace
  while (*args && isspace((unsigned char)*args)) args++;

  if (*args == '\0') {
    // Getter: Show current date and time
    char datetime[20];
    RTC_getDateTime(datetime);
    sprintf(responseBuffer, "Date/Time: %s%s", datetime, ok);
  } else {
    // Setter: Set new date and time
    RTC_setDateTime(args);
    sprintf(responseBuffer, "Date and Time set...%s", ok);
  }
}