/* @file fm_factory.h
 *
 * @brief Modulo que almacena los valores del computador de caudales. Por ahora
 * el nombre "factory" solo involucra a cuando se ejecuta por primera vez al
 * código, debido a que despues estos valores se pueden cambiar, por
 * configuraciones realizadas por el usuario, o por toma de datos de sensores o
 * periféricos externos e internos.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef FM_FACTORY_H_
#define FM_FACTORY_H_

// includes

#include "main.h"
#include "../fm_math_fp/fm_math_fp.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

typedef struct
{
    uint64_t pulse;
    fp_t volume;
    fp_t factor; // Factor en pulsos / unidad de volumen
    char unit_volume[2];
    char unit_time[1];
} totalizer_t;

typedef struct
{
    fp_t k_struct_lin_1;
    uint16_t f_struct_lin_1;
    fp_t k_struct_lin_2;
    uint16_t f_struct_lin_2;
    fp_t k_struct_lin_3;
    uint16_t f_struct_lin_3;
    fp_t k_struct_lin_4;
    uint16_t f_struct_lin_4;
    fp_t k_struct_lin_5;
    uint16_t f_struct_lin_5;
} k_factors_lin_t;

typedef enum
{
    LIN_FACTOR_1 = 1, LIN_FACTOR_2, LIN_FACTOR_3, LIN_FACTOR_4, LIN_FACTOR_5
} lin_factor_select_t;

typedef enum
{
    LIN_DIG_0,
    LIN_DIG_1,
    LIN_DIG_2,
    LIN_DIG_3,
    LIN_DIG_4,
    LIN_DIG_5,
    LIN_DIG_6,
    LIN_DIG_7,
    LIN_DIG_8,
    LIN_DIG_9,
    LIN_DIG_10,
    LIN_DIG_11
} sel_lin_digit_t;

// Defines.

// Function prototypes

totalizer_t fm_factory_get_acm();
totalizer_t fm_factory_get_rate();
totalizer_t fm_factory_get_ttl();
uint16_t fm_factory_get_frec_lin(lin_factor_select_t frec_lin_num);
fp_t fm_factory_get_k_factor_lin(lin_factor_select_t k_lin_num);
void fm_factory_get_units_time(char *p_str_time_units, int length);
void fm_factory_get_units_volume(char *p_str_volume_units, int length);
uint8_t fm_factory_get_res_acm_ttl();
uint8_t fm_factory_get_res_rate();
void fm_factory_modify_k_lin_add(uint8_t digit_k_lin, uint8_t k_sel);
void fm_factory_modify_k_lin_subs(uint8_t digit_k_lin, uint8_t k_sel);
void fm_factory_modify_k_factor_add(uint8_t digit_k);
void fm_factory_modify_k_factor_subs(uint8_t digit_k);
void fm_factory_modify_pulse_acm_ttl(uint64_t added_pulses);
void fm_factory_modify_pulse_rate(uint64_t added_pulses);
void fm_factory_modify_res_acm_ttl(uint8_t resolution_rate);
void fm_factory_modify_res_rate(uint8_t resolution_acm_ttl);
void fm_factory_modify_units_time(char *p_str_time_units);
void fm_factory_modify_units_volume(char *p_str_volume_units);
void fm_factory_reset_acm();
void fm_factory_separate_k_factor();
void fm_factory_separate_k_lin_and_frec(uint8_t k_sel);

#endif /* FM_FACTORY_H */

/*** end of file ***/
