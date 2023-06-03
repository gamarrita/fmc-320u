/* @file fm_menu_config.h
 *
 * @brief A description of the module’s purpose.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_MENU_CONFIG_H_
#define     FM_MENU_CONFIG_H_

// includes

#include "main.h"
#include "../fm_lcd_flowmeter/fm_lcd_flowmeter.h"
#include "../fm_menu_user/fm_menu_user.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

typedef enum
{
    PASS_0, PASS_UP, PASS_DOWN, PASS_ENTER, PASS_ESC
} password_digit_t;

typedef enum
{
    DAY_1 = 1,
    DAY_2,
    DAY_3,
    DAY_4,
    DAY_5,
    DAY_6,
    DAY_7,
    DAY_8,
    DAY_9,
    DAY_10,
    DAY_11,
    DAY_12,
    DAY_13,
    DAY_14,
    DAY_15,
    DAY_16,
    DAY_17,
    DAY_18,
    DAY_19,
    DAY_20,
    DAY_21,
    DAY_22,
    DAY_23,
    DAY_24,
    DAY_25,
    DAY_26,
    DAY_27,
    DAY_28,
    DAY_29,
    DAY_30,
    DAY_31
} sel_day;

typedef enum
{
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} sel_month;

typedef enum
{
    YEAR_0,
    YEAR_1,
    YEAR_2,
    YEAR_3,
    YEAR_4,
    YEAR_5,
    YEAR_6,
    YEAR_7,
    YEAR_8,
    YEAR_9,
    YEAR_10,
    YEAR_11,
    YEAR_12,
    YEAR_13,
    YEAR_14,
    YEAR_15,
    YEAR_16,
    YEAR_17,
    YEAR_18,
    YEAR_19,
    YEAR_20,
    YEAR_21,
    YEAR_22,
    YEAR_23,
    YEAR_24,
    YEAR_25,
    YEAR_26,
    YEAR_27,
    YEAR_28,
    YEAR_29,
    YEAR_30,
    YEAR_31,
    YEAR_32,
    YEAR_33,
    YEAR_34,
    YEAR_35,
    YEAR_36,
    YEAR_37,
    YEAR_38,
    YEAR_39,
    YEAR_40,
    YEAR_41,
    YEAR_42,
    YEAR_43,
    YEAR_44,
    YEAR_45,
    YEAR_46,
    YEAR_47,
    YEAR_48,
    YEAR_49,
    YEAR_50,
    YEAR_51,
    YEAR_52,
    YEAR_53,
    YEAR_54,
    YEAR_55,
    YEAR_56,
    YEAR_57,
    YEAR_58,
    YEAR_59,
    YEAR_60,
    YEAR_61,
    YEAR_62,
    YEAR_63,
    YEAR_64,
    YEAR_65,
    YEAR_66,
    YEAR_67,
    YEAR_68,
    YEAR_69,
    YEAR_70,
    YEAR_71,
    YEAR_72,
    YEAR_73,
    YEAR_74,
    YEAR_75,
    YEAR_76,
    YEAR_77,
    YEAR_78,
    YEAR_79,
    YEAR_80,
    YEAR_81,
    YEAR_82,
    YEAR_83,
    YEAR_84,
    YEAR_85,
    YEAR_86,
    YEAR_87,
    YEAR_88,
    YEAR_89,
    YEAR_90,
    YEAR_91,
    YEAR_92,
    YEAR_93,
    YEAR_94,
    YEAR_95,
    YEAR_96,
    YEAR_97,
    YEAR_98,
    YEAR_99
} sel_year;

typedef enum
{
    HOUR_0,
    HOUR_1,
    HOUR_2,
    HOUR_3,
    HOUR_4,
    HOUR_5,
    HOUR_6,
    HOUR_7,
    HOUR_8,
    HOUR_9,
    HOUR_10,
    HOUR_11,
    HOUR_12,
    HOUR_13,
    HOUR_14,
    HOUR_15,
    HOUR_16,
    HOUR_17,
    HOUR_18,
    HOUR_19,
    HOUR_20,
    HOUR_21,
    HOUR_22,
    HOUR_23
} sel_hour;

typedef enum
{
    MINU_0,
    MINU_1,
    MINU_2,
    MINU_3,
    MINU_4,
    MINU_5,
    MINU_6,
    MINU_7,
    MINU_8,
    MINU_9,
    MINU_10,
    MINU_11,
    MINU_12,
    MINU_13,
    MINU_14,
    MINU_15,
    MINU_16,
    MINU_17,
    MINU_18,
    MINU_19,
    MINU_20,
    MINU_21,
    MINU_22,
    MINU_23,
    MINU_24,
    MINU_25,
    MINU_26,
    MINU_27,
    MINU_28,
    MINU_29,
    MINU_30,
    MINU_31,
    MINU_32,
    MINU_33,
    MINU_34,
    MINU_35,
    MINU_36,
    MINU_37,
    MINU_38,
    MINU_39,
    MINU_40,
    MINU_41,
    MINU_42,
    MINU_43,
    MINU_44,
    MINU_45,
    MINU_46,
    MINU_47,
    MINU_48,
    MINU_49,
    MINU_50,
    MINU_51,
    MINU_52,
    MINU_53,
    MINU_54,
    MINU_55,
    MINU_56,
    MINU_57,
    MINU_58,
    MINU_59
} sel_minute;

typedef enum
{
    SEC_0,
    SEC_1,
    SEC_2,
    SEC_3,
    SEC_4,
    SEC_5,
    SEC_6,
    SEC_7,
    SEC_8,
    SEC_9,
    SEC_10,
    SEC_11,
    SEC_12,
    SEC_13,
    SEC_14,
    SEC_15,
    SEC_16,
    SEC_17,
    SEC_18,
    SEC_19,
    SEC_20,
    SEC_21,
    SEC_22,
    SEC_23,
    SEC_24,
    SEC_25,
    SEC_26,
    SEC_27,
    SEC_28,
    SEC_29,
    SEC_30,
    SEC_31,
    SEC_32,
    SEC_33,
    SEC_34,
    SEC_35,
    SEC_36,
    SEC_37,
    SEC_38,
    SEC_39,
    SEC_40,
    SEC_41,
    SEC_42,
    SEC_43,
    SEC_44,
    SEC_45,
    SEC_46,
    SEC_47,
    SEC_48,
    SEC_49,
    SEC_50,
    SEC_51,
    SEC_52,
    SEC_53,
    SEC_54,
    SEC_55,
    SEC_56,
    SEC_57,
    SEC_58,
    SEC_59
} sel_second;

/*
 * Lo siguiente es una tecnica para lograr que una función retorne un puntero a
 * función, donde el tipo de función retornada es el de la funcion que se está
 * ejecutando.
 */
typedef void* (*ptr_ret_menu_t)(fm_event_t);            // Un puntero a funcion
typedef ptr_ret_menu_t (*ptr_fun_menu_t)(fm_event_t);

// Defines.

// Function prototypes

ptr_ret_menu_t fm_menu_config_date_hour(fm_event_t);
ptr_ret_menu_t fm_menu_config_expansion(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_1(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_2(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_3(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_4(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_5(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_param(fm_event_t);
ptr_ret_menu_t fm_menu_config_ko_param(fm_event_t);
ptr_ret_menu_t fm_menu_config_pass(fm_event_t);
ptr_ret_menu_t fm_menu_config_span(fm_event_t);
ptr_ret_menu_t fm_menu_config_units_tim(fm_event_t);
ptr_ret_menu_t fm_menu_config_units_vol(fm_event_t);

#endif /* MENU_CONFIG_H */

/*** end of file ***/
