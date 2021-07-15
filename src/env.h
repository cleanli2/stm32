#ifndef __ENV_H
#define __ENV_H
#include <stdint.h>

#define TOTAL_SPI_FLASH_SIZE 0x200000
#define SPI_FLASH_SECTOR_SIZE 0x1000
#define SECTORS_PER_ENV_BLOCK 1
#define ENV_STORE_SIZE (SPI_FLASH_SECTOR_SIZE*SECTORS_PER_ENV_BLOCK)

//env store at end of flash
#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE-ENV_STORE_SIZE)
#define ENV_HELP_STORE_START_ADDR (ENV_STORE_START_ADDR-ENV_STORE_SIZE)

#define ENV_OK 0
#define ENV_FAIL 1
uint32_t get_env(const uint8_t* name, uint8_t*value);
uint32_t set_env(const uint8_t* name, const uint8_t*value);
int printenv();

#endif

