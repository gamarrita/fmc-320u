/* @file fm_module.h
 *
 * @brief
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef FRECUENCY_METER_H_
#define FRECUENCY_METER_H_

// includes

#include "main.h"
#include "stdio.h"
#include "string.h"
#include "../fm_debug/fm_debug.h"
#include "../fm_factory/fm_factory.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

typedef enum
{
 FREC_RES_0, FREC_RES_1, FREC_RES_2, FREC_RES_3, FREC_RES_MAX
}frecuency_meter_res_t;

// Defines.

// Function prototypes

uint8_t frecuency_meter_get_resolution();
uint64_t frecuency_meter_get_frec_u64();

#endif /* FRECUENCY_METER */

/*** end of file ***/
