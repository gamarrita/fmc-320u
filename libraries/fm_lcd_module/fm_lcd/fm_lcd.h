/* @file fm_lcd.h
 *
 * @brief
 *
 * Este es el modulo que posee la inteligencia para manejar las opciones que
 * brinda la pantalla LCD, utilizando las funciones implementadas de impresión
 * de caracteres individuales en lcd.h.
 * Las funciones de este modulo serán usadas por librerías de mas alto nivel que
 * impriman menús o controlen máquinas de estado de pantallas, tales como
 * fm_menu_user.h.
 *
 * Modo de uso:
 *
 * 1) Al iniciar un nuevo proyecto puede usar el archivo lcd.ioc para configurar
 * automaticamente.
 *
 * 2) Se debe agregar la carpeta al proyecto, sobre el nombre del proyecto click
 * derecho new -> folder -> link to alternate location -> ....
 *
 * 3) Se debe indicar al IDE que hay una nueva carpeta con codigo fuente click
 * derecho en el nombre del proyecto new -> source folder-> browse -> ....
 *
 * 4) #include "../../../../libs/fm_lcd/fm_lcd.h"
 *
 * 5) fm_lcd_init();
 *
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

#ifndef     FM_LCD_H_
#define     FM_LCD_H_

// Includes.

#include "main.h"
#include "../lcd/lcd.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes.

void fm_lcd_clear();
void fm_lcd_clear_symbol_all();
void fm_lcd_fill();
void fm_lcd_format_u32_in_line(rows_t line, uint32_t data, char *p_str,
int length);
void fm_lcd_init();
void fm_lcd_puts_rows(const char *p_str, uint8_t row);
void fm_lcd_puts_symbol(char *p_str);
void fm_lcd_puts_unit_time(char *p_str);
void fm_lcd_puts_unit_volume(char *p_str);
void fm_lcd_refresh();
void fm_lcd_write_all(uint8_t data);

#endif /* FM_LCD_H_ */

/*** end of file ***/
