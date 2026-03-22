/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ROW2_Pin GPIO_PIN_0
#define ROW2_GPIO_Port GPIOC
#define ROW1_Pin GPIO_PIN_1
#define ROW1_GPIO_Port GPIOC
#define ROW4_Pin GPIO_PIN_2
#define ROW4_GPIO_Port GPIOC
#define ROW3_Pin GPIO_PIN_3
#define ROW3_GPIO_Port GPIOC
#define LIGHT_SENSOR_Pin GPIO_PIN_0
#define LIGHT_SENSOR_GPIO_Port GPIOA
#define TEMP_SENSOR_Pin GPIO_PIN_1
#define TEMP_SENSOR_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define HUMIDITY_SENSOR_Pin GPIO_PIN_4
#define HUMIDITY_SENSOR_GPIO_Port GPIOA
#define DISPLAY_D5_Pin GPIO_PIN_10
#define DISPLAY_D5_GPIO_Port GPIOB
#define ACTUATOR_LIGHT_PANEL_Pin GPIO_PIN_6
#define ACTUATOR_LIGHT_PANEL_GPIO_Port GPIOC
#define DISPLAY_RS_Pin GPIO_PIN_7
#define DISPLAY_RS_GPIO_Port GPIOC
#define ACTUATOR_PUMP_Pin GPIO_PIN_8
#define ACTUATOR_PUMP_GPIO_Port GPIOC
#define ACTUATOR_FAN_Pin GPIO_PIN_9
#define ACTUATOR_FAN_GPIO_Port GPIOC
#define DISPLAY_D4_Pin GPIO_PIN_8
#define DISPLAY_D4_GPIO_Port GPIOA
#define DISPLAY_E_Pin GPIO_PIN_9
#define DISPLAY_E_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define COL2_Pin GPIO_PIN_10
#define COL2_GPIO_Port GPIOC
#define COL3_Pin GPIO_PIN_11
#define COL3_GPIO_Port GPIOC
#define COL1_Pin GPIO_PIN_12
#define COL1_GPIO_Port GPIOC
#define COL4_Pin GPIO_PIN_2
#define COL4_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define DISPLAY_D6_Pin GPIO_PIN_4
#define DISPLAY_D6_GPIO_Port GPIOB
#define DISPLAY_D7_Pin GPIO_PIN_5
#define DISPLAY_D7_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
