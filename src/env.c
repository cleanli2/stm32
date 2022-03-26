#include "env.h"
#include "common.h"
#include <string.h>
#include <stdint.h>

static uint32_t env_start_addr = 0xffffffff;
uint8_t env_get_char(uint32_t offset);
void set_cur_env_area(int env_area);
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
    int ct = SECTORS_PER_ENV_BLOCK;
    uint32_t i=0;
    uint32_t sector_addr = GET_SECTOR_ADDR(get_env_start_addr());
    while(ct--){
        lprintf("env_erase:sector %d\n", sector_addr);
        SPI_Flash_Erase_Sector(sector_addr++);
    }
    ct = ENV_STORE_SIZE;
    while(ct--){
        if(env_get_char(i)!=0xff){
            lprintf("%x@flash=%b!=0xff, erase fail\n", i,
                    env_get_char(i));
            return ENV_FAIL;
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
    lprintf("sf_write %x@%x\n", d, get_env_start_addr()+offset);
    SPI_Flash_Write(&d, get_env_start_addr()+offset, 1);
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
uint32_t find_env_data_start()
{
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
                goto end;
            }
        }
        ret = ENV_OK;
        goto end;
    }
end:
    set_touch_need_reinit();
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

uint32_t set_env(const char* name, const char*value)
{
    uint32_t i = 0, n, ret = ENV_OK;
    uint8_t zero_str = 0;

    if(!name){
        lprintf("name=NULL\n");
        ret = ENV_FAIL;
        goto end;
    }
    if(!value){
        value = (const char*)&zero_str;
    }
    if(strchr(name, '=')!=NULL || strchr(value, '=')){
        lprintf("'=' can't be in name or value\n");
        ret = ENV_FAIL;
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
        goto end;
    }
    n = strlen(name)+strlen(value)+2;
    if(i<n){
        lprintf("env full -- tobe fix\n");
        ret = ENV_FAIL;
        goto end;
    }
    i -= n;
    env_set_char(i++, '\0');
    i += strcpy2env(i, (uint8_t*)name);
    env_set_char(i++, '=');
    strcpy2env(i, (const uint8_t*)value);

end:
    set_touch_need_reinit();
    return ret;
}

/************************************************************************
 * Command interface: print one or all environment variables
 */
int go_through_env(int operation)
{
    uint32_t i, ret=ENV_OK, posi_name, posi;
    char buf[128], *name, *value, *posi_eq;

    i = 0;
    buf[64] = '\0';
    lprintf("env_store_start %x size %x\n", get_env_start_addr(), ENV_STORE_SIZE);

    i = find_env_data_start();
    if(i > ENV_ABNORMAL){
        ret = ENV_FAIL;
        goto end;
    }
    i++;
    while(env_get_char(i) != '\0'){
        posi = i;
        i+=strcpy2mem((uint8_t*)buf, i);
        i++;
        if(PRINT_RAW_ENV==operation){
            lprintf("%x:", posi);
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
                    if(PRINT_ACTIVE_ENV==operation){
                        *posi_eq='=';
                        lprintf("%s\n", buf);
                    }
                    else if(COPY_DATA_ENV==operation){
                        switch_env_area();
                        lprintf("copy env:%s=%s\n", name, value);
                        ret = set_env(name, value);
                        switch_env_area();
                        if(ret == ENV_FAIL){
                            lprintf("set env fail in idle env\n");
                            return ret;
                        }
                    }
                }
            }
        }
        if(i>=ENV_STORE_SIZE){
            goto end;
        }
    }
end:
    set_touch_need_reinit();
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
