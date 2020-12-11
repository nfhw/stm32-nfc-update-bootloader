/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_H
#define __HARDWARE_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define USER_FLASH_STARTADDRESS    ((uint32_t)0x08000000)

/* Define the address from where firmware will be stored. */
#define FIRMWARE_ADDRESS            ((uint32_t)0x08008000) /* Upgraded firmware base address */
#define FIRMWARE_ADDRESS_END        ((uint32_t)0x08030000)
#define FIRMWARE_ADDRESS_LAST_PAGE  ((uint32_t)FIRMWARE_ADDRESS_END - FLASH_PAGE_SIZE)
#define FIRMWARE_ADDRESS_LAST_WORD  ((uint32_t)FIRMWARE_ADDRESS_END - 4)
#define FIRMWARE_FLASH_SIZE         ((uint32_t)FIRMWARE_ADDRESS_END - FIRMWARE_ADDRESS)

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

/* Exported macro ------------------------------------------------------------*/
#ifdef DEBUG
#include "SEGGER_RTT.h"
#define DBG_PRINTF(...)               SEGGER_RTT_printf(0, __VA_ARGS__)
#define DEBUG_PRINTF(...)             SEGGER_RTT_printf(0, __VA_ARGS__)
#define DEBUG_MSG(str)                SEGGER_RTT_WriteString(0, str)
#else
#define DBG_PRINTF(...)               ((void)0)
#define DEBUG_PRINTF(fmt, args...)    ((void)0)
#define DEBUG_MSG(str)                ((void)0)
#endif

/* Exported functions --------------------------------------------------------*/
void DBG_PrintBuffer(const char* pre, const uint8_t *buf, uint16_t len, const char* post);
void HW_EraseEEPROM(uint32_t address);
void HW_EraseFLASH(const uint32_t address, const uint32_t last_address);
void HW_ProgramEEPROM(uint32_t address, uint32_t data);
void HW_ProgramFLASH(const uint32_t off, const uint8_t * const buf, const uint32_t len);
void MAINFW_Jump(void);

#ifdef __cplusplus
}
#endif
#endif /* __HARDWARE_H */
