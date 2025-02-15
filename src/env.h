#ifndef __ENV_H
#define __ENV_H
#include <stdint.h>
#include"type.h"

#ifdef ALIENTEK_MINI
//SPI Flash devide
/*
 * 0x000000
 * Dont Know
 * 0x3A0000
 * ziku16
 * 0x3E2000
 * ziku12
 * 0x411EB0
 * None
 * 0x412000
 * ENV
 * 0x413000
 * ENV Help
 * 0x414000
 * log
 * 0x420000
 * null
 * 0x4e0000
 * Dont Know
 * 0x800000
 */
//spi flash damaged now 2022/3/6
//ENV offset changed to OK flash addr
//
//#define SPI_FLASH_ZIKU16_START 0x7E000
#define SPI_FLASH_ZIKU16_START 0x3A0000
#define SPI_FLASH_ZIKU12_START 0x3E2000

#define SPI_FLASH_DAMAGED

#define TOTAL_SPI_FLASH_SIZE 0x200000
#define SPI_FLASH_SECTOR_SIZE 0x1000
#define GET_SECTOR_ADDR(addr) ((addr)>>12)
#define SECTORS_PER_ENV_BLOCK 1
#define ENV_STORE_SIZE (SPI_FLASH_SECTOR_SIZE*SECTORS_PER_ENV_BLOCK)

#ifdef SPI_FLASH_DAMAGED
//#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE/2-ENV_STORE_SIZE)
#define ENV_STORE_START_ADDR (0x412000)
#else
//env store at end of flash
#define ENV_STORE_START_ADDR (0x412000)
#endif //SPI_FLASH_DAMAGED
#define ENV_HELP_STORE_START_ADDR (0x413000)

#define SPI_FLASH_LOG_START 0x414000
#define SPI_FLASH_LOG_END 0x420000

#elif defined SUNRISE
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
 * 0x0F0000
 * log
 * 0x0FA000
 * ENV Help
 * 0x0FD000
 * ENV
 * 0x100000
 * Not used
 * 0x180000
 * ziku24
 * 0x200000
 */
//spi flash damaged now 2022/3/6
//ENV offset changed to OK flash addr
//
#define SPI_FLASH_ZIKU24_START 0x180000
#define SPI_FLASH_ZIKU16_START 0x7E000
#define SPI_FLASH_ZIKU12_START 0xC0000

#define SPI_FLASH_DAMAGED

#define TOTAL_SPI_FLASH_SIZE 0x200000
#define SPI_FLASH_SECTOR_SIZE 0x1000
#define GET_SECTOR_ADDR(addr) ((addr)>>12)
#define SECTORS_PER_ENV_BLOCK 3
#define ENV_STORE_SIZE (SPI_FLASH_SECTOR_SIZE*SECTORS_PER_ENV_BLOCK)

#ifdef SPI_FLASH_DAMAGED
#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE/2-ENV_STORE_SIZE)
#else
//env store at end of flash
#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE-ENV_STORE_SIZE)
#endif //SPI_FLASH_DAMAGED
#define ENV_HELP_STORE_START_ADDR (ENV_STORE_START_ADDR-ENV_STORE_SIZE)

#define SPI_FLASH_LOG_START 0x0F0000
#define SPI_FLASH_LOG_END 0x0FA000

#else//
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
 * 0x0F0000
 * log
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
#define GET_SECTOR_ADDR(addr) ((addr)>>12)
#define SECTORS_PER_ENV_BLOCK 3
#define ENV_STORE_SIZE (SPI_FLASH_SECTOR_SIZE*SECTORS_PER_ENV_BLOCK)

#ifdef SPI_FLASH_DAMAGED
#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE/2-ENV_STORE_SIZE)
#else
//env store at end of flash
#define ENV_STORE_START_ADDR (TOTAL_SPI_FLASH_SIZE-ENV_STORE_SIZE)
#endif //SPI_FLASH_DAMAGED
#define ENV_HELP_STORE_START_ADDR (ENV_STORE_START_ADDR-ENV_STORE_SIZE)

#define SPI_FLASH_LOG_START 0x0F0000
#define SPI_FLASH_LOG_END 0x0FA000
#endif//ALIENTEK_MINI

#define SPI_FLASH_LOG_SIZE (SPI_FLASH_LOG_END-SPI_FLASH_LOG_START)
#define ENV_OK 0
#define ENV_FAIL 1
#define ENV_INVALID 0xFFFFFFFF
#define ENV_EMPTY_DATA 0xFFFFFFFE
#define ENV_ABNORMAL 0xFFFFFFFD
#define ENV_FULL 0xFFFFFFFC
#define ENV_MAX_VALUE_LEN 0x20

#define USE_MAIN_ENV 0
#define USE_HELP_ENV 1

#define PRINT_ACTIVE_ENV 0
#define PRINT_RAW_ENV 1
#define COPY_DATA_ENV 2

uint32_t get_env(const char* name, char*value);
uint32_t set_env(const char* name, const char*value);
int printenv();
int printrawenv();
void switch_env_area();
void switch_env_area_with_data();
int erase_env_area();
uint32_t get_env_uint(const char*name, uint32_t def_value);
uint32_t set_env_uint(const char*name, uint32_t value);
void log_to_flash(const char*, u32 ri, u32 len, u32 buf_size);
void spi_flash_log_print();

#endif

