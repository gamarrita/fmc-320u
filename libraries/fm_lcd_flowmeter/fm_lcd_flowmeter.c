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

/*
 * The names of all new data types, including structures, unions, and
 * enumerations, shall consist only of lowercase characters and internal
 * underscores and end with ‘_t’.
 *
 * All new structures, unions, and enumerations shall be named via a typedef.
 *
 */

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

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
