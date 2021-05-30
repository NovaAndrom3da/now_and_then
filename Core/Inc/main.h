/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define A_Vbat_sig_Pin GPIO_PIN_0
#define A_Vbat_sig_GPIO_Port GPIOA
#define A_Vcar_sig_Pin GPIO_PIN_1
#define A_Vcar_sig_GPIO_Port GPIOA
#define A_Current__sig_Pin GPIO_PIN_2
#define A_Current__sig_GPIO_Port GPIOA
#define A_Current__sigA3_Pin GPIO_PIN_3
#define A_Current__sigA3_GPIO_Port GPIOA
#define spi_cs_Pin GPIO_PIN_4
#define spi_cs_GPIO_Port GPIOA
#define D_Aux_HS_sig_Pin GPIO_PIN_0
#define D_Aux_HS_sig_GPIO_Port GPIOB
#define D_Aux_LS_sig_Pin GPIO_PIN_1
#define D_Aux_LS_sig_GPIO_Port GPIOB
#define D_Shutdown_Contactor_sig_Pin GPIO_PIN_2
#define D_Shutdown_Contactor_sig_GPIO_Port GPIOB
#define IMD_fault_sig_Pin GPIO_PIN_10
#define IMD_fault_sig_GPIO_Port GPIOB
#define LD4_Pin GPIO_PIN_12
#define LD4_GPIO_Port GPIOD
#define LD3_Pin GPIO_PIN_13
#define LD3_GPIO_Port GPIOD
#define LD5_Pin GPIO_PIN_14
#define LD5_GPIO_Port GPIOD
#define LD6_Pin GPIO_PIN_15
#define LD6_GPIO_Port GPIOD
#define D_shutdown_latch_sig_Pin GPIO_PIN_8
#define D_shutdown_latch_sig_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define D_shutdown_bms_sig_Pin GPIO_PIN_10
#define D_shutdown_bms_sig_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define sw_dio_Pin GPIO_PIN_13
#define sw_dio_GPIO_Port GPIOA
#define sw_clk_Pin GPIO_PIN_14
#define sw_clk_GPIO_Port GPIOA
#define uart_tx_screen_Pin GPIO_PIN_5
#define uart_tx_screen_GPIO_Port GPIOD
#define spi_cs_temp_Pin GPIO_PIN_7
#define spi_cs_temp_GPIO_Port GPIOD
#define IMD_data_Pin GPIO_PIN_3
#define IMD_data_GPIO_Port GPIOB
#define D_precharge_cmd_Pin GPIO_PIN_5
#define D_precharge_cmd_GPIO_Port GPIOB
#define D_Final_Close_cmd_Pin GPIO_PIN_6
#define D_Final_Close_cmd_GPIO_Port GPIOB
#define D_charger_detect_sig_Pin GPIO_PIN_7
#define D_charger_detect_sig_GPIO_Port GPIOB
#define D_dcdc_enable_cmd_Pin GPIO_PIN_8
#define D_dcdc_enable_cmd_GPIO_Port GPIOB
#define D_BMS_fault_cmd_Pin GPIO_PIN_9
#define D_BMS_fault_cmd_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
