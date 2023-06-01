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
 * @brief
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_date_hour(fm_event_t event_id)
{
    /*Inicio de sección de variables locales estáticas*/
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_date_time_field_t field = FIELD_DAY;
    /*Fin de sección de variables locales estáticas*/

    /*Inicio de sección de otras variables no estáticas*/
    RTC_TimeTypeDef time_final;
    RTC_DateTypeDef date_final;
    extern RTC_HandleTypeDef hrtc;
    /*Fin de sección de otras variables no estáticas*/

    /*Inicio de sección de variables de punteros a función y eventos*/
    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_date_hour;
    fm_event_t event_now;
    /*Fin de sección de variables de punteros a función y eventos*/

    /*Inicio de sección de primer entrada al menú*/
    if (new_entry == 1)
    {
        field = FIELD_DAY;
        fm_lcd_clear();
        new_entry = 0;
    }
    /*Fin de sección de primer entrada al menú*/

    /*Inicio de sección de pantalla lcd*/
    fm_lcd_flowmeter_freeze_date_hour(field);
    fm_lcd_refresh();
    /*Fin de sección de pantalla lcd*/

    /*Inicio de sección de máquina de estados de eventos*/
    switch (event_id)
    {
        case EVENT_KEY_UP:
            /*
             * En toda esta sección se realiza el calculo para modificar la
             * fecha y hora del calendario del RTC. Para eso se debe tener
             * en cuenta la cantidad de días de cada mes, si es un año
             * bisiesto o no, y que al pasarse del valor máximo o mínimo de
             * cada parámetro, se vuelva a iniciar desde el valor mínimo o
             * máximo respectivamente.
             */
            if (field == FIELD_DAY)
            {
                if (date_freeze.mm == JANUARY || date_freeze.mm == MARCH
                || date_freeze.mm == MAY || date_freeze.mm == JULY
                || date_freeze.mm == AUGUST || date_freeze.mm == OCTOBER
                || date_freeze.mm == DECEMBER)
                {
                    if (date_freeze.dd < DAY_31)
                    {
                        date_freeze.dd++;
                    }
                    else
                    {
                        date_freeze.dd = DAY_1;
                    }
                }
                else if (date_freeze.mm == APRIL || date_freeze.mm == JUNE
                || date_freeze.mm == SEPTEMBER || date_freeze.mm == NOVEMBER)
                {
                    if (date_freeze.dd < DAY_30)
                    {
                        date_freeze.dd++;
                    }
                    else
                    {
                        date_freeze.dd = DAY_1;
                    }
                }
                else
                {
                    if (date_freeze.aaaa % YEAR_4 == 0)
                    {
                        if (date_freeze.dd < DAY_29)
                        {
                            date_freeze.dd++;
                        }
                        else
                        {
                            date_freeze.dd = DAY_1;
                        }
                    }
                    else
                    {
                        if (date_freeze.dd < DAY_28)
                        {
                            date_freeze.dd++;
                        }
                        else
                        {
                            date_freeze.dd = DAY_1;
                        }
                    }
                }
            }
            else if (field == FIELD_MONTH)
            {
                if (date_freeze.mm < DECEMBER)
                {
                    if ((date_freeze.mm + 1 == APRIL
                    || date_freeze.mm + 1 == JUNE
                    || date_freeze.mm + 1 == SEPTEMBER
                    || date_freeze.mm + 1 == NOVEMBER)
                    && (date_freeze.dd > DAY_30))
                    {
                        date_freeze.dd = DAY_30;
                    }
                    else if ((date_freeze.mm + 1 == FEBRUARY)
                    && (date_freeze.aaaa % YEAR_4 == 0)
                    && (date_freeze.dd > DAY_29))
                    {
                        date_freeze.dd = DAY_29;
                    }
                    else if ((date_freeze.mm + 1 == FEBRUARY)
                    && (date_freeze.aaaa % YEAR_4 != 0)
                    && (date_freeze.dd > DAY_28))
                    {
                        date_freeze.dd = DAY_28;
                    }
                    date_freeze.mm++;
                }
                else
                {
                    date_freeze.mm = JANUARY;
                }
            }
            else if (field == FIELD_YEAR)
            {
                if (date_freeze.aaaa < YEAR_99)
                {
                    if (((date_freeze.aaaa + 1) % YEAR_4 != YEAR_0)
                    && (date_freeze.mm == FEBRUARY)
                    && (date_freeze.dd > DAY_28))
                    {
                        date_freeze.dd = DAY_28;
                    }
                    date_freeze.aaaa++;
                }
                else
                {
                    date_freeze.aaaa = YEAR_0;
                }
            }
            else if (field == FIELD_HOUR)
            {
                if (time_freeze.hh < HOUR_23)
                {
                    time_freeze.hh++;
                }
                else
                {
                    time_freeze.hh = HOUR_0;
                }
            }
            else if (field == FIELD_MINUTE)
            {
                if (time_freeze.mm < MINU_59)
                {
                    time_freeze.mm++;
                }
                else
                {
                    time_freeze.mm = MINU_0;
                }
            }
            else if (field == FIELD_SECOND)
            {
                if (time_freeze.ss < SEC_59)
                {
                    time_freeze.ss++;
                }
                else
                {
                    time_freeze.ss = SEC_0;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);

        break;
        case EVENT_KEY_DOWN:
            /*
             * En toda esta sección se realiza el calculo para modificar la
             * fecha y hora del calendario del RTC. Para eso se debe tener
             * en cuenta la cantidad de días de cada mes, si es un año
             * bisiesto o no, y que al pasarse del valor máximo o mínimo de
             * cada parámetro, se vuelva a iniciar desde el valor mínimo o
             * máximo respectivamente.
             */
            if (field == FIELD_DAY)
            {
                if (date_freeze.mm == JANUARY || date_freeze.mm == MARCH
                || date_freeze.mm == MAY || date_freeze.mm == JULY
                || date_freeze.mm == AUGUST || date_freeze.mm == OCTOBER
                || date_freeze.mm == DECEMBER)
                {
                    if (date_freeze.dd > DAY_1)
                    {
                        date_freeze.dd--;
                    }
                    else
                    {
                        date_freeze.dd = DAY_31;
                    }
                }
                else if (date_freeze.mm == APRIL || date_freeze.mm == JUNE
                || date_freeze.mm == SEPTEMBER || date_freeze.mm == NOVEMBER)
                {
                    if (date_freeze.dd > DAY_1)
                    {
                        date_freeze.dd--;
                    }
                    else
                    {
                        date_freeze.dd = DAY_30;
                    }
                }
                else
                {
                    if (date_freeze.aaaa % YEAR_4 == 0)
                    {
                        if (date_freeze.dd > DAY_1)
                        {
                            date_freeze.dd--;
                        }
                        else
                        {
                            date_freeze.dd = DAY_29;
                        }
                    }
                    else
                    {
                        if (date_freeze.dd > DAY_1)
                        {
                            date_freeze.dd--;
                        }
                        else
                        {
                            date_freeze.dd = DAY_28;
                        }
                    }
                }
            }
            else if (field == FIELD_MONTH)
            {
                if (date_freeze.mm > JANUARY)
                {
                    if ((date_freeze.mm - 1 == APRIL
                    || date_freeze.mm - 1 == JUNE
                    || date_freeze.mm - 1 == SEPTEMBER
                    || date_freeze.mm - 1 == NOVEMBER)
                    && (date_freeze.dd > DAY_30))
                    {
                        date_freeze.dd = DAY_30;
                    }
                    else if ((date_freeze.mm - 1 == FEBRUARY)
                    && (date_freeze.aaaa % YEAR_4 == 0)
                    && (date_freeze.dd > DAY_29))
                    {
                        date_freeze.dd = DAY_29;
                    }
                    else if ((date_freeze.mm - 1 == FEBRUARY)
                    && (date_freeze.aaaa % YEAR_4 != 0)
                    && (date_freeze.dd > DAY_28))
                    {
                        date_freeze.dd = DAY_28;
                    }
                    date_freeze.mm--;
                }
                else
                {
                    date_freeze.mm = DECEMBER;
                }
            }
            else if (field == FIELD_YEAR)
            {
                if (date_freeze.aaaa > YEAR_0)
                {
                    if (((date_freeze.aaaa - 1) % YEAR_4 != YEAR_0)
                    && (date_freeze.mm == FEBRUARY)
                    && (date_freeze.dd > DAY_28))
                    {
                        date_freeze.dd = DAY_28;
                    }
                    date_freeze.aaaa--;
                }
                else
                {
                    date_freeze.aaaa = YEAR_99;
                }
            }
            else if (field == FIELD_HOUR)
            {
                if (time_freeze.hh > HOUR_0)
                {
                    time_freeze.hh--;
                }
                else
                {
                    time_freeze.hh = HOUR_23;
                }
            }
            else if (field == FIELD_MINUTE)
            {
                if (time_freeze.mm > MINU_0)
                {
                    time_freeze.mm--;
                }
                else
                {
                    time_freeze.mm = MINU_59;
                }
            }
            else if (field == FIELD_SECOND)
            {
                if (time_freeze.ss > SEC_0)
                {
                    time_freeze.ss--;
                }
                else
                {
                    time_freeze.ss = SEC_59;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            /*
             * Se cambia qué parámetro se va a modificar según cual estaba
             * seleccionado para modificarse actualmente.
             */
            if (field == FIELD_DAY)
            {
                field = FIELD_MONTH;
            }
            else if (field == FIELD_MONTH)
            {
                field = FIELD_YEAR;
            }
            else if (field == FIELD_YEAR)
            {
                field = FIELD_HOUR;
            }
            else if (field == FIELD_HOUR)
            {
                field = FIELD_MINUTE;
            }
            else if (field == FIELD_MINUTE)
            {
                field = FIELD_SECOND;
            }
            else if (field == FIELD_SECOND)
            {
                field = FIELD_DAY;
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
    /*Fin de sección de máquina de estados de eventos*/

    /*Inicio de sección de debugging por UART*/
#ifdef FM_DEBUG_UART_TX_MENU_ENTER
        char msg_buffer[] = "Configurar fecha y hora\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif
    /*Fin de sección de debugging por UART*/

    /*Inicio de salida del menú actual*/
    if (new_exit == 1)
    {
        /*
         * Es totalmente necesario que cuando se complete las estructuras
         * del calendario del RTC, al intentar escribir la fecha, se
         * inicialicen todas las variables de dicha estructura (.Date,
         * .Month, .Year, .Weekday) a pesar de que alguna sea irrelevante
         * (como en este caso lo es .WeekDay), ya que de lo contrario, se
         * deja a la suerte la escritura de los registros del periférico y
         * puede que al intentar leer nuevamente el calendario, los datos se
         * encuentren corrompidos y se lea cualquier cosa. Para mas info de
         * esto ver el link siguiente:
         *
         * http://www.efton.sk/STM32/gotcha/g113.html
         */
        date_final.Date = date_freeze.dd;
        date_final.Month = date_freeze.mm;
        date_final.Year = date_freeze.aaaa;
        date_final.WeekDay = RTC_WEEKDAY_FRIDAY;

        HAL_RTC_SetDate(&hrtc, &date_final, RTC_FORMAT_BIN);

        /*
         * Idem que lo de la fecha escrito arriba, pero para la hora, y en
         * general siempre que se use la HAL completando estructuras.
         */
        time_final.Hours = time_freeze.hh;
        time_final.Minutes = time_freeze.mm;
        time_final.Seconds = time_freeze.ss;
        time_final.SecondFraction = 0;
        time_final.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        time_final.StoreOperation = RTC_STOREOPERATION_RESET;
        time_final.SubSeconds = 0;
        time_final.TimeFormat = RTC_HOURFORMAT12_AM;

        HAL_RTC_SetTime(&hrtc, &time_final, RTC_FORMAT_BIN);

        correct_password = 0;

        new_entry = 1;
        new_exit = 0;
    }
    /*Fin de salida del menú actual*/

    return (ret_menu);
}

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
    /*Inicio de sección de variables locales estáticas*/
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static uint8_t password_try = 0;
    static const uint8_t password_try_max = 3;
    static const uint8_t password_length = 4;

    static uint8_t password[4] = // @suppress("Avoid magic numbers")
    {
        0,
        0,
        0,
        0
    };
    static uint8_t password_index = 0;
    /*Fin de sección de variables locales estáticas*/

    /*Inicio de sección de otras variables no estáticas*/
    /*Fin de sección de otras variables no estáticas*/

    /*Inicio de sección de variables de punteros a función y eventos*/
    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
    fm_event_t event_now;
    /*Fin de sección de variables de punteros a función y eventos*/

    /*Inicio de sección de primer entrada al menú*/
    if (new_entry == 1)
    {
        in_configuration = 1;
        correct_password = 0;
        fm_lcd_clear();
        new_entry = 0;
    }
    /*Fin de sección de primer entrada al menú*/

    /*Inicio de sección de pantalla lcd*/
    fm_lcd_flowmeter_password(password_index);
    fm_lcd_refresh();
    /*Fin de sección de pantalla lcd*/

    /*Inicio de sección de máquina de estados de eventos*/
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
    /*Fin de sección de máquina de estados de eventos*/

    /*Inicio de sección de debugging por UART*/
#ifdef FM_DEBUG_UART_TX_MENU_ENTER
        char msg_buffer[] = "Password\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif
    /*Fin de sección de debugging por UART*/

    /*Inicio de salida del menú actual*/
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
    /*Fin de salida del menú actual*/

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
    /*Inicio de sección de variables locales estáticas*/
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    /*Fin de sección de variables locales estáticas*/

    /*Inicio de sección de otras variables no estáticas*/

    /*Fin de sección de otras variables no estáticas*/

    /*Inicio de sección de variables de punteros a función y eventos*/
    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_units_tim;
    fm_event_t event_now;
    /*Fin de sección de variables de punteros a función y eventos*/

    /*Inicio de sección de primer entrada al menú*/
    if (new_entry == 1)
    {
        if (correct_password == 0)
        {
            ret_menu = (ptr_ret_menu_t) fm_menu_show_init;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);

            return (ret_menu);
        }

        fm_lcd_clear();
        new_entry = 0;
    }
    /*Fin de sección de primer entrada al menú*/

    /*Inicio de sección de pantalla lcd*/
    actual_event = event_id;
    fm_lcd_flowmeter_units_tim();
    fm_lcd_refresh();
    /*Fin de sección de pantalla lcd*/

    /*Inicio de sección de máquina de estados de eventos*/
    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (strncmp(fm_factory_get_rate().unit_time, "H", 1) == 0)
            {
                fm_factory_modify_units_time("D");
            }
            else if (strncmp(fm_factory_get_rate().unit_time, "D", 1) == 0)
            {
                fm_factory_modify_units_time("S");
            }
            else if (strncmp(fm_factory_get_rate().unit_time, "S", 1) == 0)
            {
                fm_factory_modify_units_time("M");
            }
            else if (strncmp(fm_factory_get_rate().unit_time, "M", 1) == 0)
            {
                fm_factory_modify_units_time("H");
            }

            fm_lcd_clear();
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (strncmp(fm_factory_get_rate().unit_time, "H", 1) == 0)
            {
                fm_factory_modify_units_time("M");
            }
            else if (strncmp(fm_factory_get_rate().unit_time, "M", 1) == 0)
            {
                fm_factory_modify_units_time("S");
            }
            else if (strncmp(fm_factory_get_rate().unit_time, "S", 1) == 0)
            {
                fm_factory_modify_units_time("D");
            }
            else if (strncmp(fm_factory_get_rate().unit_time, "D", 1) == 0)
            {
                fm_factory_modify_units_time("H");
            }

            fm_lcd_clear();
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
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

            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_date_hour;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    /*Fin de sección de máquina de estados de eventos*/

    /*Inicio de sección de debugging por UART*/
#ifdef FM_DEBUG_UART_TX_MENU_ENTER
        char msg_buffer[] = "Configurar unidades de tiempo y resolucion\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif
    /*Fin de sección de debugging por UART*/

    /*Inicio de salida del menú actual*/
    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }
    /*Fin de salida del menú actual*/

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
    /*Inicio de sección de variables locales estáticas*/
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    /*Fin de sección de variables locales estáticas*/

    /*Inicio de sección de otras variables no estáticas*/

    /*Fin de sección de otras variables no estáticas*/

    /*Inicio de sección de variables de punteros a función y eventos*/
    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_units_vol;
    fm_event_t event_now;
    /*Fin de sección de variables de punteros a función y eventos*/

    /*Inicio de sección de primer entrada al menú*/
    if (new_entry == 1)
    {
        if (correct_password == 0)
        {
            ret_menu = (ptr_ret_menu_t) fm_menu_show_init;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);

            return (ret_menu);
        }
        fm_lcd_clear();
        new_entry = 0;
    }
    /*Fin de sección de primer entrada al menú*/

    /*Inicio de sección de pantalla lcd*/
    actual_event = event_id;
    fm_lcd_flowmeter_units_vol();
    fm_lcd_refresh();
    /*Fin de sección de pantalla lcd*/

    /*Inicio de sección de máquina de estados de eventos*/
    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (strncmp(fm_factory_get_acm().unit_volume, "LT", 2) == 0)
            {
                fm_factory_modify_units_volume("M3");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "M3", 2) == 0)
            {
                fm_factory_modify_units_volume("MC");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "MC", 2) == 0)
            {
                fm_factory_modify_units_volume("KG");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "KG", 2) == 0)
            {
                fm_factory_modify_units_volume("GL");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "GL", 2) == 0)
            {
                fm_factory_modify_units_volume("BR");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "BR", 2) == 0)
            {
                fm_factory_modify_units_volume("--");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "--", 2) == 0)
            {
                fm_factory_modify_units_volume("LT");
            }

            fm_lcd_clear();
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (strncmp(fm_factory_get_acm().unit_volume, "LT", 2) == 0)
            {
                fm_factory_modify_units_volume("--");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "--", 2) == 0)
            {
                fm_factory_modify_units_volume("BR");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "BR", 2) == 0)
            {
                fm_factory_modify_units_volume("GL");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "GL", 2) == 0)
            {
                fm_factory_modify_units_volume("KG");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "KG", 2) == 0)
            {
                fm_factory_modify_units_volume("MC");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "MC", 2) == 0)
            {
                fm_factory_modify_units_volume("M3");
            }
            else if (strncmp(fm_factory_get_acm().unit_volume, "M3", 2) == 0)
            {
                fm_factory_modify_units_volume("LT");
            }

            fm_lcd_clear();
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
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
    /*Fin de sección de máquina de estados de eventos*/

    /*Inicio de sección de debugging por UART*/
#ifdef FM_DEBUG_UART_TX_MENU_ENTER
        char msg_buffer[] = "Configurar unidades de volumen y resolucion\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif
    /*Fin de sección de debugging por UART*/

    /*Inicio de salida del menú actual*/
    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }
    /*Fin de salida del menú actual*/

    return (ret_menu);
}

// Interrupts

/*** end of file ***/
