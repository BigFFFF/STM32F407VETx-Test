/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum
{
    KEY_CHECK,  	//按键�????测状�????
    KEY_COMFIRM,    //按键确认状�??
    KEY_RELEASE     //按键释放状�??
}keyState_e;        //状�?�枚举变�????

typedef struct
{
  keyState_e keyState;	//按键状�??
  bool keyFlag;			//按键按下标志
  uint16_t keyPin;		//按键引脚
}KEY_STATE_T;           //按键状�?�结构体
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
#define KEY_1_Pin GPIO_PIN_10
#define KEY_1_GPIO_Port GPIOE
#define KEY_2_Pin GPIO_PIN_11
#define KEY_2_GPIO_Port GPIOE
#define KEY_3_Pin GPIO_PIN_12
#define KEY_3_GPIO_Port GPIOE
#define LED_1_Pin GPIO_PIN_13
#define LED_1_GPIO_Port GPIOE
#define LED_2_Pin GPIO_PIN_14
#define LED_2_GPIO_Port GPIOE
#define LED_3_Pin GPIO_PIN_15
#define LED_3_GPIO_Port GPIOE
#define RS485_DE_Pin GPIO_PIN_7
#define RS485_DE_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */
#define KEY_GPIO_Port GPIOE
#define RS485_TX_EN			HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET)
#define RS485_RX_EN			HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
