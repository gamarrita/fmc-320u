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

// Const data.

#define K_ARR_LGTH 8

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.
totalizer_t acm;
totalizer_t rate;
totalizer_t ttl;

uint8_t k_array[K_ARR_LGTH]; //Arreglo que almacena al factor K.
uint8_t k_lin_array[K_ARR_LGTH]; //Arreglo que almacena los K_lin.
uint8_t frec_array[K_ARR_LGTH - 1];

// External variables.

extern const uint32_t g_scalar[];

// Global variables, statics.

static totalizer_t acm_config =
{
    .pulse = 1000,
    .volume.num = 0,
    .volume.res = 1,
    .factor.num = 1000, // pulsos/unidad_volumen.
    .factor.res = 2,
    .unit_volume = "LT",
    .unit_time = "S",
};

static totalizer_t rate_config =
{
    .pulse = 0,
    .volume.num = 0,
    .volume.res = 1,
    .factor.num = 1000,
    .factor.res = 2,
    .unit_volume = "LT",
    .unit_time = "S",
};

static totalizer_t ttl_config =
{
    .pulse = 12000,
    .volume.num = 0,
    .volume.res = 1,
    .factor.num = 1000, // pulsos/unidad_volumen.
    .factor.res = 2,
    .unit_volume = "LT",
    .unit_time = "S",
};

static k_factors_lin_t k_factor_lin_config =
{
    .k_struct_lin_1.num = 10000,
    .k_struct_lin_1.res = 2,
    .f_struct_lin_1 = 100,

    .k_struct_lin_2.num = 20000,
    .k_struct_lin_2.res = 2,
    .f_struct_lin_2 = 200,

    .k_struct_lin_3.num = 50000,
    .k_struct_lin_3.res = 2,
    .f_struct_lin_3 = 500,

    .k_struct_lin_4.num = 100000,
    .k_struct_lin_4.res = 2,
    .f_struct_lin_4 = 1000,

    .k_struct_lin_5.num = 150000,
    .k_struct_lin_5.res = 2,
    .f_struct_lin_5 = 1500,
};

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

uint16_t fm_factory_get_frec_lin(lin_factor_select_t frec_lin_num)
{
    if (frec_lin_num == LIN_FACTOR_1)
    {
        return (k_factor_lin_config.f_struct_lin_1);
    }
    else if (frec_lin_num == LIN_FACTOR_2)
    {
        return (k_factor_lin_config.f_struct_lin_2);
    }
    else if (frec_lin_num == LIN_FACTOR_3)
    {
        return (k_factor_lin_config.f_struct_lin_3);
    }
    else if (frec_lin_num == LIN_FACTOR_4)
    {
        return (k_factor_lin_config.f_struct_lin_4);
    }
    else if (frec_lin_num == LIN_FACTOR_5)
    {
        return (k_factor_lin_config.f_struct_lin_5);
    }

    return (0);
}

fp_t fm_factory_get_k_factor_lin(lin_factor_select_t k_lin_num)
{
    if (k_lin_num == LIN_FACTOR_1)
    {
        return (k_factor_lin_config.k_struct_lin_1);
    }
    else if (k_lin_num == LIN_FACTOR_2)
    {
        return (k_factor_lin_config.k_struct_lin_2);
    }
    else if (k_lin_num == LIN_FACTOR_3)
    {
        return (k_factor_lin_config.k_struct_lin_3);
    }
    else if (k_lin_num == LIN_FACTOR_4)
    {
        return (k_factor_lin_config.k_struct_lin_4);
    }
    else
    {
        return (k_factor_lin_config.k_struct_lin_5);
    }

}

void fm_factory_get_units_time(char *p_str_time_units, int length)
{
    if (length == 0)
    {
        return;
    }

    strncpy(p_str_time_units, rate_config.unit_time, 1);
}

void fm_factory_get_units_volume(char *p_str_volume_units, int length)
{
    if (length == 0)
    {
        return;
    }

    strncpy(p_str_volume_units, acm_config.unit_volume, 2);
}

uint8_t fm_factory_get_res_acm_ttl()
{
    return (acm_config.volume.res);
}

uint8_t fm_factory_get_res_rate()
{
    return (rate_config.volume.res);
}

void fm_factory_modify_k_lin_add(uint8_t digit_k_lin, uint8_t k_sel)
{
    uint32_t k_lin_new_num = 0;
    uint32_t frec_lin_new_num = 0;

    fm_factory_separate_k_lin_and_frec(k_sel);

    if (digit_k_lin <= LIN_DIG_7)
    {
        if (k_lin_array[K_ARR_LGTH - 1 - digit_k_lin] < g_scalar[1] - 1)
        {
            k_lin_array[K_ARR_LGTH - 1 - digit_k_lin]++;
        }
        else
        {
            k_lin_array[K_ARR_LGTH - 1 - digit_k_lin] = 0;
        }

        for (int i = 0; i <= K_ARR_LGTH - 1; i++)
        {
            k_lin_new_num = (k_lin_new_num * g_scalar[1]) + k_lin_array[i];
        }

        if (k_sel == LIN_FACTOR_1)
        {
            k_factor_lin_config.k_struct_lin_1.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_2)
        {
            k_factor_lin_config.k_struct_lin_2.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_3)
        {
            k_factor_lin_config.k_struct_lin_3.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_4)
        {
            k_factor_lin_config.k_struct_lin_4.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_5)
        {
            k_factor_lin_config.k_struct_lin_5.num = k_lin_new_num;
        }
    }
    else if (digit_k_lin > LIN_DIG_7)
    {
        if (K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8 != LIN_DIG_3)
        {
            if (frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8]
            < g_scalar[1] - 1)
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8]++;
            }
            else
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8] = 0;
            }
        }
        else
        {
            if (frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8]
            < g_scalar[0])
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8]++;
            }
            else
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8] = 0;
            }
        }

        for (int j = 0; j <= K_ARR_LGTH - 2; j++)
        {
            frec_lin_new_num = (frec_lin_new_num * g_scalar[1]) + frec_array[j];
        }

        if (k_sel == LIN_FACTOR_1)
        {
            k_factor_lin_config.f_struct_lin_1 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_2)
        {
            k_factor_lin_config.f_struct_lin_2 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_3)
        {
            k_factor_lin_config.f_struct_lin_3 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_4)
        {
            k_factor_lin_config.f_struct_lin_4 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_5)
        {
            k_factor_lin_config.f_struct_lin_5 = frec_lin_new_num;
        }
    }

}

/*
 * @brief Función que resta uno al dígito pasado como primer parámetro del
 * factor K linealizado seleccionado como segundo parámetro.
 * @param Digito a modificar del factor K de la enumeración sel_digit_k_lin_t y
 * factor k linealizado seleccionado.
 * @retval None
 */
void fm_factory_modify_k_lin_subs(uint8_t digit_k_lin, uint8_t k_sel)
{
    uint32_t k_lin_new_num = 0;
    uint32_t frec_lin_new_num = 0;

    fm_factory_separate_k_lin_and_frec(k_sel);
    if (digit_k_lin <= LIN_DIG_7)
    {
        if (k_lin_array[K_ARR_LGTH - 1 - digit_k_lin] > 0)
        {
            k_lin_array[K_ARR_LGTH - 1 - digit_k_lin]--;
        }
        else
        {
            k_lin_array[K_ARR_LGTH - 1 - digit_k_lin] = g_scalar[1] - 1;
        }

        for (int i = 0; i <= K_ARR_LGTH - 1; i++)
        {
            k_lin_new_num = (k_lin_new_num * g_scalar[1]) + k_lin_array[i];
        }

        if (k_sel == LIN_FACTOR_1)
        {
            k_factor_lin_config.k_struct_lin_1.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_2)
        {
            k_factor_lin_config.k_struct_lin_2.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_3)
        {
            k_factor_lin_config.k_struct_lin_3.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_4)
        {
            k_factor_lin_config.k_struct_lin_4.num = k_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_5)
        {
            k_factor_lin_config.k_struct_lin_5.num = k_lin_new_num;
        }
    }
    else if (digit_k_lin > LIN_DIG_7)
    {
        if (K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8 != LIN_DIG_3)
        {
            if (frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8] > 0)
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8]--;
            }
            else
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8] =
                g_scalar[1] - 1;
            }
        }
        else
        {
            if (frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8] > 0)
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8]--;
            }
            else
            {
                frec_array[K_ARR_LGTH - 2 - digit_k_lin + LIN_DIG_8] =
                g_scalar[0];
            }
        }

        for (int j = 0; j <= K_ARR_LGTH - 2; j++)
        {
            frec_lin_new_num = (frec_lin_new_num * g_scalar[1]) + frec_array[j];
        }

        if (k_sel == LIN_FACTOR_1)
        {
            k_factor_lin_config.f_struct_lin_1 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_2)
        {
            k_factor_lin_config.f_struct_lin_2 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_3)
        {
            k_factor_lin_config.f_struct_lin_3 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_4)
        {
            k_factor_lin_config.f_struct_lin_4 = frec_lin_new_num;
        }
        else if (k_sel == LIN_FACTOR_5)
        {
            k_factor_lin_config.f_struct_lin_5 = frec_lin_new_num;
        }
    }
}

/*
 * @brief Función que suma uno al dígito pasado como parámetro del factor K.
 * @param Digito a modificar del factor K.
 * @retval None
 */
void fm_factory_modify_k_factor_add(uint8_t digit_k)
{
    uint32_t k_new_num = 0;
    fm_factory_separate_k_factor();

    if (k_array[K_ARR_LGTH - 1 - digit_k] < g_scalar[1] - 1)
    {
        k_array[K_ARR_LGTH - 1 - digit_k]++;
    }
    else
    {
        k_array[K_ARR_LGTH - 1 - digit_k] = 0;
    }

    for (int i = 0; i <= K_ARR_LGTH - 1; i++)
    {
        k_new_num = (k_new_num * g_scalar[1]) + k_array[i];
    }

    acm_config.factor.num = k_new_num;
    rate_config.factor.num = k_new_num;
    ttl_config.factor.num = k_new_num;
}

void fm_factory_modify_k_factor_subs(uint8_t digit_k)
{
    uint32_t k_new_num = 0;
    fm_factory_separate_k_factor();

    if (k_array[K_ARR_LGTH - 1 - digit_k] > g_scalar[0] - 1)
    {
        k_array[K_ARR_LGTH - 1 - digit_k]--;
    }
    else
    {
        k_array[K_ARR_LGTH - 1 - digit_k] = g_scalar[1] - 1;
    }

    for (int i = 0; i <= K_ARR_LGTH - 1; i++)
    {
        k_new_num = (k_new_num * g_scalar[1]) + k_array[i];
    }

    acm_config.factor.num = k_new_num;
    rate_config.factor.num = k_new_num;
    ttl_config.factor.num = k_new_num;
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
    strncpy(acm_config.unit_time, p_str_time_units, 1);
    strncpy(rate_config.unit_time, p_str_time_units, 1);
    strncpy(ttl_config.unit_time, p_str_time_units, 1);
}

void fm_factory_modify_units_volume(char *p_str_volume_units)
{
    strncpy(acm_config.unit_volume, p_str_volume_units, 2);
    strncpy(rate_config.unit_volume, p_str_volume_units, 2);
    strncpy(ttl_config.unit_volume, p_str_volume_units, 2);
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

/*
 * @brief Función que separa en dígitos el factor K y los guarda en un arreglo
 * global.
 * @param None
 * @retval None
 */
void fm_factory_separate_k_factor()
{
    static uint32_t k_num;
    static int i = 7;

    k_num = fm_factory_get_acm().factor.num;

    while (i >= 0)
    {
        if (k_num > 0)
        {
            k_array[i] = k_num % g_scalar[1];
            k_num /= g_scalar[1];
        }
        else
        {
            k_array[i] = 0;
        }
        i--;
    }
}

void fm_factory_separate_k_lin_and_frec(uint8_t k_sel)
{
    uint32_t k_lin_num = 0;
    uint16_t frec_num = 0;

    int i = 7; //Contador que funciona solo en el while de esta función.
    int j = 6; //Contador que funciona solo en el while de esta función.

    k_lin_num = fm_factory_get_k_factor_lin(k_sel).num;
    frec_num = fm_factory_get_frec_lin(k_sel);

    while (i >= 0) //Introduzco el factor k linealizado en un arreglo.
    {
        if (k_lin_num > 0)
        {
            k_lin_array[i] = k_lin_num % g_scalar[1];
            k_lin_num /= g_scalar[1];
        }
        else
        {
            k_lin_array[i] = 0;
        }
        i--;
    }

    while (j >= 0) //Introduzco la frecuencia de linealización en otro arreglo.
    {
        if (frec_num > 0)
        {
            frec_array[j] = frec_num % g_scalar[1];
            frec_num /= g_scalar[1];
        }
        else
        {
            frec_array[j] = 0;
        }
        j--;
    }
}

// Interrupts

/*** end of file ***/

