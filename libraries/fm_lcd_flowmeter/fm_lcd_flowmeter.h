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

typedef enum
{
    LCD_FLOWMETER_RES_0,
    LCD_FLOWMETER_RES_1,
    LCD_FLOWMETER_RES_2,
    LCD_FLOWMETER_RES_3
} lcd_flometer_res_t;

typedef struct
{
    uint8_t dd;
    uint8_t mm;
    uint8_t aaaa;
} date_freeze_t;

typedef struct
{
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
} time_freeze_t;

typedef enum
{
    FIELD_DAY, FIELD_MONTH, FIELD_YEAR, FIELD_HOUR, FIELD_MINUTE, FIELD_SECOND
} sel_date_time_field_t;

typedef enum
{
    K_DIG_0, K_DIG_1, K_DIG_2, K_DIG_3, K_DIG_4, K_DIG_5, K_DIG_6, K_DIG_7
} sel_k_digit_t;

// Defines.

// Function prototypes

void fm_lcd_flowmeter_acm_rate();
void fm_lcd_flowmeter_acm_temp();
void fm_lcd_flowmeter_date_hour();
void fm_lcd_flowmeter_freeze_date_hour(sel_date_time_field_t date_time_field);
void fm_lcd_flowmeter_k_factor(sel_k_digit_t k_digit_modify);
void fm_lcd_flowmeter_k_lin(sel_lin_digit_t k_digit_modify, uint8_t k_sel);
void fm_lcd_flowmeter_password(uint8_t password_index);
void fm_lcd_flowmeter_ttl_rate();
void fm_lcd_flowmeter_units_tim();
void fm_lcd_flowmeter_units_vol();
void fm_lcd_flowmeter_version();

#endif /* FM_LCD_FLOWMETER_H */

/*** end of file ***/
