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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../User/Inc/my_time.h"
#include "../User/Inc/my_flash.h"
#include "../User/Inc/my_globe.h"
#include "../User/Inc/my_key.h"
#include "../User/Inc/my_vl6180x.h"
#include "../User/Inc/my_fdcan.h"
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
#define _3_3col1_Pin GPIO_PIN_0
#define _3_3col1_GPIO_Port GPIOA
#define _3_3col2_Pin GPIO_PIN_1
#define _3_3col2_GPIO_Port GPIOA
#define _3_3col3_Pin GPIO_PIN_2
#define _3_3col3_GPIO_Port GPIOA
#define _3_3row1_Pin GPIO_PIN_3
#define _3_3row1_GPIO_Port GPIOA
#define _3_3row2_Pin GPIO_PIN_4
#define _3_3row2_GPIO_Port GPIOA
#define _3_3row3_Pin GPIO_PIN_5
#define _3_3row3_GPIO_Port GPIOA
#define ADC_Gripper_Stroke_Pin GPIO_PIN_6
#define ADC_Gripper_Stroke_GPIO_Port GPIOA
#define ADC_7axis_Rotation_Pin GPIO_PIN_7
#define ADC_7axis_Rotation_GPIO_Port GPIOA
#define Enable_Switch_Pin GPIO_PIN_0
#define Enable_Switch_GPIO_Port GPIOB
#define Toggle_Switch_Up_Pin GPIO_PIN_8
#define Toggle_Switch_Up_GPIO_Port GPIOA
#define Toggle_Switch_Mid_Pin GPIO_PIN_9
#define Toggle_Switch_Mid_GPIO_Port GPIOA
#define Toggle_Switch_Down_Pin GPIO_PIN_10
#define Toggle_Switch_Down_GPIO_Port GPIOA
#define Vl6180x_Enable_Pin GPIO_PIN_3
#define Vl6180x_Enable_GPIO_Port GPIOB
#define Led_Red_Pin GPIO_PIN_4
#define Led_Red_GPIO_Port GPIOB
#define Led_Green_Pin GPIO_PIN_5
#define Led_Green_GPIO_Port GPIOB
#define Led_White_Pin GPIO_PIN_6
#define Led_White_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
