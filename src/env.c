#include "env.h"
#include "common.h"
#include <string.h>
#include <stdint.h>

static uint32_t env_start_addr = 0xffffffff;
uint8_t env_get_char(uint32_t offset);
void set_cur_env_area(int env_area);
static u32 flash_log_write_addr = 0xffffffff;
#define ENV_ADDR_CACHE_N 3
u32 env_addr_cache[ENV_ADDR_CACHE_N]={0};
u32 cache_w = 0;
uint32_t get_env_start_addr()
{
    return 0;
}

int erase_env_area()
{
    return ENV_OK;
}

void set_cur_env_area(int env_area)
{
}

int get_idle_env_area()
{
        return USE_MAIN_ENV;
}
int get_cur_env_area()
{
        return USE_MAIN_ENV;
}

void switch_env_area()
{
}


/*
 * the env store is like this:
 * XX XX '= YY .. YY 00 FF FF FF ... FF 00 XX XX XX '= YY YY YY 00 XX XX ...
 * */
uint32_t find_env_data_start_raw()
{
    return 0;
}

uint32_t find_env_data_start()
{
    return 0;
}

void clr_cache()
{
}

uint32_t get_env_raw_cache(const char* name, char*value, uint32_t * p_position)
{
        return ENV_OK;
}

uint32_t get_env_raw(const char* name, char*value, uint32_t * p_position)
{
        return ENV_OK;
}

uint32_t get_env(const char* name, char*value)
{
    return get_env_raw(name, value, NULL);
}

uint32_t get_name_position(const char* name)
{
    return ENV_OK;
}

uint32_t set_env_raw(const char* name, const char*value)
{
    return ENV_OK;
}

uint32_t set_env(const char* name, const char*value)
{
    return ENV_OK;
}

/************************************************************************
 * Command interface: print one or all environment variables
 */
int go_through_env(int operation)
{
    return ENV_OK;
}

int printenv()
{
    return go_through_env(PRINT_ACTIVE_ENV);
}

int printrawenv()
{
    return go_through_env(PRINT_RAW_ENV);
}

void switch_env_area_with_data()
{
}

uint32_t get_env_uint(const char*name, uint32_t def_value)
{
        return def_value;
}

uint32_t set_env_uint(const char*name, uint32_t value)
{
        return ENV_OK;
}

void find_log_write_addr()
{
}

void log_to_flash(const char*lgbuf, u32 ri, u32 len, u32 buf_size)
{
}

void spi_flash_log_print()
{
}
