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

// Interrupts

/*** end of file ***/
