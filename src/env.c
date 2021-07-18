#include "env.h"
#include <string.h>
#include <stdint.h>

uint8_t env_get_char(uint32_t offset)
{
    uint8_t ret;
    SPI_Flash_Read(&ret, ENV_STORE_START_ADDR+offset, 1);
    //lprintf("sf_read %x@%x\n", ret, ENV_STORE_START_ADDR+offset);
    return ret;
}

void env_set_char(uint32_t offset, uint8_t d)
{
    lprintf("sf_write %x@%x\n", d, ENV_STORE_START_ADDR+offset);
    SPI_Flash_Write(&d, ENV_STORE_START_ADDR+offset, 1);
}

uint32_t strcpy2env(uint32_t env_offset, uint8_t *s)
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
    while(c=env_get_char(env_offset)){
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
            lprintf("env_store_start %x size %x\n", ENV_STORE_START_ADDR, ENV_STORE_SIZE);
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

uint32_t get_env(const uint8_t* name, uint8_t*value)
{
    int i = 0, nxt;

    if(!name){
        lprintf("name=NULL\n");
        return ENV_FAIL;
    }
    if(strchr(name, '=')!=NULL){
        lprintf("'=' can't be in name\n");
        return ENV_FAIL;
    }

    i = find_env_data_start();
    if(i > ENV_ABNORMAL){
        return ENV_FAIL;
    }
    for (i++; env_get_char(i) != '\0'; i=nxt+1) {
        int val;

        for (nxt=i; env_get_char(nxt) != '\0'; ++nxt) {
            if (nxt >= ENV_STORE_SIZE) {
                return ENV_FAIL;
            }
        }
        if ((val=envmatch((uint8_t *)name, i)) < 0)
            continue;
        strcpy2mem(value, val);
        if(*value==0){//null str
            return ENV_FAIL;
        }
        return ENV_OK;
    }

    return ENV_FAIL;
}

uint32_t set_env(const uint8_t* name, const uint8_t*value)
{
    int i = 0, n;
    uint8_t zero_str = 0;

    if(!name){
        lprintf("name=NULL\n");
        return ENV_FAIL;
    }
    if(!value){
        value = &zero_str;
    }
    if(strchr(name, '=')!=NULL || strchr(value, '=')){
        lprintf("'=' can't be in name or value\n");
        return ENV_FAIL;
    }

    i = find_env_data_start();
    if(i == ENV_EMPTY_DATA){
        i = ENV_STORE_SIZE - 1;
        env_set_char(i, '\0');
    }
    else if(i > ENV_ABNORMAL){
        return ENV_FAIL;
    }
    n = strlen(name)+strlen(value)+2;
    if(i<n){
        lprintf("env full -- tobe fix\n");
        return ENV_FAIL;
    }
    i -= n;
    env_set_char(i++, '\0');
    i += strcpy2env(i, name);
    env_set_char(i++, '=');
    strcpy2env(i, value);

    return ENV_OK;
}

/************************************************************************
 * Command interface: print one or all environment variables
 */
int printenv()
{
    int i, j;
    char c, buf[64];

    i = 0;
    buf[64] = '\0';
    lprintf("env_store_start %x size %x\n", ENV_STORE_START_ADDR, ENV_STORE_SIZE);

    i = find_env_data_start();
    if(i > ENV_ABNORMAL){
        return ENV_FAIL;
    }
    i++;
    while(env_get_char(i) != '\0'){
        lprintf("%x:", i);
        i+=strcpy2mem(buf, i);
        i++;
        lprintf("%s\n", buf);
        if(i>=ENV_STORE_SIZE){
            return;
        }
    }
}
