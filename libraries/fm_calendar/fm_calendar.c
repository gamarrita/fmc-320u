/* @file fm_calendar.c
 *
 * Este modulo se encarga de recibir los datos de la fecha y hora actuales,
 * provenientes del periférico RTC, y de formatearlos para que puedan ser
 * correctamente impresos en la pantalla LCD.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_calendar.h"
#include "stdio.h"
#include "stdlib.h"

// Typedef.

/*
 * The names of all new data types, including structures, unions, and
 * enumerations, shall consist only of lowercase characters and internal
 * underscores and end with ‘_t’.
 *
 * All new structures, unions, and enumerations shall be named via a typedef.
 *
 */

// Const data.
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

const int array_length_calendar = 20;

extern RTC_HandleTypeDef hrtc;

// Defines.

// Debug.

/*
 * To temporally disable a block of code, use preprocessor's conditional
 * compilation features, eg, the following one should be used to increase the
 * the debug output information.
 *
 */
#ifndef NDEBUG
#endif

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Formatea la fecha en el formato dd.mm.aaaa y lo mete en un punto fijo
 * en fm_factory.
 * @param None
 * @retval None
 */
void fm_calendar_format_date(char *p_str, int p_str_lgth)
{
    fm_calendar_get();
    sprintf(p_str, "%02d%02d20%02d", sDate.Date, sDate.Month,
    sDate.Year);
}

/*
 * @brief Formatea la hora en el formato hh.mm.ss y lo mete en un punto fijo en
 * fm_factory.
 * @param None
 * @retval None
 */
void fm_calendar_format_time(char *p_str, int p_str_lgth)
{
    fm_calendar_get();
    sprintf(p_str, "%02d%02d%02d", sTime.Hours, sTime.Minutes,
    sTime.Seconds);
}

/*
 * @brief Toma los valores de fecha y hora del calendario del RTC, pero no los
 * usa.
 * @param None
 * @retval None
 */
void fm_calendar_get()
{
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/*
 * @brief Función que lee el día desde el RTC y lo devuelve como parámetro.
 * @param None
 * @retval Día leido del calendario.
 */
int fm_calendar_get_day()
{
    fm_calendar_get();
    return (sDate.Date);
}

/*
 * @brief Función que lee la hora desde el RTC y la devuelve como parámetro.
 * @param None
 * @retval Hora leida del calendario.
 */
int fm_calendar_get_hour()
{
    fm_calendar_get();
    return (sTime.Hours);
}

/*
 * @brief Función que lee los minutos desde el RTC y los devuelve como
 * parámetro.
 * @param None
 * @retval Minutos leidos del calendario.
 */
int fm_calendar_get_minute()
{
    fm_calendar_get();
    return (sTime.Minutes);
}

/*
 * @brief Función que lee el mes desde el RTC y lo devuelve como parámetro.
 * @param None
 * @retval Mes leido del calendario.
 */
int fm_calendar_get_month()
{
    fm_calendar_get();
    return (sDate.Month);
}

/*
 * @brief Función que lee los segundos desde el RTC y los devuelve como
 * parámetro.
 * @param None
 * @retval Segundos leidos del calendario.
 */
int fm_calendar_get_second()
{
    fm_calendar_get();
    return (sTime.Seconds);
}

/*
 * @brief Función que lee el año desde el RTC y lo devuelve como parámetro.
 * @param None
 * @retval Año leido del calendario.
 */
int fm_calendar_get_year()
{
    fm_calendar_get();
    return (sDate.Year);
}

/*
 * @brief Función que permite modificar la fecha actual del RTC.
 * @param Estructura de fecha del RTC.
 * @retval None
 */
void fm_calendar_modify_date(RTC_DateTypeDef sdate_conf)
{
    HAL_RTC_SetDate(&hrtc, &sdate_conf, RTC_FORMAT_BIN);
}

/*
 * @brief Función que permite modificar la hora actual del RTC.
 * @param Estructura de hora del RTC.
 * @retval None
 */
void fm_calendar_modify_time(RTC_TimeTypeDef stime_conf)
{
    HAL_RTC_SetTime(&hrtc, &stime_conf, RTC_FORMAT_BIN);
}

// Interrupts

/*** end of file ***/

