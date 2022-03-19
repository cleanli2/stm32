#ifndef __ENV_H
#define __ENV_H
#include <stdint.h>

//SPI Flash devide
/*
 * 0x000000
 * None
 * 0x07E000
 * ziku16
 * 0x0C0000
 * ziku12
 * 0x0EFEB0
 * None
 * 0x0FA000
 * ENV Help
 * 0x0FD000
 * ENV
 * 0x100000
 * Damaged
 * 0x200000
 */
//spi flash damaged now 2022/3/6
//ENV offset changed to OK flash addr
//
#define SPI_FLASH_ZIKU16_START 0x7E000
#define SPI_FLASH_ZIKU12_START 0xC0000

#define SPI_FLASH_DAMAGED

#define TOTAL_SPI_FLASH_SIZE 0x200000
#define SPI_FLASH_SECTOR_SIZE 0x1000
#define SECTORS_PER_ENV_BLOCK 3
#define ENV_STORE_SIZE (SPI_FLASH_SECTOR_SIZE*SECTORS_PER_ENV_BLOCK)

#ifdef SPI_FLASH_DAMAGED
#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE/2-ENV_STORE_SIZE)
#else
//env store at end of flash
#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE-ENV_STORE_SIZE)
#endif //SPI_FLASH_DAMAGED
#define ENV_HELP_STORE_START_ADDR (ENV_STORE_START_ADDR-ENV_STORE_SIZE)

#define ENV_OK 0
#define ENV_FAIL 1
#define ENV_INVALID 0xFFFFFFFF
#define ENV_EMPTY_DATA 0xFFFFFFFE
#define ENV_ABNORMAL 0xFFFFFFFD

uint32_t get_env(const uint8_t* name, uint8_t*value);
uint32_t set_env(const uint8_t* name, const uint8_t*value);
int printenv();

#endif

