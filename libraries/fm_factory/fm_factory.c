/* @file fm_factory.c
 *
 * @brief Modulo que almacena los valores del computador de caudales. Por ahora
 * el nombre "factory" solo involucra a cuando se ejecuta por primera vez al
 * código, debido a que despues estos valores se pueden cambiar, por
 * configuraciones realizadas por el usuario, o por toma de datos de sensores o
 * periféricos externos e internos.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_factory.h"

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
totalizer_t acm;
totalizer_t rate;
totalizer_t ttl;

static totalizer_t acm_config =
{
    .pulse = 1000,
    .volume.num = 1,
    .volume.res = 1,
    .factor.num = 1000, // pulsos/unidad_volumen.
    .factor.res = 0,
    .unit_volume = "LT",
    .unit_time = "S",
};

static totalizer_t rate_config =
{
    .pulse = 0,
    .volume.num = 0,
    .volume.res = 1,
    .factor.num = 1000,
    .factor.res = 0,
    .unit_volume = "LT",
    .unit_time = "S",
};

static totalizer_t ttl_config =
{
    .pulse = 12000,
    .volume.num = 1,
    .volume.res = 1,
    .factor.num = 1000, // pulsos/unidad_volumen.
    .factor.res = 0,
    .unit_volume = "LT",
    .unit_time = "S",
};

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que devuelve el parámetro acm desde la memoria.
 * @param None
 * @retval Parámetro acm.
 */
totalizer_t fm_factory_get_acm()
{
    return (acm_config);
}

/*
 * @brief Función que devuelve el parámetro rate desde la memoria.
 * @param None
 * @retval Parámetro rate.
 */
totalizer_t fm_factory_get_rate()
{
    return (rate_config);
}

/*
 * @brief Función que devuelve el parámetro ttl desde la memoria.
 * @param None
 * @retval Parámetro ttl.
 */
totalizer_t fm_factory_get_ttl()
{
    return (ttl_config);
}

void fm_factory_get_units_time(char *p_str_time_units, int length)
{
    if (length == 0)
    {
        return;
    }

    strcpy(p_str_time_units, rate_config.unit_time);
}

void fm_factory_get_units_volume(char *p_str_volume_units, int length)
{
    if (length == 0)
    {
        return;
    }

    strcpy(p_str_volume_units, acm_config.unit_volume);
}

uint8_t fm_factory_get_res_acm_ttl()
{
    return (acm_config.volume.res);
}

uint8_t fm_factory_get_res_rate()
{
    return (rate_config.volume.res);
}

/*
 * @brief Actualizo el valor de los pulsos de volumen.
 * @param Pulsos a añadir a los ya existentes.
 * @retval None
 */
void fm_factory_modify_pulse_acm_ttl(uint64_t added_pulses)
{
    acm_config.pulse += added_pulses;
    ttl_config.pulse += added_pulses;
}

/*
 * @brief Actualizo el valor del delta de pulsos para medir la frecuencia en un
 * ciclo de aproximadamente 1 segundo.
 * @param Delta de pulsos en aproximadamente 1 segundo.
 * @retval None
 */
void fm_factory_modify_pulse_rate(uint64_t added_pulses)
{
    rate_config.pulse = added_pulses;
}

void fm_factory_modify_res_acm_ttl(uint8_t resolution_acm_ttl)
{
    acm_config.volume.res = resolution_acm_ttl;
    ttl_config.volume.res = resolution_acm_ttl;
}

void fm_factory_modify_res_rate(uint8_t resolution_rate)
{
    rate_config.volume.res = resolution_rate;
}

void fm_factory_modify_units_time(char *p_str_time_units)
{
    strcpy(acm_config.unit_time, p_str_time_units);
    strcpy(rate_config.unit_time, p_str_time_units);
    strcpy(ttl_config.unit_time, p_str_time_units);
}

void fm_factory_modify_units_volume(char *p_str_volume_units)
{
    strcpy(acm_config.unit_volume, p_str_volume_units);
    strcpy(rate_config.unit_volume, p_str_volume_units);
    strcpy(ttl_config.unit_volume, p_str_volume_units);
}

/*
 * @brief Función que resetea el valor del acm.
 * @param None
 * @retval None
 */
void fm_factory_reset_acm()
{
    acm_config.pulse = 0;
    acm_config.volume.num = 0;
}

// Interrupts

/*** end of file ***/

