/* @file fm_keypad.c
 *
 * @brief Módulo encargado de manejar las interrupciones provenientes del
 * teclado. Por ahora solo puede controlar los botones de arriba y abajo.
 * 
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_keypad.h"

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
/*
 * Mapeo que tecla corresponde a que pin
 */
#define KEY_UP 	    GPIO_PIN_10
#define KEY_DOWN	GPIO_PIN_11
#define KEY_ENTER	GPIO_PIN_12
#define KEY_ESC     GPIO_PIN_13

//Debug.

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
extern TX_QUEUE event_queue_ptr;
extern TX_SEMAPHORE debounce_semaphore_ptr;

// Global variables, statics.

// Private function prototypes.

// Public function bodies.

// Private function bodies.

// Interrupts

void HAL_GPIO_EXTI_Rising_Callback(uint16_t gpio_pin) // @suppress("Name convention for function")
{
    if (gpio_pin != GPIO_PIN_14)
    {
        /*
         * Deshabilito interrupciones para evitar rebotes de los botones, hasta
         * que la tarea task_debounce termine su ejecución y se bloquee.
         */
        HAL_NVIC_DisableIRQ(EXTI10_IRQn);
        HAL_NVIC_DisableIRQ(EXTI11_IRQn);
        HAL_NVIC_DisableIRQ(EXTI12_IRQn);
        HAL_NVIC_DisableIRQ(EXTI13_IRQn);

        /*
         * Variable que almacena el nombre del botón que se presionó.
         */
        fm_event_t event_now;

        /*
         * Compruebo qué botón fue el presionado.
         */
        switch (gpio_pin)
        {
            case KEY_UP:
                event_now = EVENT_KEY_UP;
            break;
            case KEY_DOWN:
                event_now = EVENT_KEY_DOWN;
            break;
            case KEY_ENTER:
                event_now = EVENT_KEY_ENTER;
            break;
            case KEY_ESC:
                event_now = EVENT_KEY_ESC;
            break;
            default:
                return;
            break;
        }

        /*
         * Pongo el nombre del botón presionado en la cola de eventos y libero
         * el semáforo de la tarea task_debounce, para que esta se bloquee
         * durante el tiempo necesario para que no haya rebotes de los botones.
         */
        tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        tx_semaphore_ceiling_put(&debounce_semaphore_ptr, 1);
    }
}

/*** end of file ***/
