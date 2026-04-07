/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define NA_ExtSPI4_SCK_Pin GPIO_PIN_2
#define NA_ExtSPI4_SCK_GPIO_Port GPIOE
#define NA_ExtSPI4_NSS_Pin GPIO_PIN_4
#define NA_ExtSPI4_NSS_GPIO_Port GPIOE
#define NA_ExtSPI4_MISO_Pin GPIO_PIN_5
#define NA_ExtSPI4_MISO_GPIO_Port GPIOE
#define NA_ExtSPI4_MOSI_Pin GPIO_PIN_6
#define NA_ExtSPI4_MOSI_GPIO_Port GPIOE
#define L5_PI8_Pin GPIO_PIN_8
#define L5_PI8_GPIO_Port GPIOI
#define L4_PC13_Pin GPIO_PIN_13
#define L4_PC13_GPIO_Port GPIOC
#define L6_PI9_Pin GPIO_PIN_9
#define L6_PI9_GPIO_Port GPIOI
#define L7_PI10_Pin GPIO_PIN_10
#define L7_PI10_GPIO_Port GPIOI
#define L8_PI11_Pin GPIO_PIN_11
#define L8_PI11_GPIO_Port GPIOI
#define SW2_REF_Pin GPIO_PIN_0
#define SW2_REF_GPIO_Port GPIOF
#define SW3_REF_Pin GPIO_PIN_1
#define SW3_REF_GPIO_Port GPIOF
#define SA_ExtSPI5_NSS_Pin GPIO_PIN_6
#define SA_ExtSPI5_NSS_GPIO_Port GPIOF
#define SA_ExtSPI5_SCK_Pin GPIO_PIN_7
#define SA_ExtSPI5_SCK_GPIO_Port GPIOF
#define SA_ExtSPI5_MISO_Pin GPIO_PIN_8
#define SA_ExtSPI5_MISO_GPIO_Port GPIOF
#define SA_ExtSPI5_MOSI_Pin GPIO_PIN_9
#define SA_ExtSPI5_MOSI_GPIO_Port GPIOF
#define B8_PF10_Pin GPIO_PIN_10
#define B8_PF10_GPIO_Port GPIOF
#define ULPI_RESET_in_Pin GPIO_PIN_1
#define ULPI_RESET_in_GPIO_Port GPIOC
#define NA_ADC1_IN1_Pin GPIO_PIN_1
#define NA_ADC1_IN1_GPIO_Port GPIOA
#define SA_ADC2_IN2_Pin GPIO_PIN_2
#define SA_ADC2_IN2_GPIO_Port GPIOA
#define B6_PH2_Pin GPIO_PIN_2
#define B6_PH2_GPIO_Port GPIOH
#define B7_PH3_Pin GPIO_PIN_3
#define B7_PH3_GPIO_Port GPIOH
#define E_I2C2_SCL_Pin GPIO_PIN_4
#define E_I2C2_SCL_GPIO_Port GPIOH
#define E_I2C2_SDA_Pin GPIO_PIN_5
#define E_I2C2_SDA_GPIO_Port GPIOH
#define L1_PC4_Pin GPIO_PIN_4
#define L1_PC4_GPIO_Port GPIOC
#define L2_PC5_Pin GPIO_PIN_5
#define L2_PC5_GPIO_Port GPIOC
#define B1_PF11_Pin GPIO_PIN_11
#define B1_PF11_GPIO_Port GPIOF
#define B2_PF12_Pin GPIO_PIN_12
#define B2_PF12_GPIO_Port GPIOF
#define B3_PF13_Pin GPIO_PIN_13
#define B3_PF13_GPIO_Port GPIOF
#define B4_PF14_Pin GPIO_PIN_14
#define B4_PF14_GPIO_Port GPIOF
#define B5_PF15_Pin GPIO_PIN_15
#define B5_PF15_GPIO_Port GPIOF
#define SA_ExtInt1_Pin GPIO_PIN_0
#define SA_ExtInt1_GPIO_Port GPIOG
#define SA_ExtInt1_EXTI_IRQn EXTI0_IRQn
#define SA_ExtInt2_Pin GPIO_PIN_1
#define SA_ExtInt2_GPIO_Port GPIOG
#define SA_ExtInt2_EXTI_IRQn EXTI1_IRQn
#define SA_ACK_Pin GPIO_PIN_7
#define SA_ACK_GPIO_Port GPIOE
#define NA_Dout0_Pin GPIO_PIN_8
#define NA_Dout0_GPIO_Port GPIOE
#define NA_Dout1_Pin GPIO_PIN_9
#define NA_Dout1_GPIO_Port GPIOE
#define NA_Dout2_Pin GPIO_PIN_10
#define NA_Dout2_GPIO_Port GPIOE
#define NA_Dout3_Pin GPIO_PIN_11
#define NA_Dout3_GPIO_Port GPIOE
#define NA_Dout4_Pin GPIO_PIN_12
#define NA_Dout4_GPIO_Port GPIOE
#define NA_Dout5_Pin GPIO_PIN_13
#define NA_Dout5_GPIO_Port GPIOE
#define NA_Dout6_Pin GPIO_PIN_14
#define NA_Dout6_GPIO_Port GPIOE
#define NA_Dout7_Pin GPIO_PIN_15
#define NA_Dout7_GPIO_Port GPIOE
#define NA_A0_Pin GPIO_PIN_7
#define NA_A0_GPIO_Port GPIOH
#define NA_A1_Pin GPIO_PIN_8
#define NA_A1_GPIO_Port GPIOH
#define NA_A2_Pin GPIO_PIN_9
#define NA_A2_GPIO_Port GPIOH
#define NA_A3_Pin GPIO_PIN_10
#define NA_A3_GPIO_Port GPIOH
#define NA_DECODER_iE_Pin GPIO_PIN_11
#define NA_DECODER_iE_GPIO_Port GPIOH
#define NA_SA_PH12_Pin GPIO_PIN_12
#define NA_SA_PH12_GPIO_Port GPIOH
#define SA_Dout0_Pin GPIO_PIN_8
#define SA_Dout0_GPIO_Port GPIOD
#define SA_Dout1_Pin GPIO_PIN_9
#define SA_Dout1_GPIO_Port GPIOD
#define SA_Dout2_Pin GPIO_PIN_10
#define SA_Dout2_GPIO_Port GPIOD
#define SA_Dout3_Pin GPIO_PIN_11
#define SA_Dout3_GPIO_Port GPIOD
#define SA_Dout4_Pin GPIO_PIN_12
#define SA_Dout4_GPIO_Port GPIOD
#define SA_Dout5_Pin GPIO_PIN_13
#define SA_Dout5_GPIO_Port GPIOD
#define SA_Dout6_Pin GPIO_PIN_14
#define SA_Dout6_GPIO_Port GPIOD
#define SA_Dout7_Pin GPIO_PIN_15
#define SA_Dout7_GPIO_Port GPIOD
#define SA_A0_Pin GPIO_PIN_2
#define SA_A0_GPIO_Port GPIOG
#define SA_A1_Pin GPIO_PIN_3
#define SA_A1_GPIO_Port GPIOG
#define SA_A2_Pin GPIO_PIN_4
#define SA_A2_GPIO_Port GPIOG
#define SA_A3_Pin GPIO_PIN_5
#define SA_A3_GPIO_Port GPIOG
#define SA_DECODER_iE_Pin GPIO_PIN_6
#define SA_DECODER_iE_GPIO_Port GPIOG
#define SA_SPI_1or3V_Pin GPIO_PIN_7
#define SA_SPI_1or3V_GPIO_Port GPIOG
#define SA_SPI6_NSS_Pin GPIO_PIN_8
#define SA_SPI6_NSS_GPIO_Port GPIOG
#define SA_LKDET_Pin GPIO_PIN_6
#define SA_LKDET_GPIO_Port GPIOC
#define NA_L_LKDET_Pin GPIO_PIN_7
#define NA_L_LKDET_GPIO_Port GPIOC
#define NA_S_LKDET_Pin GPIO_PIN_8
#define NA_S_LKDET_GPIO_Port GPIOC
#define ULPI_MCO2_OSC_Pin GPIO_PIN_9
#define ULPI_MCO2_OSC_GPIO_Port GPIOC
#define NA_SA_PH13_Pin GPIO_PIN_13
#define NA_SA_PH13_GPIO_Port GPIOH
#define S1_PH14_Pin GPIO_PIN_14
#define S1_PH14_GPIO_Port GPIOH
#define S2_PH15_Pin GPIO_PIN_15
#define S2_PH15_GPIO_Port GPIOH
#define NA_S_SPI2_NSS_Pin GPIO_PIN_0
#define NA_S_SPI2_NSS_GPIO_Port GPIOI
#define NA_S_SPI2_SCK_Pin GPIO_PIN_1
#define NA_S_SPI2_SCK_GPIO_Port GPIOI
#define NA_S_SPI2_MISO_Pin GPIO_PIN_2
#define NA_S_SPI2_MISO_GPIO_Port GPIOI
#define NA_S_SPI2_MOSI_Pin GPIO_PIN_3
#define NA_S_SPI2_MOSI_GPIO_Port GPIOI
#define NA_L_SPI3_NSS_Pin GPIO_PIN_15
#define NA_L_SPI3_NSS_GPIO_Port GPIOA
#define NA_L_SPI3_SCK_Pin GPIO_PIN_10
#define NA_L_SPI3_SCK_GPIO_Port GPIOC
#define NA_L_SPI3__MISO_Pin GPIO_PIN_11
#define NA_L_SPI3__MISO_GPIO_Port GPIOC
#define NA_L_SPI3_MOSI_Pin GPIO_PIN_12
#define NA_L_SPI3_MOSI_GPIO_Port GPIOC
#define NA_L_SPI_1or3V_Pin GPIO_PIN_0
#define NA_L_SPI_1or3V_GPIO_Port GPIOD
#define NA_S_SPI_1or3V_Pin GPIO_PIN_1
#define NA_S_SPI_1or3V_GPIO_Port GPIOD
#define NA_L_SPI_3V_Sw_Pin GPIO_PIN_2
#define NA_L_SPI_3V_Sw_GPIO_Port GPIOD
#define NA_S_SPI_3V_Sw_Pin GPIO_PIN_3
#define NA_S_SPI_3V_Sw_GPIO_Port GPIOD
#define NA_S_MOD1orCAL_Pin GPIO_PIN_4
#define NA_S_MOD1orCAL_GPIO_Port GPIOD
#define NA_S_MOD2_Pin GPIO_PIN_5
#define NA_S_MOD2_GPIO_Port GPIOD
#define SA_SPI6_MISO_Pin GPIO_PIN_12
#define SA_SPI6_MISO_GPIO_Port GPIOG
#define SA_SPI6_SCK_Pin GPIO_PIN_13
#define SA_SPI6_SCK_GPIO_Port GPIOG
#define SA_SPI6_MOSI_Pin GPIO_PIN_14
#define SA_SPI6_MOSI_GPIO_Port GPIOG
#define MC_RX_Pin GPIO_PIN_0
#define MC_RX_GPIO_Port GPIOE
#define MC_TX_Pin GPIO_PIN_1
#define MC_TX_GPIO_Port GPIOE
#define L3_PI4_Pin GPIO_PIN_4
#define L3_PI4_GPIO_Port GPIOI
#define NA_ExtInt1_Pin GPIO_PIN_5
#define NA_ExtInt1_GPIO_Port GPIOI
#define NA_ExtInt1_EXTI_IRQn EXTI9_5_IRQn
#define NA_ExtInt2_Pin GPIO_PIN_6
#define NA_ExtInt2_GPIO_Port GPIOI
#define NA_ExtInt2_EXTI_IRQn EXTI9_5_IRQn
#define NA_ACK_Pin GPIO_PIN_7
#define NA_ACK_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
