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
    char *unit_volume;
    char *unit_time;
} totalizer_t;

// Defines.

// Function prototypes

totalizer_t fm_factory_get_acm();
totalizer_t fm_factory_get_rate();
totalizer_t fm_factory_get_ttl();
void fm_factory_get_units_time(char *p_str_time_units, int length);
void fm_factory_get_units_volume(char *p_str_volume_units, int length);
void fm_factory_modify_pulse_acm_ttl(uint64_t added_pulses);
void fm_factory_modify_pulse_rate(uint64_t added_pulses);
void fm_factory_modify_res_rate(uint8_t resolution_rate);
void fm_factory_modify_units_time(char *p_str_time_units);
void fm_factory_modify_units_volume(char *p_str_volume_units);

#endif /* FM_FACTORY_H */

/*** end of file ***/
