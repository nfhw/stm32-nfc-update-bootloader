/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "lptim.h"
#include "rtc.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"
#include "hardware.h"
#include "nfc.h"
#include <string.h>  /* memcmp */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t offset = 0x0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_DMA_Init();
  //MX_ADC_Init();
  MX_I2C1_Init();
  //MX_IWDG_Init();
  //MX_LPTIM1_Init();
  //MX_RTC_Init();
  //MX_SPI1_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  /* ST25DV NFC Chip may fail erratically, unless DC Conv Mode (PB0) Pin
   * is output high, enabling stable VCC RF. */
  HAL_GPIO_WritePin(DC_Conv_Mode_GPIO_Port, DC_Conv_Mode_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RF_Switch_GPIO_Port, RF_Switch_Pin, GPIO_PIN_SET);

  DEBUG_PRINTF("BOOTED bootldr RTT@0x%08x\n", &_SEGGER_RTT);

  /* Write default password, if no password preset */
  if(~*(uint32_t*)EEPROM_PW != *(uint32_t*)EEPROM_PW_COMPLEMENT) {
    HW_EraseEEPROM(EEPROM_PW);
    HW_EraseEEPROM(EEPROM_PW_COMPLEMENT);
    HW_ProgramEEPROM(EEPROM_PW,            0x78563412);
    HW_ProgramEEPROM(EEPROM_PW_COMPLEMENT, 0x87a9cbed);
  }

  /* When FW upgrade is indicated by the MAINFW itself,
   * listen to NFC for 2 minutes, instead of 4 seconds. */
  uint32_t wait = *(uint32_t*)EEPROM_BOOTMODE & BOOTMODE_WAITNFC_MASK ? 120000 : 4000;
  bool privileged  = *(uint32_t*)EEPROM_BOOTMODE & BOOTMODE_PASSOK_MASK;
  bool keepmailbox = *(uint32_t*)EEPROM_BOOTMODE & BOOTMODE_KEEPNFC_MASK;

  /* Next boot will be straight into mainfw */
  if(*(uint32_t*)EEPROM_BOOTMODE) {
    HW_EraseEEPROM(EEPROM_BOOTMODE);
    HW_ProgramEEPROM(EEPROM_BOOTMODE, BOOTMODE_MAINFW);
  }

  /* Initialize NFC and manage comms */
  NFC_Init(keepmailbox);
  manage_FWUpd(wait, privileged);

  /* jump to existing FW */
  MAINFW_Jump();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC
                              |RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.LptimClockSelection = RCC_LPTIM1CLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/*
 * Manages the FW update process
 */
void manage_FWUpd(uint32_t wait, bool pw_valid) {
  uint32_t r;
  /* Mailbox buffers */
  struct NFC_State nfc = {0};
  MB_HEADER_T mb_header;
  /* Exit state */
  uint32_t timeout = HAL_GetTick() + wait;
  enum {
    UPD_NEVER,
    UPD_ONGOING,
    UPD_FAILED,
  } tried_flashing = UPD_NEVER;
  /* Upload bookkeeping */
  uint8_t nonword[4], nonword_size = 0;
  uint16_t last_chunk = 0, prev_chunk = 0;
  uint32_t bytes_expected = 0, bytes_written = 0, crc;

  do {
    /* User feedback on bootldr hanging due to NACK'ed crc32 */
    if(tried_flashing == UPD_FAILED)
      HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin,  HAL_GetTick() >> 10 & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Give the I2C bus a breather */
    if(!GPO_Activated) continue; else GPO_Activated = 0;

    /* Display the cause of interrupt, and mailbox state */
    r = NFC_ReadReg(ST25DV_ADDR_DATA_I2C, ST25DV_ITSTS_DYN_REG, (uint8_t*)&nfc, 3);
    DBG_PRINTF("NFC IRQ IT_STS:0x%02x MB_CTRL:0x%02x MB_LEN:0x%02x ret:%d err:%x Interrupt\n", nfc.it_sts, nfc.mb_ctrl, nfc.mb_len, r, hi2c1.ErrorCode);
    if(r) continue;

    /* Mailbox must have incoming data (put by RF) */
    if(~nfc.mb_ctrl & ST25DV_MB_CTRL_DYN_RFPUTMSG_MASK) continue;

    /* Read Mailbox */
    if(NFC_ReadReg(ST25DV_ADDR_DATA_I2C, ST25DV_MAILBOX_RAM_REG, nfc.mb, nfc.mb_len + 1)) continue;

    /* Check function code received */
    switch(nfc.mb[MB_FCTCODE]) {
    case MB_R2HPRESENTPASSWORD: {
      DBG_PrintBuffer("NFC <RX ", nfc.mb, nfc.mb_len + 1, ", Password Message\n");

      /* Verify message size, header and password */
      if(nfc.mb_len + 1 != 9) break;
      if(memcmp(nfc.mb, (const uint8_t[5]){MB_R2HPRESENTPASSWORD, MB_RESPONSE, MB_NOERROR, MB_NOTCHAINED, 0x04}, 5)) break;
      bool pw_was_valid = pw_valid;
      pw_valid = !memcmp((uint32_t*)EEPROM_PW, nfc.mb + 5, 4);

      /* Answer ok if password is good, bad request otherwise */
      const uint8_t response[5] = {MB_R2HPRESENTPASSWORD, MB_RESPONSE, pw_valid ? MB_NOERROR : MB_BADREQUEST, MB_NOTCHAINED, 0x00};
      if(NFC_WriteReg(ST25DV_ADDR_DATA_I2C, ST25DV_MAILBOX_RAM_REG, response, sizeof response)) continue;
      DBG_PrintBuffer("NFC >TX ", response, sizeof response, ", Password Message\n");

      /* Extend the timeout to 120 seconds */
      wait = 120000;

      /* Stop NFC comms and continue onto booting mainfw (even if borked!) */
      if(pw_was_valid && !pw_valid) goto exit;
      break;
    }
    case MB_R2HFIRMWAREUPDATE: {
      DBG_PrintBuffer("NFC <RX ", nfc.mb, nfc.mb_len + 1 < 20 ? nfc.mb_len + 1 : 20, ", Firmware Upload Message\n");
      if(!pw_valid) break;
      MBDecodeHeader(nfc.mb, &mb_header);

      /* Acknowledge received:
       * pass: stop NFC comms and boot mainfw
       * fail: keep listening */
      if(mb_header.cmdresp == MB_ACKNOWLEDGE) {
        if(mb_header.error) break; else goto exit;
      }

      /* Verify header:
       * - only commands, preclude incoming response or acknowledge
       * - only chained messages are sensible
       * - check message chunk numbers are sensible
       * - check nothing got skipped
       * - check that total byte count is consistent
       * - check that total chunk count is consistent
       * - check upload size fits in FLASH */
      bool first_msg = mb_header.chunknb == 1;
      bool bad_msg =
          mb_header.cmdresp != MB_COMMAND ||
          mb_header.chaining != MB_CHAINED ||
          mb_header.chunknb > mb_header.totalchunk ||
          (!first_msg && mb_header.chunknb != prev_chunk + 1) ||
          (!first_msg && mb_header.fulllength != bytes_expected) ||
          (!first_msg && mb_header.totalchunk != last_chunk) ||
          (!first_msg && tried_flashing != UPD_ONGOING) ||
          (first_msg && mb_header.fulllength > FIRMWARE_FLASH_SIZE);

      if(bad_msg || first_msg) {
        /* Reset bookkeeping */
        bytes_written = 0;
        bytes_expected = mb_header.fulllength;
        prev_chunk = 0;
        last_chunk = mb_header.totalchunk;
        nonword_size = 0;
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
      }
      if(bad_msg) {
        tried_flashing = tried_flashing ? UPD_FAILED : 0;
        break;
      }
      if(first_msg) {
        HW_EraseFLASH(FIRMWARE_ADDRESS, FIRMWARE_ADDRESS_LAST_PAGE);
        HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);

        /* Preclude bootup until the phone has ACK'ed crc32 of final frame */
        tried_flashing = UPD_ONGOING;
      }

      /* Write data to FLASH */
      {
        uint8_t *pos = nfc.mb + MB_CH_DATA;
        size_t len = mb_header.framelength;
        prev_chunk++;

        /* Resume partial word */
        if(nonword_size) {
          size_t size = 4U - nonword_size;
          size = size > len ? len : size;
          memcpy(nonword + nonword_size, pos, size);
          nonword_size += size, pos += size, len -= size;

          /* Write cached word */
          if(nonword_size == 4) {
            HW_ProgramFLASH(bytes_written, nonword, 4);
            DEBUG_PRINTF("FLASH <WR 0x%08x val:0x%02x%02x%02x%02x, Cached word\n", bytes_written, nonword[3], nonword[2], nonword[1], nonword[0]);
            bytes_written += 4, nonword_size = 0;
          }
        }

        /* Write words */
        if(len >> 2 << 2) {
          HW_ProgramFLASH(bytes_written, pos, len >> 2 << 2);
          DEBUG_PRINTF("FLASH <WR 0x%08x cnt:%d, Words\n", bytes_written, len);
          bytes_written += len >> 2 << 2, pos += len >> 2 << 2, len = len % 4;
        }

        /* Cache partial word */
        if(len) {
          nonword_size = len;
          memcpy(nonword, pos, len);

          /* Write partial word (last chunk) */
          if(mb_header.chunknb == last_chunk) {
            memset(nonword + nonword_size, 0, 4U - nonword_size);
            HW_ProgramFLASH(bytes_written, nonword, 4);
            DEBUG_PRINTF("FLASH <WR 0x%08x val:0x%02x%02x%02x%02x, Partial word\n", bytes_written, nonword[3], nonword[2], nonword[1], nonword[0]);
            bytes_written += nonword_size;
          }
        }
      }

      DEBUG_PRINTF("NFC UPD 0x%08x/0x%08x written\n", bytes_written, bytes_expected);

      /* User feedback by toggling green led on each flashed chunk */
      HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, prev_chunk % 2 ? GPIO_PIN_SET : GPIO_PIN_RESET);

      /* Last chunk */
      if(mb_header.chunknb == last_chunk) {
        /* Check that bytes add up */
        const uint8_t err = bytes_written == bytes_expected ? MB_NOERROR : MB_DEFAULTERROR;

        /* Prepare crc to send to reader */
        crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)FIRMWARE_ADDRESS, (uint32_t)(bytes_written / 4));
        const uint8_t response[9] = {MB_R2HFIRMWAREUPDATE, MB_RESPONSE, err, MB_NOTCHAINED, 0x04, crc >> 24, crc >> 16, crc >> 8, crc};
        if(NFC_WriteReg(ST25DV_ADDR_DATA_I2C, ST25DV_MAILBOX_RAM_REG, response, sizeof response)) continue;
        DBG_PrintBuffer("NFC >TX ", response, sizeof response, ", Firmware Upload Message\n");

        /* Until phone has responded, assume we've been NACK'ed */
        tried_flashing = UPD_FAILED;
      }
      break;
    }
    default: {
      DBG_PrintBuffer("NFC <RX ", nfc.mb, nfc.mb_len + 1, ", Undefined Message\n");
      break;
    }
    }

    // Prolong the timeout
    timeout = HAL_GetTick() + wait;
  /* Timeout on 2 minutes of inactivity */
  } while(tried_flashing || HAL_GetTick() < timeout || HAL_GetTick() >= -wait);
exit:
  /* Mailbox may contain password, clear it on privileged session close */
  NFC_ClearMailbox();

  /* Disco's over. */
  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
