/* @file lcd.c
 *
 * @brief Este es un driver intermedio entre fm_lcd.c y pcf8553.c. Entre las
 * responsabilidades de este modulo están:
 * Dar accesos a la escritura y borrado de cada caracter, punto y simbolo en el
 * LCD custom:
 * Fila HIGH de 8 caracteres
 * Fila LOW de 7 caracteres
 * Símbolos RATE, BACH, TTL, ACM, H, M, S, D....
 * Puntos para números decimales de cada fila (7 puntos en la fila HIGH, y
 * 6 en la fila LOW).
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "lcd.h"

// Typedef.

typedef struct
{
    uint8_t pos;
    uint8_t reg;
} octal_t;

typedef struct
{
    uint8_t pos;
    uint8_t reg;
} segments_14_t;

// Defines.

/*
 * Ver hoja de datos del lcd, cada segmento se identifica con una letra, se
 * codifica a numero. Cada segmento esta conectado a un pin del pcf8553, luego
 * cada pin esta relacionado con bit de un registro de 8 bits en el mapa de
 * memoria del pcf8553. Se debe construir una matiz que relacione los segmentos
 * con el bit que se quiere encender, esto se hará para un solo digito, luego
 * por se obtendrán las posiciones de los siguientes dígitos.
 *
 */
#define SEG_A               0
#define SEG_B               1
#define SEG_C               2
#define SEG_D               3
#define SEG_E               4
#define SEG_F               5
#define SEG_G               6
#define SEG_H               7
#define SEG_I               8
#define SEG_J               9
#define SEG_K               10
#define SEG_L               11
#define SEG_M               12
#define SEG_N               13

#define LCD_MSG_LENGTH      12

// Project variables, non-static, at least used in other file.

// Extern variables.

extern uint8_t g_lcd_map[PCF8553_DATA_SIZE];
//extern uint8_t in_configuration;

// Global variables, statics.

/*
 * Lo que se quiera escribir en las líneas 1 y 2 primero se vuelca a este
 * buffer. Leer el buffer es la unica manera practica que se tiene para
 * saber que esta escrito en la pantalla, no se debe corromper esta condicion.
 */
static uint8_t g_buf[LCD_ROWS][LCD_COLS];
static uint8_t g_col;
static uint8_t g_row;

/*
 *  Para un caracter octal, en la linea superior del LCD, inicializo el
 *  siguiente arreglo. Ver tabla memoria del PCF8553, el encendido/apagado
 *  de cada segmento esta controlado por un bit en esta tabla, son 20 registros
 *  de 8 bits cada uno, 160 bits par controlar la misma cantidad de segmentos.
 *  Los valores de inicialización de la siguiente tabla corresponde al caracter
 *  ubicado mas a la derecha en la tabla superior, ver datasheet del LCD.
 *  Cada par de valores pos y reg se corresponden a la direccion de un registro
 *  y el bit correspondiente que controlan los segmentos de A a G.
 *  Solo se necesitan los datos de este caracter, las posiciones de los demas
 *  se obtienen por aritmetica dentro de la funcion lcd_write_line.
 */
octal_t octal_1[LINE_0_DIGITS - 1] =
{
    {
        .pos = 6,
        .reg = 7
    },
    {
        .pos = 6,
        .reg = 2
    },
    {
        .pos = 7,
        .reg = 2
    },
    {
        .pos = 6,
        .reg = 17
    },
    {
        .pos = 6,
        .reg = 12
    },
    {
        .pos = 7,
        .reg = 17
    },
    {
        .pos = 7,
        .reg = 12
    },
    {
        .pos = 7,
        .reg = 7
    },
};

octal_t octal_2[LINE_0_DIGITS - 1] =
{
    {
        .pos = 7,
        .reg = 5
    },
    {
        .pos = 7,
        .reg = 0
    },
    {
        .pos = 6,
        .reg = 0
    },
    {
        .pos = 7,
        .reg = 15
    },
    {
        .pos = 7,
        .reg = 10
    },
    {
        .pos = 6,
        .reg = 15
    },
    {
        .pos = 6,
        .reg = 10
    },
    {
        .pos = 6,
        .reg = 5
    }
};

segments_14_t left_14_segments[SEGMENTS_14_SIZE] =
{
    {
        .pos = 5, //SEG_A
        .reg = 5
    },
    {
        .pos = 5, //SEG_B
        .reg = 0
    },
    {
        .pos = 3, //SEG_C
        .reg = 0
    },
    {
        .pos = 2, //SEG_D
        .reg = 15
    },
    {
        .pos = 5, //SEG_E
        .reg = 15
    },
    {
        .pos = 3, //SEG_F
        .reg = 15
    },
    {
        .pos = 3, //SEG_G
        .reg = 10
    },
    {
        .pos = 4, //SEG_H
        .reg = 0
    },
    {
        .pos = 2, //SEG_I
        .reg = 0
    },
    {
        .pos = 2, //SEG_J
        .reg = 5
    },
    {
        .pos = 4, //SEG_K
        .reg = 5
    },
    {
        .pos = 2, //SEG_L
        .reg = 10
    },
    {
        .pos = 4, //SEG_M
        .reg = 10
    },
    {
        .pos = 4, //SEG_N
        .reg = 15
    }
};

segments_14_t right_14_segments[SEGMENTS_14_SIZE] =
{
    {
        .pos = 1, //SEG_A
        .reg = 5
    },
    {
        .pos = 1, //SEG_B
        .reg = 0
    },
    {
        .pos = 7, //SEG_C
        .reg = 4
    },
    {
        .pos = 6, //SEG_D
        .reg = 19
    },
    {
        .pos = 1, //SEG_E
        .reg = 15
    },
    {
        .pos = 7, //SEG_F
        .reg = 19
    },
    {
        .pos = 7, //SEG_G
        .reg = 14
    },
    {
        .pos = 0, //SEG_H
        .reg = 0
    },
    {
        .pos = 6, //SEG_I
        .reg = 4
    },
    {
        .pos = 6, //SEG_J
        .reg = 9
    },
    {
        .pos = 0, //SEG_K
        .reg = 5
    },
    {
        .pos = 6, //SEG_L
        .reg = 14
    },
    {
        .pos = 0, //SEG_M
        .reg = 10
    },
    {
        .pos = 0, //SEG_N
        .reg = 15
    }
};

// Private function prototypes.

void lcd_write_line(uint8_t seg, uint8_t data);

void lcd_write_14_line(uint8_t seg, uint8_t data, uint8_t col);

// Public function bodies.

/*
 * @brief Función que limpia toda la pantalla y los arreglos que contienen los
 * bits de los segmentos a imprimir.
 * @param  None
 * @retval None
 */
void lcd_clear_all()
{
    pcf8553_clear_buff();

    /*
     * Limpia el buffer intermedio de los números que aparecen en las filas HIGH
     * y LOW de la pantalla LCD. No es muy eficiente si se pretende usar al
     * refrescar la pantalla cada x tiempo, pero será necesario usarla por el
     * momento al pasar de una pantalla a otra.
     */
    for (int cont_buff_row = 0; cont_buff_row < LCD_ROWS; cont_buff_row++)
    {
        for (int cont_buff_col = 0; cont_buff_col < LCD_COLS; cont_buff_col++)
        {
            g_buf[cont_buff_row][cont_buff_col] = 0;
        }
    }

    pcf8553_write_all(NONE_SEGMENTS);
}

/*
 * @brief Función que limpia de la pantalla (pero no del buffer intermedio) los
 * dígitos de la fila que se pasa como segundo parámetro.
 * @param Dígito a borrar de la pantalla.
 * @param Fila de este dígito.
 * @retval None
 */
void lcd_clear_digit(screen_digits_t digit, rows_t line)
{
    if (line == HIGH_ROW)
    {
        switch (digit)
        {
            case DIGIT_0:
                g_lcd_map[REG_12] &= ~(1 << BIT_7);
                g_lcd_map[REG_12] &= ~(1 << BIT_6);
                g_lcd_map[REG_17] &= ~(1 << BIT_7);
                g_lcd_map[REG_17] &= ~(1 << BIT_6);
                g_lcd_map[REG_2] &= ~(1 << BIT_6);
                g_lcd_map[REG_2] &= ~(1 << BIT_7);
                g_lcd_map[REG_7] &= ~(1 << BIT_6);
            break;
            case DIGIT_1:
                g_lcd_map[REG_13] &= ~(1 << BIT_1);
                g_lcd_map[REG_13] &= ~(1 << BIT_0);
                g_lcd_map[REG_18] &= ~(1 << BIT_1);
                g_lcd_map[REG_18] &= ~(1 << BIT_0);
                g_lcd_map[REG_3] &= ~(1 << BIT_0);
                g_lcd_map[REG_3] &= ~(1 << BIT_1);
                g_lcd_map[REG_8] &= ~(1 << BIT_0);
            break;
            case DIGIT_2:
                g_lcd_map[REG_13] &= ~(1 << BIT_3);
                g_lcd_map[REG_13] &= ~(1 << BIT_2);
                g_lcd_map[REG_18] &= ~(1 << BIT_3);
                g_lcd_map[REG_18] &= ~(1 << BIT_2);
                g_lcd_map[REG_3] &= ~(1 << BIT_2);
                g_lcd_map[REG_3] &= ~(1 << BIT_3);
                g_lcd_map[REG_8] &= ~(1 << BIT_2);
            break;
            case DIGIT_3:
                g_lcd_map[REG_13] &= ~(1 << BIT_5);
                g_lcd_map[REG_13] &= ~(1 << BIT_4);
                g_lcd_map[REG_18] &= ~(1 << BIT_5);
                g_lcd_map[REG_18] &= ~(1 << BIT_4);
                g_lcd_map[REG_3] &= ~(1 << BIT_4);
                g_lcd_map[REG_3] &= ~(1 << BIT_5);
                g_lcd_map[REG_8] &= ~(1 << BIT_4);
            break;
            case DIGIT_4:
                g_lcd_map[REG_13] &= ~(1 << BIT_7);
                g_lcd_map[REG_13] &= ~(1 << BIT_6);
                g_lcd_map[REG_18] &= ~(1 << BIT_7);
                g_lcd_map[REG_18] &= ~(1 << BIT_6);
                g_lcd_map[REG_3] &= ~(1 << BIT_6);
                g_lcd_map[REG_3] &= ~(1 << BIT_7);
                g_lcd_map[REG_8] &= ~(1 << BIT_6);
            break;
            case DIGIT_5:
                g_lcd_map[REG_14] &= ~(1 << BIT_1);
                g_lcd_map[REG_14] &= ~(1 << BIT_0);
                g_lcd_map[REG_19] &= ~(1 << BIT_1);
                g_lcd_map[REG_19] &= ~(1 << BIT_0);
                g_lcd_map[REG_4] &= ~(1 << BIT_0);
                g_lcd_map[REG_4] &= ~(1 << BIT_1);
                g_lcd_map[REG_9] &= ~(1 << BIT_0);
            break;
            case DIGIT_6:
                g_lcd_map[REG_14] &= ~(1 << BIT_3);
                g_lcd_map[REG_14] &= ~(1 << BIT_2);
                g_lcd_map[REG_19] &= ~(1 << BIT_3);
                g_lcd_map[REG_19] &= ~(1 << BIT_2);
                g_lcd_map[REG_4] &= ~(1 << BIT_2);
                g_lcd_map[REG_4] &= ~(1 << BIT_3);
                g_lcd_map[REG_9] &= ~(1 << BIT_2);
            break;
            case DIGIT_7:
                g_lcd_map[REG_14] &= ~(1 << BIT_5);
                g_lcd_map[REG_14] &= ~(1 << BIT_4);
                g_lcd_map[REG_19] &= ~(1 << BIT_5);
                g_lcd_map[REG_19] &= ~(1 << BIT_4);
                g_lcd_map[REG_4] &= ~(1 << BIT_4);
                g_lcd_map[REG_4] &= ~(1 << BIT_5);
                g_lcd_map[REG_9] &= ~(1 << BIT_4);
            break;
            default:
            break;
        }
    }
    else if (line == LOW_ROW)
    {
        switch (digit)
        {
            case DIGIT_0:
                g_lcd_map[REG_12] &= ~(1 << BIT_2);
                g_lcd_map[REG_12] &= ~(1 << BIT_3);
                g_lcd_map[REG_17] &= ~(1 << BIT_2);
                g_lcd_map[REG_17] &= ~(1 << BIT_3);
                g_lcd_map[REG_2] &= ~(1 << BIT_2);
                g_lcd_map[REG_2] &= ~(1 << BIT_3);
                g_lcd_map[REG_7] &= ~(1 << BIT_3);
            break;
            case DIGIT_1:
                g_lcd_map[REG_12] &= ~(1 << BIT_0);
                g_lcd_map[REG_12] &= ~(1 << BIT_1);
                g_lcd_map[REG_17] &= ~(1 << BIT_0);
                g_lcd_map[REG_17] &= ~(1 << BIT_1);
                g_lcd_map[REG_2] &= ~(1 << BIT_0);
                g_lcd_map[REG_2] &= ~(1 << BIT_1);
                g_lcd_map[REG_7] &= ~(1 << BIT_1);
            break;
            case DIGIT_2:
                g_lcd_map[REG_11] &= ~(1 << BIT_6);
                g_lcd_map[REG_11] &= ~(1 << BIT_7);
                g_lcd_map[REG_16] &= ~(1 << BIT_6);
                g_lcd_map[REG_16] &= ~(1 << BIT_7);
                g_lcd_map[REG_1] &= ~(1 << BIT_6);
                g_lcd_map[REG_1] &= ~(1 << BIT_7);
                g_lcd_map[REG_6] &= ~(1 << BIT_7);
            break;
            case DIGIT_3:
                g_lcd_map[REG_11] &= ~(1 << BIT_4);
                g_lcd_map[REG_11] &= ~(1 << BIT_5);
                g_lcd_map[REG_16] &= ~(1 << BIT_4);
                g_lcd_map[REG_16] &= ~(1 << BIT_5);
                g_lcd_map[REG_1] &= ~(1 << BIT_4);
                g_lcd_map[REG_1] &= ~(1 << BIT_5);
                g_lcd_map[REG_6] &= ~(1 << BIT_5);
            break;
            case DIGIT_4:
                g_lcd_map[REG_11] &= ~(1 << BIT_2);
                g_lcd_map[REG_11] &= ~(1 << BIT_3);
                g_lcd_map[REG_16] &= ~(1 << BIT_2);
                g_lcd_map[REG_16] &= ~(1 << BIT_3);
                g_lcd_map[REG_1] &= ~(1 << BIT_2);
                g_lcd_map[REG_1] &= ~(1 << BIT_3);
                g_lcd_map[REG_6] &= ~(1 << BIT_3);
            break;
            case DIGIT_5:
                g_lcd_map[REG_11] &= ~(1 << BIT_0);
                g_lcd_map[REG_11] &= ~(1 << BIT_1);
                g_lcd_map[REG_16] &= ~(1 << BIT_0);
                g_lcd_map[REG_16] &= ~(1 << BIT_1);
                g_lcd_map[REG_1] &= ~(1 << BIT_0);
                g_lcd_map[REG_1] &= ~(1 << BIT_1);
                g_lcd_map[REG_6] &= ~(1 << BIT_1);
            break;
            case DIGIT_6:
                g_lcd_map[REG_10] &= ~(1 << BIT_6);
                g_lcd_map[REG_10] &= ~(1 << BIT_7);
                g_lcd_map[REG_15] &= ~(1 << BIT_6);
                g_lcd_map[REG_15] &= ~(1 << BIT_7);
                g_lcd_map[REG_0] &= ~(1 << BIT_6);
                g_lcd_map[REG_0] &= ~(1 << BIT_7);
                g_lcd_map[REG_5] &= ~(1 << BIT_7);
            break;
            default:
            break;
        }
    }
}

/*
 * @brief Función que borra un punto en específico que estaba escrito en la
 * pantalla.
 * @param Enumeraciones rows_t y point_t de lcd.h
 * @retval None
 */
void lcd_clear_point(rows_t line, point_t point)
{
    if (line == HIGH_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] &= ~(1 << BIT_7);
            break;
            case PNT_1:
                g_lcd_map[REG_8] &= ~(1 << BIT_1);
            break;
            case PNT_2:
                g_lcd_map[REG_8] &= ~(1 << BIT_3);
            break;
            case PNT_3:
                g_lcd_map[REG_8] &= ~(1 << BIT_5);
            break;
            case PNT_4:
                g_lcd_map[REG_8] &= ~(1 << BIT_7);
            break;
            case PNT_5:
                g_lcd_map[REG_9] &= ~(1 << BIT_1);
            break;
            case PNT_6:
                g_lcd_map[REG_9] &= ~(1 << BIT_3);
            break;
            default:
            break;
        }
    }
    else if (line == LOW_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] &= ~(1 << BIT_2);
            break;
            case PNT_1:
                g_lcd_map[REG_7] &= ~(1 << BIT_0);
            break;
            case PNT_2:
                g_lcd_map[REG_6] &= ~(1 << BIT_6);
            break;
            case PNT_3:
                g_lcd_map[REG_6] &= ~(1 << BIT_4);
            break;
            case PNT_4:
                g_lcd_map[REG_6] &= ~(1 << BIT_2);
            break;
            case PNT_5:
                g_lcd_map[REG_6] &= ~(1 << BIT_0);
            break;
            default:
            break;
        }
    }
}

/*
 * @brief Función que limpia de la pantalla un símbolo especificado como
 * parámetro.
 * @param Símbolo a borrar de la pantalla de la enum symbols_t.
 * @retval None
 */
void lcd_clear_symbol(symbols_t symbol)
{
    switch(symbol)
    {
        case ACM:
            g_lcd_map[REG_12] &= ~(1 << BIT_4);
        break;
        case TTL:
            g_lcd_map[REG_17] &= ~(1 << BIT_4);
        break;
        case RATE:
            g_lcd_map[REG_17] &= ~(1 << BIT_5);
        break;
        case BATTERY:
            g_lcd_map[REG_7]  &= ~(1 << BIT_5);
        break;
        case POWER:
            g_lcd_map[REG_2]  &= ~(1 << BIT_5);
        break;
        case E:
            g_lcd_map[REG_2]  &= ~(1 << BIT_4);
        break;
        case BATCH:
            g_lcd_map[REG_12] &= ~(1 << BIT_5);
        break;
        case BACKSLASH:
            g_lcd_map[REG_10] &= ~(1 << BIT_1);
        break;
        case PULSE_POINT:
            g_lcd_map[REG_7]  &= ~(1 << BIT_4);
        break;
        default:
        break;
    }
}

/*
 * @brief Inicialización de la pantalla LCD mediante el puerto SPI1.
 * @param  None
 * @retval None
 */
void lcd_init()
{
    pcf8553_init();
}

/*
 * @brief Función que escribe en pantalla un caracter de 8 segmentos en la
 * columna y fila elegidas.
 * @param caracter a escribir tipo char, columna y fila de tipo uint8_t
 * @retval None
 */
void lcd_put_8_seg_char(char c, uint8_t col, uint8_t row)
{
    g_row = row + 1;

    /*
     * Si el buffer contiene lo mismo que se va a escribir salteo la escritura.
     * Aunque esto mejora mucho la eficiencia, no tengo que escribir por SPI
     * este caracter, considero muy riesgosa esta tecnica, la sincronización
     * entre el buffer y el contenido de la memoria del pcf8553 se debe
     * asegurar. El uso del return prematuro no se si es buena práctica.
     *
     * No se activa esta funcionalidad si se está en el menú de configuración,
     * debido a que entra en conflicto con la funcionalidad del parpadeo de los
     * campos a modificar.
     */
//    if (in_configuration == 0)
//    {
//        if (g_buf[row][col] == c)
//        {
//            return;
//        }
//    }

    g_buf[row][col] = c;

    switch (row)
    {
        case 0:
            if (col < LINE_0_DIGITS - 1)
            {
                g_col = col;
            }
            else
            {
                return;
            }
        break;
        case 1:
            if (col < LINE_1_DIGITS - 1)
            {
                g_col = 6 - col; // @suppress("Avoid magic numbers")
            }
            else
            {
                return;
            }

        break;
        default:
            return;
        break;
    }

    switch (c)
    {
        case ' ':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case '.':
            lcd_write_line(SEG_H, 1);
        break;
        case '0':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '1':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case '2':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '3':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '4':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case '5':
        case 'S':
        case 's':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '6':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '7':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '8':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '9':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case 'A':
        case 'a':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case 'B':
        case 'b':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case 'C':
        case 'c':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case 'D':
        case 'd':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case 'E':
        case 'e':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case 'F':
        case 'f':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case 'P':
        case 'p':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        default:
        break;
    }
}

/*
 * @brief Función que escribe en pantalla dos caracteres de 14 segmentos c/u en
 * la fila inferior.
 * @param String formada por dos caracteres (obligatoriamente 2) que indica los
 * segmentos a encender.
 * @retval None
 */
void lcd_puts_14_seg_str_2(char *p_str)
{
    static uint8_t sel_col_puts_2 = 0;

    while(sel_col_puts_2 <= 1)
    {
        switch(p_str[sel_col_puts_2])
        {
            case '0':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '1':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '2':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '3':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '4':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '5':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '6':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '7':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '8':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '9':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'A':
            case 'a':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'B':
            case 'b':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'C':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'c':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'D':
            case 'd':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'E':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'e':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'F':
            case 'f':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'G':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'g':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'H':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'h':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'I':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'i':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'J':
            case 'j':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'K':
            case 'k':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case 'L':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'l':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'M':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case 'm':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'N':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'n':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'O':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'o':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'P':
            case 'p':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'Q':
            case 'q':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'R':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'r':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'S':
            case 's':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'T':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 't':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'U':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'u':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'V':
            case 'v':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case 'W':
            case 'w':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case 'X':
            case 'x':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case 'Y':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case 'y':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case 'Z':
            case 'z':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case ' ':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '-':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '+':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '*':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case '/':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
            case '_':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '=':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '(':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '[':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case ')':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case ']':
                lcd_write_14_line(SEG_A, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case ',':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '`':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            case '"':
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 0, sel_col_puts_2);
                break;
            default:
                lcd_write_14_line(SEG_A, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_B, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_C, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_D, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_E, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_F, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_G, 0, sel_col_puts_2);
                lcd_write_14_line(SEG_H, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_I, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_J, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_K, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_L, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_M, 1, sel_col_puts_2);
                lcd_write_14_line(SEG_N, 1, sel_col_puts_2);
                break;
        }

        sel_col_puts_2++;
    }

    sel_col_puts_2 = 0;
}

/*
 * @brief Función que envía por medio de un buffer lo escrito en la pantalla
 * anteriormente. Es un método mas eficiente primero escribir lo que se quiera
 * enviar en dicho buffer y luego enviar todo junto.
 * @param None
 * @retval None
 */
void lcd_refresh()
{
    pcf8553_dump();
}

/*
 * @brief Función que escribe un punto en específico en la pantalla.
 * @param Enumeraciones rows_t y point_t de lcd.h
 * @retval None
 */
void lcd_set_point(rows_t line, point_t point)
{
    if (line == HIGH_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] |= (1 << BIT_7);
            break;
            case PNT_1:
                g_lcd_map[REG_8] |= (1 << BIT_1);
            break;
            case PNT_2:
                g_lcd_map[REG_8] |= (1 << BIT_3);
            break;
            case PNT_3:
                g_lcd_map[REG_8] |= (1 << BIT_5);
            break;
            case PNT_4:
                g_lcd_map[REG_8] |= (1 << BIT_7);
            break;
            case PNT_5:
                g_lcd_map[REG_9] |= (1 << BIT_1);
            break;
            case PNT_6:
                g_lcd_map[REG_9] |= (1 << BIT_3);
            break;
            default:
            break;
        }
    }
    else if (line == LOW_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] |= (1 << BIT_2);
            break;
            case PNT_1:
                g_lcd_map[REG_7] |= (1 << BIT_0);
            break;
            case PNT_2:
                g_lcd_map[REG_6] |= (1 << BIT_6);
            break;
            case PNT_3:
                g_lcd_map[REG_6] |= (1 << BIT_4);
            break;
            case PNT_4:
                g_lcd_map[REG_6] |= (1 << BIT_2);
            break;
            case PNT_5:
                g_lcd_map[REG_6] |= (1 << BIT_0);
            break;
            default:
            break;
        }
    }
}

/*
 * @brief Función que escribe en la pantalla un símbolo especificado como
 * parámetro.
 * @param Símbolo a escribir en la pantalla de la enum symbols_t.
 * @retval None
 */
void lcd_set_symbol(symbols_t symbol)
{
    switch(symbol)
     {
         case ACM:
             g_lcd_map[REG_12] |= (1 << BIT_4);
         break;
         case TTL:
             g_lcd_map[REG_17] |= (1 << BIT_4);
         break;
         case RATE:
             g_lcd_map[REG_17] |= (1 << BIT_5);
         break;
         case BATTERY:
             g_lcd_map[REG_7]  |= (1 << BIT_5);
         break;
         case POWER:
             g_lcd_map[REG_2]  |= (1 << BIT_5);
         break;
         case E:
             g_lcd_map[REG_2]  |= (1 << BIT_4);
         break;
         case BATCH:
             g_lcd_map[REG_12] |= (1 << BIT_5);
         break;
         case BACKSLASH:
             g_lcd_map[REG_10] |= (1 << BIT_1);
         break;
         case PULSE_POINT:
             g_lcd_map[REG_7]  |= (1 << BIT_4);
         break;
         default:
         break;
     }
}

/*
 * @brief Función que escribe en la pantalla un segmento correspondiente a una
 * unidad de tiempo que se le pasa como parámetro.
 * @param Segmento a escribir de la pantalla de la enum symbols_t.
 * @retval None
 */
void lcd_set_symbol_unit_time(symbols_t symbol)
{
    switch(symbol)
    {
        case H:
            g_lcd_map[REG_5]  |= (1 << BIT_6);
        break;
        case D:
            g_lcd_map[REG_9]  |= (1 << BIT_5);
        break;
        case S:
            g_lcd_map[REG_10] |= (1 << BIT_5);
        break;
        case M:
            g_lcd_map[REG_9]  |= (1 << BIT_7);
        break;
        default:
        break;
    }
}

/*
 * @brief Función que escribe en pantalla los segmentos pasados como parámetro.
 * @param Segmentos a imprimir.
 * @retval None
 */
void lcd_write_all(uint8_t data)
{
    pcf8553_write_all(data);
}

// Private function bodies.

/*
 * @brief Esta funcion ajusta el estado de un segmento, encendido o apagado,
 * por cada llamada. El caracter a manipular, linea 1 o linea 2 y la posicion
 * del cursor, son informaciones guardadas en dos variables globales, g_cursor
 * y g_line. Con el mapeo de los segmentos, y sabiendo que segmento se quiere
 * manipular, notar que para el primer caracter de cada linea se tiene una matriz
 * con la posicion en el mapa de memoria para el pcf8553, con lo cual no se
 * requiere calculo, el calculo es de utilidad para los caracteres que no ocupan
 * la primera posicion de cada linea.
 * @param
 * @retval None
 */
void lcd_write_line(uint8_t seg, uint8_t data)
{
    uint8_t reg = 0;
    uint8_t pos = 0;

    /*
     *
     *
     */
    switch (g_row)
    {
        case 1:
            pos = octal_1[seg].pos;
            pos += g_col * 2;
            reg = (pos / 8) + octal_1[seg].reg; // @suppress("Avoid magic numbers")
            pos = pos % 8; // @suppress("Avoid magic numbers")
        break;
        case 2:
            pos = octal_2[seg].pos;
            pos += g_col * 2;
            reg = (pos / 8) + octal_2[seg].reg; // @suppress("Avoid magic numbers")
            pos = pos % 8; // @suppress("Avoid magic numbers")
        break;
        default:
        break;
    }

    if (data)
    {
        g_lcd_map[reg] |= 1 << pos;
    }
    else
    {
        g_lcd_map[reg] &= ~(1 << pos);
    }
}

/*
 * @brief Función de funcionamiento similar a lcd_write_8_line, pero para los
 * dos conjuntos de 14 segmentos de la fila inferior, típicamente usados para
 * unidades de volumen, aunque pueden tener otros fines.
 * @param Segmento a imprimir o borrar, indicación de si se escribe o se borra
 * y en cual de los dos conjuntos de 14 segmentos se trabaja.
 * @retval None
 */
void lcd_write_14_line(uint8_t seg, uint8_t data, uint8_t col)
{
    if(col == 0)
    {
        if (data)
        {
            g_lcd_map[left_14_segments[seg].reg] |= (1 << left_14_segments[seg].pos);
        }
        else
        {
            g_lcd_map[left_14_segments[seg].reg] &= ~(1 << left_14_segments[seg].pos);
        }
    }
    else
    {
        if (data)
        {
            g_lcd_map[right_14_segments[seg].reg] |= (1 << right_14_segments[seg].pos);
        }
        else
        {
            g_lcd_map[right_14_segments[seg].reg] &= ~(1 << right_14_segments[seg].pos);
        }
    }
}

/*** end of file ***/
