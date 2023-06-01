/* @file fmc.c
 *
 * @brief Este módulo es un computador de caudales que toma los datos de entrada
 * de los sensores y los procesa para obtener valores medibles como litros.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.

#include "fmc.h"

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

extern totalizer_t acm;
extern totalizer_t rate;
extern totalizer_t ttl;

// External variables.

// Global variables, statics.

static const uint32_t g_scalar_fmc[] = {1, 10, 100, 1000, 10000, 100000,
                                        1000000};

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que obtiene el valor del caudal acumulado y lo devuelve como
 * parámetro de retorno.
 * @param  None
 * @retval Volumen acumulado como estructura.
 */
totalizer_t fmc_get_acm()
{
    acm = fm_factory_get_acm();

    acm = fmc_totalizer_init(acm);

    return (acm);
}

/*
 * @brief Función que obtiene el valor del caudal instantaneo 'rate' y lo
 * devuelve como parámetro de retorno.
 * @param  None
 * @retval Caudal instantaneo como estructura.
 */
totalizer_t fmc_get_rate()
{
    rate = fm_factory_get_rate();

    rate = fmc_totalizer_init(rate);

    return (rate);
}

/*
 * @brief Función que obtiene el valor del caudal histórico y lo devuelve como
 * parámetro de retorno.
 * @param  None
 * @retval Volumen histórico como estructura.
 */
totalizer_t fmc_get_ttl()
{
    ttl = fm_factory_get_ttl();

    ttl = fmc_totalizer_init(ttl);

    return (ttl);
}


/*
 * @brief Esta función limpia la cantidad de pulsos leidos por el contador de
 * pulsos.
 * @param  Puntero a estructura que contiene datos como la cantidad de pulsos
 * leidos, valor y resolución del caudal/volumen, entre otros.
 * @retval None
 */
void fmc_totalizer_clear_pulse(totalizer_t *p_totalizer)
{
    p_totalizer->pulse = 0;
    fmc_totalizer_refresh(p_totalizer);
}

/*
 * @brief
 * @param
 * @retval None
 */
totalizer_t fmc_totalizer_init(totalizer_t totalizer)
{
    fmc_totalizer_refresh(&totalizer);

    return (totalizer);
}

/*
 * @brief Esta función refresca el volumen obtenido de dividir los pulsos leidos
 * por el factor correspondiente.
 * @param Puntero a estructura que contiene datos como la cantidad de pulsos
 * leidos, valor y resolución del caudal/volumen, entre otros.
 * @retval None
 */
void fmc_totalizer_refresh(totalizer_t *p_totalizer)
{
    uint64_t result;

    /*
     * result es la cantidad de pulsos almacenados en la estructura p_totalizer.
     */
    result = (uint64_t) p_totalizer->pulse;

    /*
     * Pulsos escalados en el factor y en la resolucion a mostrar.
     */
    result *= g_scalar_fmc[p_totalizer->factor.res + p_totalizer->volume.res];

    /*
     * Obtengo el valor numérico del volumen dividiendo los pulsos escalados
     * por el factor.
     */
    result /= p_totalizer->factor.num;

    p_totalizer->volume.num = (uint32_t) result;

}

// Interrupts

/*** end of file ***/
