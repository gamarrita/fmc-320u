/* @file fm_module.c
 *
 * @brief
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_lcd_flowmeter.h"

// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

uint8_t blink = 1;

fm_event_t actual_event;
fm_event_t previous_event;

time_freeze_t time_freeze;
date_freeze_t date_freeze;

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que compacta la visibilidad del menú de usuario acm rate.
 * @param None
 * @retval None
 */
void fm_lcd_flowmeter_acm_rate()
{
    static char lcd_msg[PCF8553_DATA_SIZE];
    static char lcd_msg_unit_time[PCF8553_DATA_SIZE];
    static char lcd_msg_unit_volume[PCF8553_DATA_SIZE];

    fp_to_str(fmc_get_acm().volume, ' ', LINE_0_DIGITS, lcd_msg,
    sizeof(lcd_msg));
    fp_add_dot(fmc_get_acm().volume, lcd_msg, sizeof(lcd_msg));
    fm_lcd_puts_rows(lcd_msg, HIGH_ROW);

    fp_to_str(fmc_get_rate().volume, ' ', LINE_1_DIGITS, lcd_msg,
    sizeof(lcd_msg));
    fp_add_dot(fmc_get_rate().volume, lcd_msg, sizeof(lcd_msg));
    fm_lcd_puts_rows(lcd_msg, LOW_ROW);

    fm_lcd_puts_symbol("ACM");
    fm_lcd_puts_symbol("RATE");

    fm_factory_get_units_volume(lcd_msg_unit_volume, PCF8553_DATA_SIZE);
    fm_lcd_puts_unit_volume(lcd_msg_unit_volume);

    fm_lcd_puts_symbol("BACKSLASH");

    fm_factory_get_units_time(lcd_msg_unit_time, PCF8553_DATA_SIZE);
    fm_lcd_puts_unit_time(lcd_msg_unit_time);
}

/*
 * @brief Función que compacta la visibilidad del menú de usuario acm
 * temperature.
 * @param None
 * @retval None
 */
void fm_lcd_flowmeter_acm_temp()
{
    static char lcd_msg[PCF8553_DATA_SIZE];
    static char temp_str[PCF8553_DATA_SIZE];

    fp_to_str(fmc_get_acm().volume, ' ', LINE_0_DIGITS, lcd_msg,
    sizeof(lcd_msg));
    fp_add_dot(fmc_get_acm().volume, lcd_msg, sizeof(lcd_msg));
    fm_lcd_puts_rows(lcd_msg, HIGH_ROW);

    fm_temp_stm32_format_str(temp_str, PCF8553_DATA_SIZE);
    fm_lcd_puts_rows(temp_str, LOW_ROW);

    fm_lcd_puts_symbol("ACM");

    fm_lcd_puts_unit_volume("oC");
}

/*
 * @brief Función que compacta la visibilidad del menú de usuario date hour.
 * @param None
 * @retval None
 */
void fm_lcd_flowmeter_date_hour()
{
    static char date_string[PCF8553_DATA_SIZE];
    static char time_string[PCF8553_DATA_SIZE];

    fm_calendar_format_date(date_string, PCF8553_DATA_SIZE);
    fm_lcd_puts_rows(date_string, HIGH_ROW);

    fm_calendar_format_time(time_string, PCF8553_DATA_SIZE);
    fm_lcd_puts_rows(time_string, LOW_ROW);

    lcd_set_point(HIGH_ROW, PNT_3);
    lcd_set_point(HIGH_ROW, PNT_1);
    lcd_set_point(LOW_ROW, PNT_3);
    lcd_set_point(LOW_ROW, PNT_1);
}

/*
 * @brief Función que compacta la visibilidad del menú de configuración de fecha
 * y hora.
 * @param Campo a modificar, el cual debe parpadear cada 1 segundo.
 * @retval None
 */
void fm_lcd_flowmeter_freeze_date_hour(sel_date_time_field_t date_time_field)
{
    static char date_string[PCF8553_DATA_SIZE];
    static char time_string[PCF8553_DATA_SIZE];

    snprintf(date_string, strlen(date_string), "%02u%02u20%02u", date_freeze.dd,
    date_freeze.mm, date_freeze.aaaa);
    fm_lcd_puts_rows(date_string, HIGH_ROW);

    snprintf(time_string, strlen(time_string), "%02u%02u%02u", time_freeze.hh,
    time_freeze.mm, time_freeze.ss);
    fm_lcd_puts_rows(time_string, LOW_ROW);

    lcd_set_point(HIGH_ROW, PNT_3);
    lcd_set_point(HIGH_ROW, PNT_1);
    lcd_set_point(LOW_ROW, PNT_3);
    lcd_set_point(LOW_ROW, PNT_1);

    if (actual_event == EVENT_KEY_ENTER
    || (previous_event == EVENT_KEY_ENTER && actual_event == EVENT_LCD_REFRESH))
    {
        blink = 1;
    }
    else if (actual_event == EVENT_KEY_DOWN
    || (previous_event == EVENT_KEY_DOWN && actual_event == EVENT_LCD_REFRESH)
    || actual_event == EVENT_KEY_UP
    || (previous_event == EVENT_KEY_UP && actual_event == EVENT_LCD_REFRESH))
    {
        blink = 0;
    }

    if (blink == 1)
    {
        if (date_time_field == FIELD_DAY)
        {
            lcd_clear_digit(DIGIT_1, HIGH_ROW);
            lcd_clear_digit(DIGIT_0, HIGH_ROW);
        }
        else if (date_time_field == FIELD_MONTH)
        {
            lcd_clear_digit(DIGIT_3, HIGH_ROW);
            lcd_clear_digit(DIGIT_2, HIGH_ROW);
        }
        else if (date_time_field == FIELD_YEAR)
        {
            lcd_clear_digit(DIGIT_7, HIGH_ROW);
            lcd_clear_digit(DIGIT_6, HIGH_ROW);
            lcd_clear_digit(DIGIT_5, HIGH_ROW);
            lcd_clear_digit(DIGIT_4, HIGH_ROW);
        }
        else if (date_time_field == FIELD_HOUR)
        {
            lcd_clear_digit(DIGIT_1, LOW_ROW);
            lcd_clear_digit(DIGIT_0, LOW_ROW);
        }
        else if (date_time_field == FIELD_MINUTE)
        {
            lcd_clear_digit(DIGIT_3, LOW_ROW);
            lcd_clear_digit(DIGIT_2, LOW_ROW);
        }
        else if (date_time_field == FIELD_SECOND)
        {
            lcd_clear_digit(DIGIT_5, LOW_ROW);
            lcd_clear_digit(DIGIT_4, LOW_ROW);
        }

        blink = 0;
    }
    else
    {
        blink = 1;
    }
}

/*
 * @brief Función que compacta la visibilidad del menú de configuración de
 * contraseña.
 * @param Dígito actual a introducir de la contraseña.
 * @retval None
 */
void fm_lcd_flowmeter_password(uint8_t password_index)
{
    fm_lcd_puts_rows(" PASS", HIGH_ROW);

    if (password_index == 1)
    {
        fm_lcd_puts_rows("8", LOW_ROW);
    }
    else if (password_index == 2)
    {
        fm_lcd_puts_rows("88", LOW_ROW);
    }
    else if (password_index == 3) // @suppress("Avoid magic numbers")
    {
        fm_lcd_puts_rows("888", LOW_ROW);
    }
}

/*
 * @brief Función que compacta la visibilidad del menú de usuario ttl rate.
 * @param None
 * @retval None
 */
void fm_lcd_flowmeter_ttl_rate()
{
    static char lcd_msg[PCF8553_DATA_SIZE];
    static char lcd_msg_unit_time[PCF8553_DATA_SIZE];
    static char lcd_msg_unit_volume[PCF8553_DATA_SIZE];

    fp_to_str(fmc_get_ttl().volume, ' ', LINE_0_DIGITS, lcd_msg,
    sizeof(lcd_msg));
    fp_add_dot(fmc_get_ttl().volume, lcd_msg, sizeof(lcd_msg));
    fm_lcd_puts_rows(lcd_msg, HIGH_ROW);

    fp_to_str(fmc_get_rate().volume, ' ', LINE_1_DIGITS, lcd_msg,
    sizeof(lcd_msg));
    fp_add_dot(fmc_get_rate().volume, lcd_msg, sizeof(lcd_msg));
    fm_lcd_puts_rows(lcd_msg, LOW_ROW);

    fm_lcd_puts_symbol("TTL");
    fm_lcd_puts_symbol("RATE");

    fm_factory_get_units_volume(lcd_msg_unit_volume, PCF8553_DATA_SIZE);
    fm_lcd_puts_unit_volume(lcd_msg_unit_volume);

    fm_lcd_puts_symbol("BACKSLASH");

    fm_factory_get_units_time(lcd_msg_unit_time, PCF8553_DATA_SIZE);
    fm_lcd_puts_unit_time(lcd_msg_unit_time);
}

/*
 * @brief Función que compacta la visibilidad del menú de configuración de
 * unidades de tiempo.
 * @param None
 * @retval None
 */
void fm_lcd_flowmeter_units_tim()
{
    static char lcd_msg_unit_time[PCF8553_DATA_SIZE];
    static point_t actual_res;

    fm_lcd_puts_rows("0000000", LOW_ROW);

    if (actual_event != EVENT_LCD_REFRESH
    || (previous_event != EVENT_LCD_REFRESH && actual_event == EVENT_LCD_REFRESH))
    {
        blink = 0;
    }

    if (fm_factory_get_res_rate() == LCD_FLOWMETER_RES_1)
    {
        actual_res = PNT_5;
    }
    else if (fm_factory_get_res_rate() == LCD_FLOWMETER_RES_2)
    {
        actual_res = PNT_4;
    }
    else if (fm_factory_get_res_rate() == LCD_FLOWMETER_RES_3)
    {
        actual_res = PNT_3;
    }

    if (blink == 1)
    {
        lcd_clear_point(LOW_ROW, actual_res);
        fm_lcd_puts_unit_time(" ");

        blink = 0;
    }
    else
    {
        if (fm_factory_get_res_rate() != LCD_FLOWMETER_RES_0)
        {
            lcd_set_point(LOW_ROW, actual_res);
        }
        else
        {
            lcd_clear_point(LOW_ROW, actual_res);
        }
        fm_factory_get_units_time(lcd_msg_unit_time, PCF8553_DATA_SIZE);
        fm_lcd_puts_unit_time(lcd_msg_unit_time);

        blink = 1;
    }

}

/*
 * @brief Función que compacta la visibilidad del menú de configuración de
 * unidades de volumen.
 * @param None
 * @retval None
 */
void fm_lcd_flowmeter_units_vol()
{
    static char lcd_msg_unit_volume[PCF8553_DATA_SIZE];
    static point_t actual_res;

    fm_lcd_puts_rows("00000000", HIGH_ROW);

    if (actual_event != EVENT_LCD_REFRESH
    || (previous_event != EVENT_LCD_REFRESH && actual_event == EVENT_LCD_REFRESH))
    {
        blink = 0;
    }

    if (fm_factory_get_res_acm_ttl() == LCD_FLOWMETER_RES_1)
    {
        actual_res = PNT_6;
    }
    else if (fm_factory_get_res_acm_ttl() == LCD_FLOWMETER_RES_2)
    {
        actual_res = PNT_5;
    }
    else if (fm_factory_get_res_acm_ttl() == LCD_FLOWMETER_RES_3)
    {
        actual_res = PNT_4;
    }

    if (blink == 1)
    {
        lcd_clear_point(HIGH_ROW, actual_res);
        fm_lcd_puts_unit_volume("  ");

        blink = 0;
    }
    else
    {
        if (fm_factory_get_res_acm_ttl() != LCD_FLOWMETER_RES_0)
        {
            lcd_set_point(HIGH_ROW, actual_res);
        }
        else
        {
            lcd_clear_point(HIGH_ROW, actual_res);
        }
        fm_factory_get_units_volume(lcd_msg_unit_volume, PCF8553_DATA_SIZE);
        fm_lcd_puts_unit_volume(lcd_msg_unit_volume);

        blink = 1;
    }

    fm_lcd_puts_symbol("ACM");
    fm_lcd_puts_symbol("TTL");
}

/*
 * @brief Función que compacta la visibilidad de la versión del firmware.
 * @param None
 * @retval None
 */
void fm_lcd_flowmeter_version()
{
    char version_str[PCF8553_DATA_SIZE];

    fm_version_get(version_str, PCF8553_DATA_SIZE);
    fm_lcd_puts_rows(version_str, LOW_ROW);

    fm_lcd_puts_unit_volume("VE");
}

// Interrupts

/*** end of file ***/
