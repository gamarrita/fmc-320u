/* @file fmc.h
 *
 * @brief Este módulo es un computador de caudales que toma los datos de entrada
 * de los sensores y los procesa para obtener valores medibles como litros.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef FMC_H_
#define FMC_H_

// includes

#include "main.h"
#include "../fm_factory/fm_factory.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes

totalizer_t fmc_get_acm();
totalizer_t fmc_get_rate();
totalizer_t fmc_get_ttl();
void fmc_totalizer_clear_pulse(totalizer_t *p_totalizer);
totalizer_t fmc_totalizer_init(totalizer_t totalizer);
void fmc_totalizer_refresh(totalizer_t *p_totalizer);

#endif /* FMC_H */

/*** end of file ***/
