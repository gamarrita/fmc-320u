/* @file fm_module.c
 *
 * @brief Each source file shall be comprised of some or all of the following
 * sections, in the order listed down below.
 * Comenzar cada nuevo archivo usando un template. Hay un template general a
 * todos los proyectos que puede ser usado hasta el momento de introducir un
 * cambio que solo responderá a un proyecto en progreso, es ese caso el
 * template pasará a ser parte del proyecto.
 *
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.

#include "fm_math_fp.h"

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

//Debug.

// Project variables, non-static, at least used in other file.

// External variables.

// Global variables, statics.

static const int fp_str_max = 20;

static const uint32_t g_scalar[] = {1, 10, 100, 1000, 10000, 100000, 1000000};

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Agrega el punto decimal a una string que representa un entero.
 * Esta funcion es típicamente usada luego de  fm_lcd_u32_to_str();
 * @param fp numero con punto decimal.
 * @param p_str puntero al string destino.
 * @param str_size tamaño del string.
 * @retval int.
 *
 */
int fp_add_dot(fp_t fp, char *p_str, int str_size)
{
    int idx_end;
    int idx_now;

    /*
     * Si la resolución asignada por el usuario es 0, no hay nada que hacer.
     */
    if (fp.res == 0)
    {
        return (0);
    }

    idx_now = strlen(p_str); //Como indice apunta a terminador nulo.

    /*
     * Chequeo de contorno para ver si entra el punto o no.
     */
    if ((idx_now + 1 + 1) >= str_size)
    {
        return (-1);
    }

    idx_end = idx_now - fp.res; //Como índice apunta a donde debe ir el '.'

    /*
     * Desplazo posiciones en el arreglo hasta llegar a la posición donde debe
     * ir ubicado el '.'
     */
    while (idx_now != idx_end)
    {
        p_str[idx_now + 1] = p_str[idx_now];
        idx_now--;
    }
    p_str[idx_now + 1] = p_str[idx_now];
    p_str[idx_now] = '.';

    return (0);
}

/*
 * @brief Convierte un numero con punto decimal a una string, pero sin el punto.
 * @param fp numero con punto decimal.
 * @param leading_char caracter de relleno, cero para no rellenar.
 * @param al rellenar debemos indicar a que ancho con esta variable.
 * @param p_str string destino, por referencia.
 * @param str_size tamaño del string.
 * @retval int.
 */
int fp_to_str(fp_t fp, char leading_char, int str_width, char *p_str,
int str_size)
{
    int idx_1 = 0;

    /*
     * Este es el unico chequeo de contorno. Verifica que el tamaño del string
     * no sea menor a PCF8553_DATA_SIZE, calculado como suficiente para operar
     * con las líneas del lcd.
     */
    if (str_size < fp_str_max)
    {
        return (-1);
    }

    /*
     * Esta linea es necesaria para representa bien a num = 0
     */
    p_str[idx_1] = fp.num % g_scalar[1] + '0';

    uint32_t num_aux = fp.num;

    /*
     * Almaceno el número de atrás para adelante hasta que llegar al primer
     * dígito inclusive
     */
    while (fp.num / g_scalar[1])
    {
        idx_1++;
        fp.num /= g_scalar[1];
        p_str[idx_1] = fp.num % g_scalar[1] + '0';
    }

    /*
     * Si se midió 0 pulsos, se rellena con 0 hasta pasar el punto y asi se
     * mantiene la resolución deseada.
     */
    if (num_aux == 0)
    {
        while (idx_1 < fp.res)
        {
            idx_1++;
            p_str[idx_1] = fp.num % g_scalar[1] + '0';
        }
    }

    /*
     * Si el caracter a completar no es 0, se lo debe agregar al final del
     * arreglo hasta completar el largo de la linea (7 u 8 caracteres + \0).
     */
    if (leading_char)
    {
        while (idx_1 < str_width - 1)
        {
            idx_1++;
            p_str[idx_1] = leading_char;
        }
    }
    p_str[idx_1] = '\0';
    idx_1--;

    /*
     *Esta sección da vuelta el arreglo ya que se completó al revés.
     */
    int idx_2 = 0;
    char ch_temp;

    while (idx_1 > idx_2)
    {
        ch_temp = p_str[idx_1];
        p_str[idx_1] = p_str[idx_2];
        p_str[idx_2] = ch_temp;
        idx_1--;
        idx_2++;
    }

    return (0);
}


// Interrupts

/*** end of file ***/
