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
#include "../fmc/fmc.h"
#include "../fm_lcd_module/fm_lcd/fm_lcd.h"
#include "../fm_temp_stm32/fm_temp_stm32.h"
#include "../fm_calendar/fm_calendar.h"
#include "../fm_keypad/fm_keypad.h"
#include "../fm_version/fm_version.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes

void fm_lcd_flowmeter_acm_rate();
void fm_lcd_flowmeter_acm_temp();
void fm_lcd_flowmeter_date_hour();
void fm_lcd_flowmeter_ttl_rate();
void fm_lcd_flowmeter_version();

#endif /* FM_LCD_FLOWMETER_H */

/*** end of file ***/
