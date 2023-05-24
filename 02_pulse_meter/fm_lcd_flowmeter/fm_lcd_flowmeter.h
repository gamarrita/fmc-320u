/* @file fm_lcd_flowmeter.h
 *
 * @brief Este módulo es el encargado de utilizar las funciones de manejo del
 * lcd para imprimir en pantalla cosas relacionadas al caudalímetro. Toma sus
 * datos para imprimir de fm_factory y fmc, y también utiliza formateos de datos
 * presentes en fm_math_fp.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef FM_LCD_FLOWMETER_H_
#define FM_LCD_FLOWMETER_H_

// includes

#include "main.h"
#include "../fm_lcd/fm_lcd.h"
#include "../fm_math_fp/fm_math_fp.h"
#include "../fmc/fmc.h"
#include "../frecuency_meter/frecuency_meter.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes

void fm_lcd_flowmeter_acm_rate();
void fm_lcd_flowmeter_ttl_rate();

#endif /* FM_LCD_FLOWMETER_H */

/*** end of file ***/
