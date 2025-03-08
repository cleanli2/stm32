#include "env.h"
#include "common.h"
#include <string.h>
#include <stdint.h>

static uint32_t env_start_addr = 0xffffffff;
uint8_t env_get_char(uint32_t offset);
void set_cur_env_area(int env_area);
static u32 flash_log_write_addr = 0xffffffff;
uint32_t get_env_start_addr()
{
    uint8_t tmpc;

    if(env_start_addr == 0xffffffff){
        //last byte will not be 0xff if main env has data
        set_cur_env_area(USE_MAIN_ENV);
        tmpc = env_get_char(ENV_STORE_SIZE-1);
        lprintf("last byte=0x%b\n", tmpc);
        if(tmpc==0xff){
            //will use help env
            lprintf("env:set hlep env %X\n", ENV_STORE_START_ADDR);
            set_cur_env_area(USE_HELP_ENV);
        }
        lprintf("env use %X\n", env_start_addr);
    }
    return env_start_addr;
}

int erase_env_area()
{
    uint8_t r;
    int ct = SECTORS_PER_ENV_BLOCK;
    uint32_t i=0;
    uint32_t sector_addr = GET_SECTOR_ADDR(get_env_start_addr());
    while(ct--){
        lprintf("env_erase:sector %d\n", sector_addr);
        SPI_Flash_Erase_Sector(sector_addr++);
    }
    ct = ENV_STORE_SIZE;
    while(ct--){
        if((r=env_get_char(i))!=0xff){
            lprintf("%x@flash=%b!=0xff, retry\n", i, r);
            r=env_get_char(i);
            if(r!=0xff){
                return ENV_FAIL;
            }
            else{
                lprintf("retry OK\r\n");
            }
        }
        i++;
    }
    return ENV_OK;
}

void set_cur_env_area(int env_area)
{
    if(env_area == USE_MAIN_ENV){
        lprintf("env set to main %X\n", ENV_STORE_START_ADDR);
        env_start_addr=ENV_STORE_START_ADDR;
    }
    else if(env_area == USE_HELP_ENV){
        lprintf("env set to help %X\n", ENV_HELP_STORE_START_ADDR);
        env_start_addr=ENV_HELP_STORE_START_ADDR;
    }
    else{
        lprintf("set_cur_env_area:fault para. Doing nothing\n");
    }
}

int get_idle_env_area()
{
    if( get_env_start_addr()==ENV_STORE_START_ADDR){
        lprintf("curenv is main %X\n", ENV_STORE_START_ADDR);
        return USE_HELP_ENV;
    }
    else if(get_env_start_addr()==ENV_HELP_STORE_START_ADDR){
        lprintf("curenv is help %X\n", ENV_HELP_STORE_START_ADDR);
        return USE_MAIN_ENV;
    }
    return ENV_INVALID;
}
int get_cur_env_area()
{
    if(get_env_start_addr()==ENV_STORE_START_ADDR){
        lprintf("cur env is main %X\n", ENV_STORE_START_ADDR);
        return USE_MAIN_ENV;
    }
    else if(get_env_start_addr()==ENV_HELP_STORE_START_ADDR){
        lprintf("cur env is help %X\n", ENV_HELP_STORE_START_ADDR);
        return USE_HELP_ENV;
    }
    return ENV_INVALID;
}

void switch_env_area()
{
    set_cur_env_area(get_idle_env_area());
}

uint8_t env_get_char(uint32_t offset)
{
    uint8_t ret;
    SPI_Flash_Read(&ret, get_env_start_addr()+offset, 1);
    //lprintf("sf_read %x@%x\n", ret, get_env_start_addr()+offset);
    return ret;
}

void env_set_char(uint32_t offset, uint8_t d)
{
    uint8_t rd;
    SPI_Flash_Write_Byte(d, get_env_start_addr()+offset);
    rd = SPI_Flash_Read_Byte(get_env_start_addr()+offset);
    if(d == rd){
        //lprintf("sf_write %x@%x OK\n", d, get_env_start_addr()+offset);
    }
    else{
        lprintf("sf_write %x@%x#%x\n", (uint32_t)d, get_env_start_addr()+offset, (uint32_t)rd);
    }
}

uint32_t strcpy2env(uint32_t env_offset, const uint8_t *s)
{
    uint32_t len = 0;
    while(*s){
        len++;
        env_set_char(env_offset++, *s++);
    }
    return len;
}

uint32_t strcpy2mem(uint8_t *s, uint32_t env_offset)
{
    uint32_t len = 0;
    uint8_t c;
    while((c = env_get_char(env_offset))){
        lprintf("c=%b off=%x\r\n", c, env_offset);
        len++;
        *s++ = c;
        env_offset++;
    }
    *s = 0;
    return len;
}

int envmatch (uint8_t *s1, int i2)
{

    while (*s1 == env_get_char(i2++))
        if (*s1++ == '=')
            return(i2);
    if (*s1 == '\0' && env_get_char(i2-1) == '=')
        return(i2);
    return(-1);
}

/*
 * the env store is like this:
 * XX XX '= YY .. YY 00 FF FF FF ... FF 00 XX XX XX '= YY YY YY 00 XX XX ...
 * */
uint32_t find_env_data_start_raw()
{
    uint32_t i_down=0, i_up=ENV_STORE_SIZE-1, i;
    if(env_get_char(i_down) != 0xff){
        lprintf("env data is full\n");
        return ENV_INVALID;
    }
    if(env_get_char(i_up) == 0xff){
        lprintf("empty env block\n");
        return ENV_EMPTY_DATA;
    }
    while(1!=i_up-i_down){
        //lprintf("[%d,%d]\n", i_down, i_up);
        i=(i_down+i_up)/2;
        if(0xff==env_get_char(i)){
            i_down = i;
        }
        else{
            i_up = i;
        }
    }
    //lprintf("i_up %d\n", i_up);
    if(env_get_char(i_up) != 0){
        lprintf("FFXX g env flash error %x\n", i_up);
        lprintf("env_store_start %x size %x\n", get_env_start_addr(), ENV_STORE_SIZE);
        return ENV_FAIL;
    }
    return i_up;
#if 0
    uint32_t i = 0, ff_i = 0;
    //go through not 0xff
    if(env_get_char(i) != 0xff){
        while(env_get_char(i) != 0xff){
            i++;
            if(i == ENV_STORE_SIZE){
                lprintf("env data is full\n");
                return ENV_INVALID;
            }
        }
        if(env_get_char(i-1) != 0){
            lprintf("00FF g env flash error %x\n", i-2);
            lprintf("env_store_start %x size %x\n", get_env_start_addr(), ENV_STORE_SIZE);
            return ENV_FAIL;
        }
    }

    //go through 0xff
    while(env_get_char(i) == 0xff){
        i++;
        ff_i++;
        if(i == ENV_STORE_SIZE){//new ENV BLOCK, all 0xff
            lprintf("end of env store is 0xff\n");
            if(i == ENV_STORE_SIZE){
                lprintf("empty env block\n");
                return ENV_EMPTY_DATA;
            }
            else{
                return ENV_INVALID;
            }
        }
    }
    if(env_get_char(i) != 0){
        lprintf("FF00 env flash error %x\n", i);
        return ENV_INVALID;
    }
    return i;
#endif
}

uint32_t find_env_data_start()
{
    static int env_data_start_i = -1;
    uint32_t ret;
    uint8_t db, da;
    if(env_data_start_i != -1){//check if it is OK
        db = env_get_char(env_data_start_i -1);
        da = env_get_char(env_data_start_i);
        if(0xff == db && 0x00 == da){
            return env_data_start_i;
        }
        else{
            lprintf("env_data_start_i err:0x%b 0x%b\n", db, da);
        }
    }
    ret = find_env_data_start_raw();
    if(ret > ENV_ABNORMAL && ret != ENV_EMPTY_DATA){
        lprintf("w25f read fail, reinit SD lowlevel\n");
        SD_LowLevel_Init();
        //retry
        ret = find_env_data_start_raw();
    }
    env_data_start_i = ret;
    return ret;
}

uint32_t get_env_raw(const char* name, char*value, uint32_t * p_position)
{
    uint32_t i = 0, nxt, ret = ENV_OK;
    if(p_position!=NULL){
        *p_position=ENV_INVALID;
    }

    if(!name){
        lprintf("name=NULL\n");
        ret = ENV_FAIL;
        goto end;
    }
    if(strchr(name, '=')!=NULL){
        lprintf("'=' can't be in name\n");
        ret = ENV_FAIL;
        goto end;
    }

    i = find_env_data_start();
    if(i > ENV_ABNORMAL){
        ret = ENV_FAIL;
        lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    for (i++; env_get_char(i) != '\0'; i=nxt+1) {
        int val;

        for (nxt=i; env_get_char(nxt) != '\0'; ++nxt) {
            if (nxt >= ENV_STORE_SIZE) {
                ret = ENV_FAIL;
                goto end;
            }
        }
        if ((val=envmatch((uint8_t *)name, i)) < 0)
            continue;
        if(p_position!=NULL){
            *p_position = i;
            goto end;
        }
        if(value!=NULL){
            strcpy2mem((uint8_t*)value, val);
            if(*value==0){//null str
                ret = ENV_FAIL;
                lprintf("--enverr%d\n",__LINE__);
                goto end;
            }
        }
        ret = ENV_OK;
        goto end;
    }
end:
    return ret;
}

uint32_t get_env(const char* name, char*value)
{
    return get_env_raw(name, value, NULL);
}

uint32_t get_name_position(const char* name)
{
    uint32_t ret;
    get_env_raw(name, NULL, &ret);
    return ret;
}

uint32_t set_env_raw(const char* name, const char*value)
{
    uint32_t i = 0, n, ret = ENV_OK;
    uint8_t zero_str = 0;

    if(!name){
        lprintf("name=NULL\n");
        ret = ENV_FAIL;
        lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    if(!value){
        value = (const char*)&zero_str;
    }
    if(strchr(name, '=')!=NULL || strchr(value, '=')){
        lprintf("'=' can't be in name or value\n");
        ret = ENV_FAIL;
        lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    char ev[ENV_MAX_VALUE_LEN];
    if(ENV_OK==get_env(name, ev)){
        if(!strcmp(ev, value)){
            lprintf("env already set\n");
            return ret;
        }
    }

    i = find_env_data_start();
    if(i == ENV_EMPTY_DATA){
        i = ENV_STORE_SIZE - 1;
        env_set_char(i, '\0');
    }
    else if(i > ENV_ABNORMAL){
        ret = ENV_FAIL;
        lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    lprintf("env used %d%\n", (ENV_STORE_SIZE - i)*100/ENV_STORE_SIZE);
    n = strlen(name)+strlen(value)+2;
    if(i<n){
        lprintf("env full\n");
        ret = ENV_FULL;
        lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    i -= n;
    env_set_char(i++, '\0');
    i += strcpy2env(i, (uint8_t*)name);
    env_set_char(i++, '=');
    strcpy2env(i, (const uint8_t*)value);

end:
    return ret;
}

uint32_t set_env(const char* name, const char*value)
{
    uint32_t ret;
    ret = set_env_raw(name, value);
    if(ret==ENV_FULL){
        lprintf("env full. Try switching env area...\n");
        switch_env_area_with_data();
        lprintf("set env again\n");
        ret = set_env_raw(name, value);
    }
    return ret;
}

/************************************************************************
 * Command interface: print one or all environment variables
 */
int go_through_env(int operation)
{
    uint32_t i, ret=ENV_OK, posi_name, posi;
    char buf[64], *name, *value, *posi_eq;

    i = 0;
    memset(buf, 0, 64);
    get_cur_env_area();
    lprintf("env_store_start %x size %x\n\n", get_env_start_addr(), ENV_STORE_SIZE);

    i = find_env_data_start();
    if(i > ENV_ABNORMAL){
        ret = ENV_FAIL;
        //lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    lprintf("env used %d%\n", (ENV_STORE_SIZE - i)*100/ENV_STORE_SIZE);
    i++;
    while(env_get_char(i) != '\0'){
        posi = i;
        i+=strcpy2mem((uint8_t*)buf, i);
        i++;
        if(PRINT_RAW_ENV==operation){
            lprintf("%x:", posi);
            mem_print(buf, 0, 64);
            lprintf("%s\n", buf);
        }
        else{
            posi_eq=strchr(buf, '=');
            if(posi_eq){
                name = buf;
                *posi_eq=0;
                value= posi_eq+1;
                posi_name = get_name_position(name);
                if(posi_name == ENV_INVALID ||
                        posi_name == posi){
                    if(*value){//value is not null
                        if(PRINT_ACTIVE_ENV==operation){
                            *posi_eq='=';
                            lprintf("%s\n", buf);
                        }
                        else if(COPY_DATA_ENV==operation){
                            lprintf("copy env:%s=%s\n", name, value);
                            switch_env_area();
                            ret = set_env_raw(name, value);
                            switch_env_area();
                            if(ret == ENV_FAIL){
                                lprintf("set env fail in idle env\n");
                                return ret;
                            }
                        }
                    }
                }
            }
        }
        if(i>=ENV_STORE_SIZE){
            //lprintf("--enverr%d\n",__LINE__);
            goto end;
        }
    }
end:
    return ret;
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
    //int cur_area = get_cur_env_area();
    //int idle_area = get_idle_env_area();
    if(ENV_OK == go_through_env(COPY_DATA_ENV)){
        lprintf("copy env to idle OK, erase cur env\n");
        erase_env_area();
        switch_env_area();
    }
}

uint32_t get_env_uint(const char*name, uint32_t def_value)
{
    char t[ENV_MAX_VALUE_LEN], *p=&t[0];
    uint32_t ret;
    if(ENV_OK == get_env(name, t)){
        str_to_hex(p, &ret);
        lprintf("envgetint %s=%x\n", name, ret);
        return ret;
    }
    else{
        lprintf("envgetint %s fail, use default\n", name);
        return def_value;
    }
}

uint32_t set_env_uint(const char*name, uint32_t value)
{
    char t[ENV_MAX_VALUE_LEN];
    memset(t, 0, ENV_MAX_VALUE_LEN);
    slprintf(t, "%x", value);
    if(ENV_OK == set_env(name, t)){
        return ENV_OK;
    }
    else{
        lprintf("envsetint %s fail\n", name);
        return ENV_FAIL;
    }
}

void find_log_write_addr()
{
    flash_log_write_addr = SPI_FLASH_LOG_START;
    while(1)
    {
        if(0xff==SPI_Flash_Read_Byte(flash_log_write_addr))
        {
            return;
        }
        flash_log_write_addr++;
        if(SPI_FLASH_LOG_END <= flash_log_write_addr){
            lprintf("full of log, erase first sector to start\n");
            flash_log_write_addr = SPI_FLASH_LOG_START;
            SPI_Flash_Erase_Sector(flash_log_write_addr/SPI_FLASH_SECTOR_SIZE);//erase sector
            return;
        }
    }
}

void log_to_flash(const char*lgbuf, u32 ri, u32 len, u32 buf_size)
{
    u32 buf_left, flash_left, w_len;
    if(0xffffffff == flash_log_write_addr){
        find_log_write_addr();
    }

    lprintf("log>flash:i %d len %d fadr 0x%x\n",
            ri, len, flash_log_write_addr);
    while(1){
        lprintf("ri %d %d %x\n", ri, len, flash_log_write_addr);
        if(ri+len >= buf_size)
        {
            buf_left = buf_size - ri;
        }
        else{
            buf_left = len;
        }
        flash_left = SPI_FLASH_LOG_END - flash_log_write_addr;
        w_len = MIN(buf_left, flash_left);
        lprintf("wl %d %d %x\n", w_len, buf_left, flash_left);
        SPI_Flash_Write_direct_erase((const u8*)lgbuf+ri, flash_log_write_addr, w_len);
        flash_log_write_addr += w_len;
        ri += w_len;
        len -= w_len;
        if(0 == len){
            break;
        }
        if(flash_log_write_addr >= SPI_FLASH_LOG_END){
            flash_log_write_addr = SPI_FLASH_LOG_START;
        }
        if(ri >= buf_size){
            ri = 0;
        }
    }
    if(0xff!=SPI_Flash_Read_Byte(flash_log_write_addr))
    {
        lprintf("next flash log addr %x is not 0xff, erase it\n",
                flash_log_write_addr);
        SPI_Flash_Erase_Sector(flash_log_write_addr/SPI_FLASH_SECTOR_SIZE);//erase sector
    }
    lprintf("log>flash:done fadr 0x%x\n", flash_log_write_addr);
}

void spi_flash_log_print()
{
    u32 ch_cnt = 0;
    char ch;
    u32 addr = SPI_FLASH_LOG_START;
    while(1){
        ch=SPI_Flash_Read_Byte(addr);
        if(0xff!=ch)
        {
            __io_putchar(ch);
        }
        ch_cnt++;
        if(ch_cnt >= SPI_FLASH_LOG_SIZE){
            return;
        }
        addr++;
    }
}
