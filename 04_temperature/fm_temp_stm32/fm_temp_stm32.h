/* @file fm_temperature_sensor.h
 *
 * Este módulo se encarga de obtener la temperatura interna del micro, con el
 * objetivo de mostrarla en la pantalla lcd, en el menú "acm_temp". Para esto,
 * la formatea como un número entero.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_TEMPERATURE_SENSOR_H_
#define     FM_TEMPERATURE_SENSOR_H_

// includes

#include "main.h"
#include "string.h"
#include "stdio.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes

int fm_temp_stm32_format_int();
void fm_temp_stm32_format_str(char *ptr_str, int str_lgth);
void fm_temp_stm32_configure();

#endif /* FM_TEMPERATURE_SENSOR_H */

/*** end of file ***/
