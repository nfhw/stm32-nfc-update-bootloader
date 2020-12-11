/* Includes ------------------------------------------------------------------*/
#include "hardware.h"
#include <string.h>

/* Public functions ----------------------------------------------------------*/
void HW_EraseEEPROM(uint32_t address) {
  HAL_FLASHEx_DATAEEPROM_Unlock();
  if (HAL_FLASHEx_DATAEEPROM_Erase(address) != HAL_OK) {
    DBG_PRINTF("ERROR ERASING EEPROM: 0x%02X!\n", address);
  }
  HAL_FLASHEx_DATAEEPROM_Lock();
}

void HW_ProgramEEPROM(uint32_t address, uint32_t data) {
  HAL_FLASHEx_DATAEEPROM_Unlock();
  if (HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, address, data)
      != HAL_OK) {
    DBG_PRINTF("ERROR PROGRAMMING EEPROM: 0x%02X!\n", address);
  }
  HAL_FLASHEx_DATAEEPROM_Lock();
}

void HW_EraseFLASH(const uint32_t address, const uint32_t last_address) {
  uint32_t page_error;
  FLASH_EraseInitTypeDef erase;

  if(address > last_address || address % FLASH_PAGE_SIZE || last_address % FLASH_PAGE_SIZE)
    goto err;

  erase.TypeErase = FLASH_TYPEERASE_PAGES;
  erase.PageAddress = address;
  erase.NbPages = (last_address - address) / FLASH_PAGE_SIZE;

  HAL_FLASH_Unlock();
  if(HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK) goto err;
  HAL_FLASH_Lock();
  if(page_error == 0xFFFFFFFF) return;
err:
  DEBUG_PRINTF("ERROR ERASING FLASH: 0x%08x to 0x%08x!\n", address, last_address);
}

void HW_ProgramFLASH(const uint32_t off, const uint8_t * const buf, const uint32_t len) {
  const uint32_t address = FIRMWARE_ADDRESS + off;
  const uint32_t last_address = FIRMWARE_ADDRESS_LAST_WORD;

  if(address > last_address || address % 4 || len % 4)
    goto err;

  HAL_FLASH_Unlock();
  for(uint32_t i = 0; i < len; i += 4) {
    uint32_t data;
    memcpy(&data, buf + i, sizeof data);
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, data)) goto err;
  }
  HAL_FLASH_Lock();

  return;
err:
  DEBUG_PRINTF("ERROR PROGRAMMING FLASH: 0x%08x to 0x%08x!\n", address, last_address);
}

void MAINFW_Jump(void) {
    /* Initialize user application's Stack Pointer */
    __set_MSP(((uint32_t*)FIRMWARE_ADDRESS)[0]);
    /* Jump to user application */
    ((void(*)(void))((uint32_t*)FIRMWARE_ADDRESS)[1])();
}

void DBG_PrintBuffer(const char* pre, const uint8_t *buf, uint16_t len, const char* post) {
  DBG_PRINTF("%s0x", pre);
  if(len) do {
    DBG_PRINTF("%02x", buf[--len]);
  } while(len);
  DBG_PRINTF("%s", post);
}
