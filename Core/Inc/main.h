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
#define BOTONERA_COL1_Pin GPIO_PIN_0
#define BOTONERA_COL1_GPIO_Port GPIOC
#define BOTONERA_COL2_Pin GPIO_PIN_1
#define BOTONERA_COL2_GPIO_Port GPIOC
#define BOTONERA_COL4_Pin GPIO_PIN_2
#define BOTONERA_COL4_GPIO_Port GPIOC
#define BOTONERA_COL3_Pin GPIO_PIN_3
#define BOTONERA_COL3_GPIO_Port GPIOC
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
#define D6_Pin GPIO_PIN_10
#define D6_GPIO_Port GPIOB
#define D_EN_Pin GPIO_PIN_7
#define D_EN_GPIO_Port GPIOC
#define D7_Pin GPIO_PIN_8
#define D7_GPIO_Port GPIOA
#define D_RS_Pin GPIO_PIN_9
#define D_RS_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define BOTONERA_FILA2_Pin GPIO_PIN_10
#define BOTONERA_FILA2_GPIO_Port GPIOC
#define BOTONERA_FILA3_Pin GPIO_PIN_11
#define BOTONERA_FILA3_GPIO_Port GPIOC
#define BOTONERA_FILA1_Pin GPIO_PIN_12
#define BOTONERA_FILA1_GPIO_Port GPIOC
#define BOTONERA_FILA4_Pin GPIO_PIN_2
#define BOTONERA_FILA4_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_4
#define D5_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_5
#define D4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
