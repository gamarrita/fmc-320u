/* @file fm_lcd.c
 *
 * @brief Este es el modulo que posee la inteligencia para manejar las opciones
 * que brinda la pantalla LCD, utilizando las funciones implementadas de
 * impresión de caracteres individuales en lcd.h.
 * Las funciones de este modulo serán usadas por librerías de mas alto nivel que
 * impriman menús o controlen máquinas de estado de pantallas, tales como
 * fm_menu_user.h.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_lcd.h"

// Typedef.

// Const data.

// Defines.
#define MSG_LENGTH    12
// Debug.

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

// Global variables, statics.

const int ten_multiplier = 10;

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que limpia toda la pantalla.
 * @param  None
 * @retval None
 */
void fm_lcd_clear()
{
    lcd_clear_all();
}

/*
 * @brief Función que limpia de la pantalla todos los símbolos.
 * @param None
 * @retval None
 */
void fm_lcd_clear_symbol_all()
{
    lcd_clear_symbol(ACM);
    lcd_clear_symbol(TTL);
    lcd_clear_symbol(RATE);
    lcd_clear_symbol(BATTERY);
    lcd_clear_symbol(PULSE_POINT);
    lcd_clear_symbol(POWER);
    lcd_clear_symbol(E);
    lcd_clear_symbol(BATCH);
    lcd_clear_symbol(BACKSLASH);
}

/*
 * @brief Función que formatea información pasada como parámetro para ser
 * colocada en una de las dos filas de la pantalla LCD.
 * @param Enumeracion rows_t de lcd.h.
 * @param Datos a formatear.
 * @param String que almacena los datos formateados.
 * @param Longitud de este String.
 * @retval arreglo con la información formateada tipo char.
 */
void fm_lcd_format_u32_in_line(rows_t line, uint32_t data, char *p_str,
int length)
{
    if (line == HIGH_ROW)
    {
        snprintf(p_str, length, "%8lu", data);
    }
    else if (line == LOW_ROW)
    {
        snprintf(p_str, length, "%7lu", data);
    }
}

/*
 * @brief Es la primera instrucción a llamar para usar el lcd.
 * @param  None
 * @retval None
 */
void fm_lcd_init()
{
    lcd_clear_all();
    lcd_init();
}

/*
 * @brief Función que imprime en una de las dos filas del lcd, row 0 o row 1.
 * @param Arreglo a imprimir.
 * @param Fila donde se coloca el arreglo a imprimir.
 * @retval None
 */
void fm_lcd_puts_rows(const char *c, const rows_t row)
{
    uint8_t col = 0;
    uint8_t col_limit;

    if (row == 0)
    {
        col_limit = LINE_0_DIGITS - 1;
    }
    else
    {
        col_limit = LINE_1_DIGITS - 1;
    }

    while ((*c) && (col < col_limit))
    {
        if (((*c >= '0') && (*c <= '9')) || (*c == ' ') || (*c == '.')
        || (*c == 'A') || (*c == 'a') || (*c == 'B') || (*c == 'b')
        || (*c == 'C') || (*c == 'c') || (*c == 'D') || (*c == 'd')
        || (*c == 'E') || (*c == 'e') || (*c == 'F') || (*c == 'f')
        || (*c == 'P') || (*c == 'p') || (*c == 'S') || (*c == 's'))
        {
            lcd_put_8_seg_char(*c, col, row);
        }
        col++;
        c++;
        if (*c == '.')
        {
            col--;
        }
    }
}

/*
 * @brief Función que imprime un símbolo en la pantalla según el string que se
 * le pase como parámetro.
 * @param String que almacena dicho símbolo.
 * @retval None
 */
void fm_lcd_puts_symbol(char *p_str)
{
    if (strcmp(p_str, "ACM") == 0)
    {
        lcd_set_symbol(ACM);
    }
    else if (strcmp(p_str, "TTL") == 0)
    {
        lcd_set_symbol(TTL);
    }
    else if (strcmp(p_str, "RATE") == 0)
    {
        lcd_set_symbol(RATE);
    }
    else if (strcmp(p_str, "BATTERY") == 0)
    {
        lcd_set_symbol(BATTERY);
    }
    else if (strcmp(p_str, "POWER") == 0)
    {
        lcd_set_symbol(POWER);
    }
    else if (strcmp(p_str, "E") == 0)
    {
        lcd_set_symbol(E);
    }
    else if (strcmp(p_str, "BATCH") == 0)
    {
        lcd_set_symbol(BATCH);
    }
    else if (strcmp(p_str, "BACKSLASH") == 0)
    {
        lcd_set_symbol(BACKSLASH);
    }
    else if (strcmp(p_str, "PULSE_POINT") == 0)
    {
        lcd_set_symbol(PULSE_POINT);
    }
    else
    {
        return;
    }
}

/*
 * @brief Función que imprime una unidad de tiempo en la pantalla según el
 * string que se le pase como parámetro.
 * @param String que almacena dicha unidad.
 * @retval None
 */
void fm_lcd_puts_unit_time(char *p_str)
{
    if (strlen(p_str) == 0)
    {
        return;
    }

    if (strcmp(p_str, "M") == 0)
    {
        lcd_set_symbol_unit_time(M);
    }
    else if (strcmp(p_str, "S") == 0)
    {
        lcd_set_symbol_unit_time(S);
    }
    else if (strcmp(p_str, "D") == 0)
    {
        lcd_set_symbol_unit_time(D);
    }
    else if (strcmp(p_str, "H") == 0)
    {
        lcd_set_symbol_unit_time(H);
    }
    else if (strcmp(p_str, " ") == 0)
    {
        lcd_set_symbol_unit_time(NO_UNIT_TIME);
    }
    else
    {
        return;
    }
}

/*
 * @brief Función que imprime una unidad de volumen en la pantalla según el
 * string que se le pase como parámetro.
 * @param String que almacena dicha unidad.
 * @retval None
 */
void fm_lcd_puts_unit_volume(char *p_str)
{
    if (strlen(p_str) != 2)
    {
        return;
    }

    lcd_puts_14_seg_str_2(p_str);
}

/*
 * @brief Función que permite actualizar la pantalla llamando a una función de
 * la librería de mas bajo nivel 'lcd.h'.
 * @param  None
 * @retval None
 */
void fm_lcd_refresh()
{
    lcd_refresh();
}

/*
 * @brief Función que escribe en pantalla los segmentos pasados como parámetro.
 * @param Segmentos a imprimir.
 * @retval None
 */
void fm_lcd_write_all(uint8_t data)
{
    lcd_write_all(data);
}

// Interrupts

/*** end of file ***/
