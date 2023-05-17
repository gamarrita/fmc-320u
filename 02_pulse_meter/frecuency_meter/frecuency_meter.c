/* @file frecuency_meter.c
 *
 * @brief
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.

#include "frecuency_meter.h"
#include "stdio.h"
#include "string.h"

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

float frecuency = 0;

// External variables.

extern LPTIM_HandleTypeDef hlptim3;
extern LPTIM_HandleTypeDef hlptim4;
extern UART_HandleTypeDef huart1;

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
    static uint8_t new_entry = 1;
    static uint16_t ticks_1 = 0;
    static uint16_t ticks_2 = 0;
    static uint16_t delta_ticks = 0;
    static uint16_t pulses_1 = 0;
    static uint16_t pulses_2 = 0;
    static uint16_t delta_pulses = 0;

    static const uint16_t lse_frecuency = 32768;

    static char frecuency_msg[30];

    if(new_entry)
    {
        ticks_1 = HAL_LPTIM_ReadCounter(&hlptim3);
        pulses_1 = HAL_LPTIM_ReadCounter(&hlptim4);

        new_entry = 0;
    }
    else
    {
        ticks_2 = HAL_LPTIM_ReadCounter(&hlptim3);
        pulses_2 = HAL_LPTIM_ReadCounter(&hlptim4);

        delta_ticks = ticks_2 - ticks_1;
        delta_pulses = pulses_2 - pulses_1;

        frecuency = (float)(delta_pulses * delta_ticks) / lse_frecuency;

        ticks_1 = ticks_2;
        pulses_1 = pulses_2;
    }

    sprintf(frecuency_msg, "Frecuencia: %0.2f Hz\n",frecuency);
    HAL_UART_Transmit(&huart1, (uint8_t*)frecuency_msg,
    strlen(frecuency_msg), HAL_MAX_DELAY);

    HAL_NVIC_DisableIRQ(EXTI14_IRQn);
}


// Interrupts

/*** end of file ***/
