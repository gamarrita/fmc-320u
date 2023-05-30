/* @file frecuency_meter.c
 *
 * @brief
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.

#include "fm_frec_meter.h"

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

double frecuency = 0;
uint8_t no_pulse_cnt = 0;

// External variables.

extern UART_HandleTypeDef huart1;

// Global variables, statics.

static const uint32_t g_scalar_frec_meter[] = {0, 10, 100, 1000};

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que obtiene la resolución real de la frecuencia medida por el
 * computador de caudales (que debe tener 4 dígitos obligatoriamente).
 * @param None
 * @retval Resolución de la frecuencia como entero sin signo de 8 bits.
 */
uint8_t frecuency_meter_get_resolution()
{
    if(frecuency >= g_scalar_frec_meter[FREC_RES_0] &&
    frecuency < g_scalar_frec_meter[FREC_RES_1])
    {
        return(FREC_RES_3);
    }
    else if(frecuency >= g_scalar_frec_meter[FREC_RES_1] &&
    frecuency < g_scalar_frec_meter[FREC_RES_2])
    {
        return(FREC_RES_2);
    }
    else if(frecuency >= g_scalar_frec_meter[FREC_RES_2] &&
    frecuency < g_scalar_frec_meter[FREC_RES_3])
    {
        return(FREC_RES_1);
    }
    else
    {
        return(FREC_RES_0);
    }
}

/*
 * @brief Función que obtiene la frecuencia como un número entero, moviendo los
 * dígitos decimales reales hacia la derecha y cortando los que no son
 * verdaderos.
 */
uint64_t frecuency_meter_get_frec_u64()
{
    static uint64_t frec_u64 = 0;

    frec_u64 = frecuency * g_scalar_frec_meter[frecuency_meter_get_resolution()];

    return (frec_u64);
}

// Interrupts

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) // @suppress("Name convention for function")
{
    if(GPIO_Pin == GPIO_PIN_14)
    {
        static uint8_t new_entry = 1;
        static uint16_t ticks_1 = 0;
        static uint16_t ticks_2 = 0;
        static uint16_t delta_ticks = 0;
        static uint16_t pulses_1 = 0;
        static uint16_t pulses_2 = 0;
        static uint16_t delta_pulses = 0;
        static GPIO_InitTypeDef GPIO_InitStruct = {0};

        static const double lse_frecuency = 32768;

        no_pulse_cnt = 1;

        if(new_entry)
        {
            ticks_1 = LPTIM3 -> CNT;
            pulses_1 = LPTIM4 -> CNT;

            new_entry = 0;
        }
        else
        {
            ticks_2 = LPTIM3 -> CNT;
            pulses_2 = LPTIM4 -> CNT;

            delta_ticks = ticks_2 - ticks_1;
            delta_pulses = pulses_2 - pulses_1;

            frecuency = (double)(delta_pulses * lse_frecuency) / delta_ticks;

            #ifdef FM_DEBUG_UART_TX_PULSE_DIFF

                  static char d_pulses[50];

                  sprintf(d_pulses, "delta_p: %u \n", delta_pulses);
                  HAL_UART_Transmit(&huart1, (uint8_t*)d_pulses,
                  strlen(d_pulses), HAL_MAX_DELAY);
            #endif

            #ifdef FM_DEBUG_UART_TX_TICKS_DIFF

                  static char d_ticks[50];

                  sprintf(d_ticks, "delta_t: %u \n", delta_ticks);
                  HAL_UART_Transmit(&huart1, (uint8_t*)d_ticks,
                  strlen(d_ticks), HAL_MAX_DELAY);
            #endif

            ticks_1 = ticks_2;
            pulses_1 = pulses_2;
        }

        fm_factory_modify_pulse_rate(frecuency_meter_get_frec_u64());

        GPIO_InitStruct.Pin = PULSE_IT_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(PULSE_IT_GPIO_Port, &GPIO_InitStruct);

        HAL_NVIC_DisableIRQ(EXTI14_IRQn);
    }
}

/*** end of file ***/
