/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define BL_PWM_POS_Pin GPIO_PIN_1
#define BL_PWM_POS_GPIO_Port GPIOA
#define T_ADC_Pin GPIO_PIN_4
#define T_ADC_GPIO_Port GPIOA
#define RT_ADC_Pin GPIO_PIN_5
#define RT_ADC_GPIO_Port GPIOA
#define AC1_ADC_Pin GPIO_PIN_6
#define AC1_ADC_GPIO_Port GPIOA
#define AC2_ADC_Pin GPIO_PIN_7
#define AC2_ADC_GPIO_Port GPIOA
#define BEMF_ADC_Pin GPIO_PIN_4
#define BEMF_ADC_GPIO_Port GPIOC
#define BOARD_24V_ADC_Pin GPIO_PIN_0
#define BOARD_24V_ADC_GPIO_Port GPIOB
#define OH_DOOR_ADC_Pin GPIO_PIN_1
#define OH_DOOR_ADC_GPIO_Port GPIOB
#define OH_DOOR_PWR_ON_NEGATIVE_POS_Pin GPIO_PIN_12
#define OH_DOOR_PWR_ON_NEGATIVE_POS_GPIO_Port GPIOF
#define OH_DOOR_PWR_ON_POSITIVE_POS_Pin GPIO_PIN_13
#define OH_DOOR_PWR_ON_POSITIVE_POS_GPIO_Port GPIOF
#define RB2_PWR_ON_NEGATIVE_POS_Pin GPIO_PIN_14
#define RB2_PWR_ON_NEGATIVE_POS_GPIO_Port GPIOF
#define RB2_PWR_ON_POSITIVE_POS_Pin GPIO_PIN_15
#define RB2_PWR_ON_POSITIVE_POS_GPIO_Port GPIOF
#define RB1_PWR_ON_NEGATIVE_POS_Pin GPIO_PIN_0
#define RB1_PWR_ON_NEGATIVE_POS_GPIO_Port GPIOG
#define RB1_PWR_ON_POSITIVE_POS_Pin GPIO_PIN_1
#define RB1_PWR_ON_POSITIVE_POS_GPIO_Port GPIOG
#define AC_FAN1_PWR_POS_Pin GPIO_PIN_8
#define AC_FAN1_PWR_POS_GPIO_Port GPIOE
#define AC_FAN2_PWR_POS_Pin GPIO_PIN_9
#define AC_FAN2_PWR_POS_GPIO_Port GPIOE
#define RICE_FAN_PWR_POS_Pin GPIO_PIN_10
#define RICE_FAN_PWR_POS_GPIO_Port GPIOE
#define UV_LAMP_PWR_POS_Pin GPIO_PIN_11
#define UV_LAMP_PWR_POS_GPIO_Port GPIOE
#define E_LAMP_PWR_POS_Pin GPIO_PIN_12
#define E_LAMP_PWR_POS_GPIO_Port GPIOE
#define RM_MOTOR_PWR_POS_Pin GPIO_PIN_13
#define RM_MOTOR_PWR_POS_GPIO_Port GPIOE
#define SCK_8711_POS_Pin GPIO_PIN_13
#define SCK_8711_POS_GPIO_Port GPIOB
#define MISO_8711_POS_Pin GPIO_PIN_14
#define MISO_8711_POS_GPIO_Port GPIOB
#define MOSI_8711_POS_Pin GPIO_PIN_15
#define MOSI_8711_POS_GPIO_Port GPIOB
#define CS_8711_POS_Pin GPIO_PIN_8
#define CS_8711_POS_GPIO_Port GPIOD
#define DIR_8711_POS_Pin GPIO_PIN_9
#define DIR_8711_POS_GPIO_Port GPIOD
#define RESET_8711_POS_Pin GPIO_PIN_10
#define RESET_8711_POS_GPIO_Port GPIOD
#define SLEEP_8711_POS_Pin GPIO_PIN_11
#define SLEEP_8711_POS_GPIO_Port GPIOD
#define FAULT_8711_POS_Pin GPIO_PIN_12
#define FAULT_8711_POS_GPIO_Port GPIOD
#define STALL_8711_POS_Pin GPIO_PIN_13
#define STALL_8711_POS_GPIO_Port GPIOD
#define RB1_1_EMPTY_POS_Pin GPIO_PIN_3
#define RB1_1_EMPTY_POS_GPIO_Port GPIOG
#define RB1_2_EMPTY_POS_Pin GPIO_PIN_4
#define RB1_2_EMPTY_POS_GPIO_Port GPIOG
#define OH_DOOR_CLEAR_POS_Pin GPIO_PIN_5
#define OH_DOOR_CLEAR_POS_GPIO_Port GPIOG
#define RICE_TAKE_AWAY_POS_Pin GPIO_PIN_6
#define RICE_TAKE_AWAY_POS_GPIO_Port GPIOG
#define STEP_8711_POS_Pin GPIO_PIN_6
#define STEP_8711_POS_GPIO_Port GPIOC
#define RB1_1_TURN_ON_POS_Pin GPIO_PIN_0
#define RB1_1_TURN_ON_POS_GPIO_Port GPIOD
#define RB1_TURN_OFF_POS_Pin GPIO_PIN_1
#define RB1_TURN_OFF_POS_GPIO_Port GPIOD
#define RB1_2_TURN_ON_POS_Pin GPIO_PIN_2
#define RB1_2_TURN_ON_POS_GPIO_Port GPIOD
#define RB2_TURN_ON_POS_Pin GPIO_PIN_3
#define RB2_TURN_ON_POS_GPIO_Port GPIOD
#define RB2_TURN_OFF_POS_Pin GPIO_PIN_4
#define RB2_TURN_OFF_POS_GPIO_Port GPIOD
#define OH_DOOR_TURN_ON_POS_Pin GPIO_PIN_5
#define OH_DOOR_TURN_ON_POS_GPIO_Port GPIOD
#define OH_DOOR_TURN_OFF_POS_Pin GPIO_PIN_6
#define OH_DOOR_TURN_OFF_POS_GPIO_Port GPIOD
#define RL_RESET_POS_Pin GPIO_PIN_7
#define RL_RESET_POS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
