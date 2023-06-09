/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "string.h"
#include "../../../libs/fm_keypad/fm_keypad.h"
#include "../../../libs/fm_menu_user/fm_menu_user.h"
#include "../../../libs/fm_menu_config/fm_menu_config.h"
//#include "../../../libs/fm_debug.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FM_THREADX_LOW_POWER

#define THREAD_STACK_SIZE 1024

#define QUEUE_STACK_SIZE 1024

/*
 * Tamaño de memoria para ser usado por Tracex. no cualquier tamaño fuciona
 * creo tiene que ser multiplo de 10.
 */
#define TRACEX_BUFFER_SIZE 16000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

extern RTC_HandleTypeDef hrtc;

uint16_t g_lptim1_start;
uint16_t g_lptim1_end;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

uint8_t menu_task_stack[THREAD_STACK_SIZE];
uint8_t debounce_task_stack[THREAD_STACK_SIZE];

uint8_t event_queue_stack[QUEUE_STACK_SIZE];

TX_THREAD menu_task_ptr;
TX_THREAD debounce_task_ptr;
TX_SEMAPHORE debounce_semaphore_ptr;
TX_QUEUE event_queue_ptr;

#ifndef FM_THREADX_LOW_POWER
//  uint8_t tracex_buffer[TRACEX_BUFFER_SIZE] __attribute__((section (".trace")));
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

VOID menu_task_entry (ULONG initial_input);
VOID debounce_task_entry (ULONG initial_input);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */
  tx_thread_create(&menu_task_ptr, "menu_task", menu_task_entry, 0x1234,
                  menu_task_stack, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);

  tx_thread_create(&debounce_task_ptr, "debounce_task", debounce_task_entry,
                  0x1234, debounce_task_stack, THREAD_STACK_SIZE, 14, 14, 1,
                  TX_AUTO_START);

  tx_semaphore_create(&debounce_semaphore_ptr, "debounce_semaphore", 0);

  tx_queue_create(&event_queue_ptr, "event_queue", sizeof(fm_event_t),
  event_queue_stack, 1024);

  #ifndef FM_THREADX_LOW_POWER
//      tx_trace_enable(&tracex_buffer, TRACEX_BUFFER_SIZE, 30);
  #endif
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/**
  * @brief  App_ThreadX_LowPower_Timer_Setup
  * @param  count : TX timer count
  * @retval None
  */
void App_ThreadX_LowPower_Timer_Setup(ULONG count)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Timer_Setup */
#ifdef FM_THREADX_LOW_POWER
  uint32_t ticks_to_sleep;

  g_lptim1_start = LPTIM1->CNT;
  /*
   * clock_freq/RTC_WAKEUPCLOCK_RTCCLK_DIV16
   */
  ticks_to_sleep = (32768 / 16) * count;
  ticks_to_sleep /= TX_TIMER_TICKS_PER_SECOND;


#ifdef FM_DEBUG_UART_TX_TIME_ON_IDLE
  fm_debug_uint32_uart(count);
#endif

  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, ticks_to_sleep,
      RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
  {
    Error_Handler();
  }

#endif
  /* USER CODE END  App_ThreadX_LowPower_Timer_Setup */
}

/**
  * @brief  App_ThreadX_LowPower_Enter
  * @param  None
  * @retval None
  */
void App_ThreadX_LowPower_Enter(void)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Enter */

  HAL_GPIO_WritePin(led_blue_GPIO_Port, led_blue_Pin, GPIO_PIN_RESET);


  #ifdef FM_THREADX_LOW_POWER
  HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
  #endif

  /* USER CODE END  App_ThreadX_LowPower_Enter */
}

/**
  * @brief  App_ThreadX_LowPower_Exit
  * @param  None
  * @retval None
  */
void App_ThreadX_LowPower_Exit(void)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Exit */

  /*
     * If CPU wakes up other reason but timer flag we must wait until
     * expected time was elapsed. Debugger issues makes wake up CPU earlier
     */
  #ifdef FM_THREADX_LOW_POWER
    SystemClock_Config();
  #endif

  HAL_GPIO_WritePin(led_blue_GPIO_Port, led_blue_Pin, GPIO_PIN_SET);

  /* USER CODE END  App_ThreadX_LowPower_Exit */
}

/**
  * @brief  App_ThreadX_LowPower_Timer_Adjust
  * @param  None
  * @retval Amount of time (in ticks)
  */
ULONG App_ThreadX_LowPower_Timer_Adjust(void)
{
  /* USER CODE BEGIN  App_ThreadX_LowPower_Timer_Adjust */
#ifdef FM_THREADX_LOW_POWER
  static uint16_t cnt_drift = 0;
  ULONG cnt_ret;
  g_lptim1_end = LPTIM1->CNT;
  cnt_ret = (g_lptim1_end - g_lptim1_start);
  cnt_ret *= TX_TIMER_TICKS_PER_SECOND;
  cnt_ret += cnt_drift;
  cnt_drift = cnt_ret % 2048; // 2048 = lptim_clok_frq /  clock_prescaler
  cnt_ret /= 2048;
  return (cnt_ret);
#else
  return (0);
#endif
  /* USER CODE END  App_ThreadX_LowPower_Timer_Adjust */
}

/* USER CODE BEGIN 1 */
VOID menu_task_entry(ULONG initial_input)
{
    static int backlight_cd = 0;
//    static int pulse_counter = 0;
    ptr_fun_menu_t ptr_menu = fm_menu_show_init;
    fm_event_t event_next = EVENT_LCD_REFRESH;
    UINT ret_status;
    while(1)
    {
//        HAL_LPTIM_Counter_Start(&hlptim4);
        ret_status = tx_queue_receive(&event_queue_ptr,&event_next,100);

        if(ptr_menu != fm_menu_show_init && ptr_menu != fm_menu_show_version)
        {
            if(event_next == EVENT_LCD_REFRESH && backlight_cd > 0)
            {
                backlight_cd--;
            }
            else if(event_next == EVENT_LCD_REFRESH && backlight_cd <= 0)
            {
                HAL_GPIO_WritePin(PCF8553_BACKLIGHT_GPIO_Port,
                PCF8553_BACKLIGHT_Pin, GPIO_PIN_SET);
                backlight_cd = 0;
            }
            else if(event_next != EVENT_LCD_REFRESH)
            {
                HAL_GPIO_WritePin(PCF8553_BACKLIGHT_GPIO_Port,
                PCF8553_BACKLIGHT_Pin, GPIO_PIN_RESET);
                backlight_cd = 10;
            }
        }

        if(ret_status == TX_SUCCESS)
        {
            ptr_menu = (ptr_fun_menu_t)(*ptr_menu)(event_next);
        }
        else
        {
            ptr_menu = (ptr_fun_menu_t)(*ptr_menu)(EVENT_LCD_REFRESH);
        }

//        HAL_LPTIM_Counter_Stop(&hlptim4);

//        pulse_counter = HAL_LPTIM_ReadCounter(&hlptim4);
    }
}

VOID debounce_task_entry(ULONG initial_input)
{
    /*
     * debounce_time*10ms se espera para evitar rebotes.
     */
    const int debounce_time = 14;

    while(1)
    {
        tx_semaphore_get(&debounce_semaphore_ptr,TX_WAIT_FOREVER);

        tx_thread_sleep(debounce_time);

        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);

        HAL_NVIC_EnableIRQ(EXTI10_IRQn);
        HAL_NVIC_EnableIRQ(EXTI11_IRQn);
        HAL_NVIC_EnableIRQ(EXTI12_IRQn);
        HAL_NVIC_EnableIRQ(EXTI13_IRQn);
    }
}
/* USER CODE END 1 */
