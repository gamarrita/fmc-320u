/* @file fm_version.c
 *
 * @brief En este módulo se irán actualizando los dígitos correspondientes a la
 * versión del firmware en formato V.R.R (Version.Revision.Release). De aquí
 * también se extraerá la versión para enviarla a la pantalla inicial de
 * fm_lcd_version.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_version.h"

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
const int version_lgth = 20;

const int version = 1;
const int revision = 3;
const int release = 0;

version_t ve;

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Esta función devuelve la versión actual del firmware.
 * @param None
 * @retval Número de versión en formato VRR.
 */
void fm_version_get(char *ptr_str, int str_lgth)
{
    if (str_lgth <= 0)
    {
        return;
    }

    fm_version_set();

    sprintf(ptr_str, "%u.%u.%u", ve.version, ve.revision, ve.release);
}

/*
 * @brief Esta función setea los 3 números de la versión según las variables
 * constantes definidas en este mismo archivo.
 * @param None
 * @retval None
 */
void fm_version_set()
{
    ve.version = version;
    ve.revision = revision;
    ve.release = release;
}

// Interrupts

/*** end of file ***/
