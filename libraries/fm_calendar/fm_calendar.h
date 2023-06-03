/* @file fm_calendar.h
 *
 * Este modulo se encarga de recibir los datos de la fecha y hora actuales,
 * provenientes del periférico RTC, y de formatearlos para que puedan ser
 * correctamente impresos en la pantalla LCD.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_CALENDAR_H_
#define     FM_CALENDAR_H_

// includes

#include "main.h"
#include "../fm_debug/fm_debug.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes

void fm_calendar_format_date(char *p_str, int p_str_lgth);
void fm_calendar_format_time(char *p_str, int p_str_lgth);
void fm_calendar_get();
uint8_t fm_calendar_get_day();
uint8_t fm_calendar_get_hour();
uint8_t fm_calendar_get_minute();
uint8_t fm_calendar_get_month();
uint8_t fm_calendar_get_second();
uint8_t fm_calendar_get_year();
void fm_calendar_modify_date(RTC_DateTypeDef sdate_conf);
void fm_calendar_modify_time(RTC_TimeTypeDef stime_conf);

#endif /* FM_CALENDAR_H */

/*** end of file ***/
