/* @file fm_debug.h
 *
 * @brief Para incluir este header se debe hacer de la siguiente manera:
 * #include "..\..\..\..\libs\fm_debug\fm_debug.h"
 *
 *
 * @par
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

#ifndef     FM_DEBUG_H_
#define     FM_DEBUG_H_

// includes.
#include "main.h"
#include "string.h"
#include "stdio.h"

// Macros, defines, microcontroller pins (dhs).

/*
 * La siguiente macro controla los mensajes de debug a nivel global, comentar
 * la macro para deshabilitar todo mensaje de macro.
 */

#define FM_DEBUG

/*
 * Si FM_DEBUG esta definida controlo individualmente cada mensaje de debug
 * el siguiente condicional. Típicamente solo uno, o unos pocos, estarán
 * habilitados.
 */

#ifdef FM_DEBUG

#define FM_DEBUG_UART_TX_TIME_ON_IDLE
//#define FM_DEBUG_UART_TX_MENU_ENTER
//#define FM_DEBUG_UART_TX_CALENDAR
//#define FM_DEBUG_UART_TX_TEMP_SENSOR
//#define FM_DEBUG_UART_TX_PRESS_KEYPAD
//#define FM_DEBUG_UART_TX_PULSE_DIFF
#define FM_DEBUG_UART_TX_PULSE_FRECUENCY
//#define FM_DEBUG_UART_TX_TICKS_DIFF

#endif

// Typedef.

// Defines.

// Function prototypes

void fm_debug_msg_itm(const uint8_t *msg, uint8_t len);
void fm_debug_msg_uart(const uint8_t *p_msg, uint8_t len);
void fm_debug_uint8_uart(uint8_t num);
void fm_debug_uint16_uart(uint16_t num);
void fm_debug_uint32_uart(uint32_t num);
void fm_debug_float_uart(float num);

#endif /* FM_DEBUG_H */

/*** end of file ***/

