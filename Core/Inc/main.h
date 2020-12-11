/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* EEPROM Layout */
#define EEPROM_BOOTMODE           (DATA_EEPROM_BASE)
#define EEPROM_BOOTMODE_END       (DATA_EEPROM_BASE + 0x4)
#define EEPROM_UNUSED0            (DATA_EEPROM_BASE + 0x4)
#define EEPROM_UNUSED0_END        (DATA_EEPROM_BASE + 0x8)
#define EEPROM_PW                 (DATA_EEPROM_BASE + 0x8)
#define EEPROM_PW_END             (DATA_EEPROM_BASE + 0xc)
#define EEPROM_PW_COMPLEMENT      (DATA_EEPROM_BASE + 0xc)
#define EEPROM_PW_COMPLEMENT_END  (DATA_EEPROM_BASE + 0x10)
#define EEPROM_LORA               (DATA_EEPROM_BASE + 0x10)       // start flash adress to store lorawan context
#define EEPROM_LORA_END           (DATA_EEPROM_BASE + 0x200)
/* Bootloader BOOTMODES */
#define BOOTMODE_MAINFW           ((uint32_t)0x0)
#define BOOTMODE_WAITNFC_MASK     ((uint32_t)0x1) /* Bootldr hangs around for longer */
#define BOOTMODE_PASSOK_MASK      ((uint32_t)0x2) /* Bootldr is privileged from get go */
#define BOOTMODE_KEEPNFC_MASK     ((uint32_t)0x4) /* Bootldr doesn't reset ST25DV IC */


/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void manage_FWUpd(uint32_t wait, bool pw_valid);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Button0_Pin GPIO_PIN_0
#define Button0_GPIO_Port GPIOA
#define Button0_EXTI_IRQn EXTI0_1_IRQn
#define ST25DVx_NFC_Int_Pin GPIO_PIN_1
#define ST25DVx_NFC_Int_GPIO_Port GPIOA
#define ST25DVx_NFC_Int_EXTI_IRQn EXTI0_1_IRQn
#define RF_Switch_Pin GPIO_PIN_2
#define RF_Switch_GPIO_Port GPIOA
#define SX126x_SPI_NSS_Pin GPIO_PIN_4
#define SX126x_SPI_NSS_GPIO_Port GPIOA
#define SX126x_SPI_SCK_Pin GPIO_PIN_5
#define SX126x_SPI_SCK_GPIO_Port GPIOA
#define SX126x_SPI_MISO_Pin GPIO_PIN_6
#define SX126x_SPI_MISO_GPIO_Port GPIOA
#define SX126x_SPI_MOSI_Pin GPIO_PIN_7
#define SX126x_SPI_MOSI_GPIO_Port GPIOA
#define DC_Conv_Mode_Pin GPIO_PIN_0
#define DC_Conv_Mode_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_8
#define LED_1_GPIO_Port GPIOA
#define LED_2_Pin GPIO_PIN_9
#define LED_2_GPIO_Port GPIOA
#define SX126x_Busy_Pin GPIO_PIN_11
#define SX126x_Busy_GPIO_Port GPIOA
#define SX126x_DIO3_Pin GPIO_PIN_12
#define SX126x_DIO3_GPIO_Port GPIOA
#define SWDIO_STLink_Pin GPIO_PIN_13
#define SWDIO_STLink_GPIO_Port GPIOA
#define SWCLK_STLink_Pin GPIO_PIN_14
#define SWCLK_STLink_GPIO_Port GPIOA
#define SX126x_Reset_Pin GPIO_PIN_4
#define SX126x_Reset_GPIO_Port GPIOB
#define SX126x_DIO1_Pin GPIO_PIN_5
#define SX126x_DIO1_GPIO_Port GPIOB
#define SX126x_DIO1_EXTI_IRQn EXTI4_15_IRQn
#define ST25DVx_I2C_SCL_Pin GPIO_PIN_6
#define ST25DVx_I2C_SCL_GPIO_Port GPIOB
#define ST25DVx_I2C_SDA_Pin GPIO_PIN_7
#define ST25DVx_I2C_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
