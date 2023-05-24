/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

typedef struct
{
    uint8_t dia;
    uint8_t mes;
    uint8_t año;
} date_rtc_t;

typedef struct
{
    uint8_t hora;
    uint8_t minutos;
    uint8_t segundos;
} time_rtc_t;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PCF8553_SCL_Pin GPIO_PIN_1
#define PCF8553_SCL_GPIO_Port GPIOA
#define PCF8553_CE_Pin GPIO_PIN_4
#define PCF8553_CE_GPIO_Port GPIOA
#define PCF8553_RESET_Pin GPIO_PIN_6
#define PCF8553_RESET_GPIO_Port GPIOA
#define PCF8553_SDIO_Pin GPIO_PIN_7
#define PCF8553_SDIO_GPIO_Port GPIOA
#define led_red_Pin GPIO_PIN_2
#define led_red_GPIO_Port GPIOG
#define led_blue_Pin GPIO_PIN_7
#define led_blue_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
