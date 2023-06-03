/* @file fm_version.h
 *
 * @brief
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_VERSION_H_
#define     FM_VERSION_H_

// includes

#include "stdio.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../fm_debug/fm_debug.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

typedef struct
{
    uint8_t version;
    uint8_t revision;
    uint8_t release;
} version_t;

// Defines.

// Function prototypes

void fm_version_get(char *ptr_str, int str_lgth);
void fm_version_set();

#endif /* FM_VERSION_H */

/*** end of file ***/
