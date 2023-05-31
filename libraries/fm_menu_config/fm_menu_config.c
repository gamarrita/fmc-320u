/* @file fm_menu_config.c
 *
 * @brief
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_menu_config.h"

// Typedef.

// Const data.

// Defines.

// Debug.

// Project variables, non-static, at least used in other file.

int freeze_time;
int freeze_date;
uint8_t correct_password;
uint8_t in_configuration;

// External variables.

extern TX_QUEUE event_queue_ptr;

extern fm_event_t actual_event;
extern fm_event_t previous_event;

extern time_freeze_t time_freeze;
extern date_freeze_t date_freeze;

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que imprime el menú que permite introducir una contraseña del
 * caudalímetro. Posee un algoritmo que levanta una flag si la contraseña,
 * compuesta por las 4 teclas del teclado mecánico, es la misma que la
 * de configuración (DOWN -> UP -> UP -> ENTER) o la de operario (DOWN ->
 * DOWN -> UP -> UP). Si se introduce mal la contraseña se deja volverlo a
 * intentar hasta dos veces. A la tercera vez de ingresada incorrectamente, se
 * regresa al menú de inicio donde se encienden todos los leds.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_pass(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    static uint8_t password_try = 0;
    static const uint8_t password_try_max = 3;
    static const uint8_t password_length = 4;

    /*
     * Arreglo estático que almacena la contraseña que ingresa el usuario.
     * Inicialmente está relleno con ceros.
     */
    static uint8_t password[4] = // @suppress("Avoid magic numbers")
    {
        0,
        0,
        0,
        0
    };
    static uint8_t password_index = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        in_configuration = 1;
        correct_password = 0;
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_flowmeter_password(password_index);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            password[password_index] = 1;

            if (password_index < password_length - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
            }
        break;
        case EVENT_KEY_DOWN:
            password[password_index] = 2;

            if (password_index < password_length - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
            }
        break;
        case EVENT_KEY_ENTER:
            password[password_index] = 3; // @suppress("Avoid magic numbers")

            if (password_index < password_length - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
            }
        break;
        case EVENT_KEY_ESC:
            password[password_index] = 4; // @suppress("Avoid magic numbers")

            if (password_index < password_length - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
            }
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;

#ifdef FM_DEBUG_UART_TX_MENU_ENTER
        char msg_buffer[] = "Password\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1 && password_index >= password_length - 1)
    {
        /*
         * Si la contraseña ingresada es correcta, se activa una flag global que
         * permite modificar los parámetros de los menús de configuración.
         */
        if (password[0] == PASS_DOWN && password[1] == PASS_UP
        && password[2] == PASS_UP
        && password[password_length - 1] == PASS_ENTER)
        {
            correct_password = 1;

//            ret_menu = (ptr_ret_menu_t) fm_menu_config_k_param;
//            event_now = EVENT_LCD_REFRESH;
//            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);

            password_try = 0;
        }
        else if (password[0] == PASS_DOWN && password[1] == PASS_DOWN
        && password[2] == PASS_UP && password[password_length - 1] == PASS_UP)
        {
            correct_password = 1;

            ret_menu = (ptr_ret_menu_t) fm_menu_config_units_vol;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);

            date_freeze.dd = fm_calendar_get_day();
            date_freeze.mm = fm_calendar_get_month();
            date_freeze.aaaa = fm_calendar_get_year();

            time_freeze.hh = fm_calendar_get_hour();
            time_freeze.mm = fm_calendar_get_minute();
            time_freeze.ss = fm_calendar_get_second();

            password_try = 0;
        }
        else
        {
            correct_password = 0;
            password_try++;

            if (password_try < password_try_max)
            {
                ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
            else
            {
                ret_menu = (ptr_ret_menu_t) fm_menu_show_init;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);

                password_try = 0;
            }
        }

        /*
         * Reinicio el arreglo de la contraseña ingresada.
         */
        password_index = PASS_0;
        password[0] = PASS_0;
        password[1] = PASS_0;
        password[2] = PASS_0;
        password[password_length - 1] = PASS_0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de configuración tanto de las unidades de
 * volumen y tiempo, como de la resolución de las medidas.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_units_tim(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_units_tim;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    actual_event = event_id;
    fm_lcd_flowmeter_units_tim();
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                if (strcmp(fm_factory_get_rate().unit_time, "H") == 0)
                {
                    fm_factory_modify_units_time("D");
                }
                else if (strcmp(fm_factory_get_rate().unit_time, "D") == 0)
                {
                    fm_factory_modify_units_time("S");
                }
                else if (strcmp(fm_factory_get_rate().unit_time, "S") == 0)
                {
                    fm_factory_modify_units_time("M");
                }
                else if (strcmp(fm_factory_get_rate().unit_time, "M") == 0)
                {
                    fm_factory_modify_units_time("H");
                }
                fm_lcd_clear();
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                if (strcmp(fm_factory_get_rate().unit_time, "H") == 0)
                {
                    fm_factory_modify_units_time("M");
                }
                else if (strcmp(fm_factory_get_rate().unit_time, "M") == 0)
                {
                    fm_factory_modify_units_time("S");
                }
                else if (strcmp(fm_factory_get_rate().unit_time, "S") == 0)
                {
                    fm_factory_modify_units_time("D");
                }
                else if (strcmp(fm_factory_get_rate().unit_time, "D") == 0)
                {
                    fm_factory_modify_units_time("H");
                }
                fm_lcd_clear();
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if (fm_factory_get_res_rate() == LCD_FLOWMETER_RES_0)
                {
                    fm_factory_modify_res_rate(LCD_FLOWMETER_RES_1);
                }
                else if (fm_factory_get_res_rate() == LCD_FLOWMETER_RES_1)
                {
                    fm_factory_modify_res_rate(LCD_FLOWMETER_RES_2);
                }
                else if (fm_factory_get_res_rate() == LCD_FLOWMETER_RES_2)
                {
                    fm_factory_modify_res_rate(LCD_FLOWMETER_RES_3);
                }
                else if (fm_factory_get_res_rate() == LCD_FLOWMETER_RES_3)
                {
                    fm_factory_modify_res_rate(LCD_FLOWMETER_RES_0);
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_init;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;

#ifdef FM_DEBUG_UART_TX_MENU_ENTER
        char msg_buffer[] = "Configurar unidades de tiempo y resolucion\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de configuración tanto de las unidades de
 * volumen y tiempo, como de la resolución de las medidas.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_units_vol(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_units_vol;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    actual_event = event_id;
    fm_lcd_flowmeter_units_vol();
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                if (strcmp(fm_factory_get_acm().unit_volume, "LT") == 0)
                {
                    fm_factory_modify_units_volume("M3");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "M3") == 0)
                {
                    fm_factory_modify_units_volume("MC");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "MC") == 0)
                {
                    fm_factory_modify_units_volume("KG");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "KG") == 0)
                {
                    fm_factory_modify_units_volume("GL");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "GL") == 0)
                {
                    fm_factory_modify_units_volume("BR");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "BR") == 0)
                {
                    fm_factory_modify_units_volume("--");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "--") == 0)
                {
                    fm_factory_modify_units_volume("LT");
                }
                fm_lcd_clear();
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                if (strcmp(fm_factory_get_acm().unit_volume, "LT") == 0)
                {
                    fm_factory_modify_units_volume("--");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "--") == 0)
                {
                    fm_factory_modify_units_volume("BR");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "BR") == 0)
                {
                    fm_factory_modify_units_volume("GL");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "GL") == 0)
                {
                    fm_factory_modify_units_volume("KG");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "KG") == 0)
                {
                    fm_factory_modify_units_volume("MC");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "MC") == 0)
                {
                    fm_factory_modify_units_volume("M3");
                }
                else if (strcmp(fm_factory_get_acm().unit_volume, "M3") == 0)
                {
                    fm_factory_modify_units_volume("LT");
                }
                fm_lcd_clear();
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if (fm_factory_get_res_acm_ttl() == LCD_FLOWMETER_RES_0)
                {
                    fm_factory_modify_res_acm_ttl(LCD_FLOWMETER_RES_1);
                }
                else if (fm_factory_get_res_acm_ttl() == LCD_FLOWMETER_RES_1)
                {
                    fm_factory_modify_res_acm_ttl(LCD_FLOWMETER_RES_2);
                }
                else if (fm_factory_get_res_acm_ttl() == LCD_FLOWMETER_RES_2)
                {
                    fm_factory_modify_res_acm_ttl(LCD_FLOWMETER_RES_3);
                }
                else if (fm_factory_get_res_acm_ttl() == LCD_FLOWMETER_RES_3)
                {
                    fm_factory_modify_res_acm_ttl(LCD_FLOWMETER_RES_0);
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_units_tim;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
#ifdef FM_DEBUG_UART_TX_MENU_ENTER
    char msg_buffer[] = "Configurar unidades de volumen y resolucion\n";
    fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
#endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

// Interrupts

/*** end of file ***/
