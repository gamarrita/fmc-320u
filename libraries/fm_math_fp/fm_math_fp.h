/* @file fm_math_fp.h
 *
 * @brief Modulo destinado a realizar operaciones con variables de punto fijo
 * definidas como typedef en este mismo archivo.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef FM_MATH_FP_H_
#define FM_MATH_FP_H_

// includes

#include "main.h"
#include "stdio.h"
#include "string.h"
#include "../fm_debug/fm_debug.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

/*
 * @brief tipo de dato de punto fijo.
 */
typedef struct
{
    uint32_t num;
    uint8_t res;
} fp_t;

// Defines.

// Function prototypes

int fp_add_dot(fp_t fp, char *p_str, int str_size);
int fp_to_str(fp_t fp, char leading_char, int str_width, char *p_str,
int str_size);

#endif /* FM_MATH_FP_H */

/*** end of file ***/
