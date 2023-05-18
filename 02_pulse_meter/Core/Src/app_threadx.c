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
#include "stdio.h"
#include "string.h"
#include "../../frecuency_meter/frecuency_meter.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FM_THREADX_LOW_POWER
#define THREAD_STACK_SIZE 1024

/*
 * Tamaño de memoria para ser usado por TraceX. no cualquier tamaño funciona
 * creo que tiene que ser multiplo de 10.
 */
#define TRACEX_BUFFER_SIZE 16000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

uint8_t pulse_meter_thread_stack[THREAD_STACK_SIZE];
TX_THREAD pulse_meter_thread_ptr;

// Si no esta activado el modo de bajo consumo activo TraceX
#ifndef FM_THREADX_LOW_POWER
uint8_t tracex_buffer[TRACEX_BUFFER_SIZE] __attribute__((section (".trace")));
#endif

extern LPTIM_HandleTypeDef hlptim3;
extern LPTIM_HandleTypeDef hlptim4;
extern RTC_HandleTypeDef hrtc;

TX_THREAD thread_ptr;
uint16_t g_lptim1_start;
uint16_t g_lptim1_end;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

VOID pulse_meter_thread_entry(ULONG initial_input);

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

  tx_thread_create(&pulse_meter_thread_ptr, "pulse_meter_thread_name",
  pulse_meter_thread_entry, 0x1234, pulse_meter_thread_stack, THREAD_STACK_SIZE,
  15, 15, 1, TX_AUTO_START);

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
  * @brief  app_threadx_lowpower_timer_setup
  * @param  count : TX timer count
  * @retval None
  */
void app_threadx_lowpower_timer_setup(ULONG count)
{
  /* USER CODE BEGIN  app_threadx_lowpower_timer_setup */

    #ifdef FM_THREADX_LOW_POWER

        uint32_t ticks_to_sleep;
        g_lptim1_start = LPTIM1->CNT;
        ticks_to_sleep = (32768 / 16) * count; // clock_freq /  RTC_WAKEUPCLOCK_RTCCLK_DIV16
        ticks_to_sleep /= TX_TIMER_TICKS_PER_SECOND;

    #ifdef FM_DEBUG_UART_TX_TIME_ON_IDLE
    //  fm_debug_uint32_uart(count);
    #endif

    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, ticks_to_sleep,
    RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
    {
        Error_Handler();
    }

    #endif

  /* USER CODE END  app_threadx_lowpower_timer_setup */
}

/**
  * @brief  app_threadx_lowpower_enter
  * @param  None
  * @retval None
  */
void app_threadx_lowpower_enter(void)
{
  /* USER CODE BEGIN  app_threadx_lowpower_enter */

    HAL_GPIO_WritePin(led_blue_GPIO_Port, led_blue_Pin, GPIO_PIN_RESET);

    #ifdef FM_THREADX_LOW_POWER
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    #endif

  /* USER CODE END  app_threadx_lowpower_enter */
}

/**
  * @brief  app_threadx_lowpower_exit
  * @param  None
  * @retval None
  */
void app_threadx_lowpower_exit(void)
{
  /* USER CODE BEGIN  app_threadx_lowpower_exit */

    /*
     * If CPU wakes up other reason but timer flag we must wait until
     * expected time was elapsed. Debugger issues makes wake up CPU earlier
     */
     #ifdef FM_THREADX_LOW_POWER
         call_system_clk_config();
     #endif

     HAL_GPIO_WritePin(led_blue_GPIO_Port, led_blue_Pin, GPIO_PIN_SET);

  /* USER CODE END  app_threadx_lowpower_exit */
}

/**
  * @brief  app_threadx_lowpower_timer_adjust
  * @param  None
  * @retval Amount of time (in ticks)
  */
ULONG app_threadx_lowpower_timer_adjust(void)
{
  /* USER CODE BEGIN  app_threadx_lowpower_timer_adjust */
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
  /* USER CODE END  app_threadx_lowpower_timer_adjust */
}

/* USER CODE BEGIN 1 */

VOID pulse_meter_thread_entry(ULONG initial_input)
{
    static uint16_t pulse_counter_begin = 0;
    static uint16_t pulse_counter_end = 0;
    static uint16_t pulse_counter_diff = 0;

    static char pulse_counter_diff_msg[40];

    HAL_LPTIM_Counter_Start(&hlptim4);
    HAL_LPTIM_Counter_Start(&hlptim3);

    while(1)
    {
        pulse_counter_begin = pulse_counter_end;

        tx_thread_sleep(100);

        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
        HAL_NVIC_EnableIRQ(EXTI14_IRQn);

        pulse_counter_end = HAL_LPTIM_ReadCounter(&hlptim4);
        pulse_counter_diff = pulse_counter_end - pulse_counter_begin;
    }
}

/* USER CODE END 1 */
