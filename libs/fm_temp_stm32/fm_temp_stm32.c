/* @file fm_temperature_sensor.c
 *
 * Este módulo se encarga de obtener la temperatura interna del micro, con el
 * objetivo de mostrarla en la pantalla lcd, en el menú "acm_temp". Para esto,
 * la formatea como un número entero.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_temp_stm32.h"

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
extern ADC_HandleTypeDef hadc1;
// Defines.

// Debug.

/*
 * To temporally disable a block of code, use preprocessor's conditional
 * compilation features, eg, the following one should be used to increase the
 * the debug output information.
 *
 */
#ifndef NDEBUG
#endif

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que toma la lectura del ADC1, el cual está asociado a la
 * medición de temperatura del microcontrolador, y la devuelve convertida a
 * grados celcius.
 * @param None
 * @retval Temperatura interna del micro en grados celcius.
 */
int fm_temp_stm32_format()
{
    uint16_t raw_value;
    int temp_celcius;

    fm_temp_stm32_get();
    raw_value = HAL_ADC_GetValue(&hadc1);
    temp_celcius = __HAL_ADC_CALC_TEMPERATURE(&hadc1, 3285, raw_value,
    ADC_RESOLUTION_12B);

    #ifdef FM_DEBUG_UART_TX_TEMP_SENSOR
      fm_debug_uint8_uart((uint8_t)temp_celcius);
    #endif

    return (temp_celcius);
}

/*
 * @brief Función que lee el ADC 1 asociado al sensor de temperatura.
 * @param None
 * @retval None
 */
void fm_temp_stm32_get()
{
    const uint8_t poll_time = 100;

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, poll_time);
    HAL_ADC_Stop(&hadc1);
}

// Interrupts

/*** end of file ***/

