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
uint32_t get_env(const uint8_t* name, uint8_t*value)
{
    int i = 0, nxt;

    if(strchr(name, '=')!=NULL){
        lprintf("'=' can't be in name\n");
        return ENV_FAIL;
    }

    //go through not 0xff
    if(env_get_char(i++) != 0xff){
        while(env_get_char(i++) != 0xff);
        if(env_get_char(i-2) != 0){
            lprintf("00FF g env flash error %x\n", i-2);
            lprintf("env_store_start %x size %x\n", ENV_STORE_START_ADDR, ENV_STORE_SIZE);
            return ENV_FAIL;
        }
    }
    //go through 0xff
    while(env_get_char(i) == 0xff){
        i++;
        if(i == ENV_STORE_SIZE){//new ENV BLOCK, all 0xff
            lprintf("empty env block\n");
            return ENV_FAIL;
        }
    }
    if(env_get_char(i) != 0){
        lprintf("FF00 env flash error %x\n", i);
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
        return ENV_OK;
    }

    return ENV_FAIL;
}

uint32_t set_env(const uint8_t* name, const uint8_t*value)
{
    int i = 0, n;

    if(strchr(name, '=')!=NULL || strchr(value, '=')){
        lprintf("'=' can't be in name or value\n");
        return ENV_FAIL;
    }

    //go through not 0xff
    if(env_get_char(i++) != 0xff){
        while(env_get_char(i++) != 0xff);
        if(env_get_char(i-2) != 0){
            lprintf("00FF s env flash error %x\n", i-2);
            return ENV_FAIL;
        }
    }
    //go through 0xff
    while(env_get_char(i) == 0xff){
        i++;
        if(i == ENV_STORE_SIZE){//new ENV BLOCK, all 0xff
            i--;
            env_set_char(i, '\0');
            break;
        }
    }
    if(env_get_char(i) != 0){
        lprintf("FF00 s env flash error %x\n", i);
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
    char c, buf[17];

    i = 0;
    buf[16] = '\0';
    //go through not 0xff
    if(env_get_char(i++) != 0xff){
        while(env_get_char(i++) != 0xff);
        if(env_get_char(i-2) != 0){
            lprintf("00FF g env flash error\n");
            return ENV_FAIL;
        }
    }
    //go through 0xff
    while(env_get_char(i) == 0xff){
        i++;
        if(i == ENV_STORE_SIZE){//new ENV BLOCK, all 0xff
            lprintf("empty env block\n");
            return ENV_FAIL;
        }
    }
    if(env_get_char(i) != 0){
        lprintf("FF00 env flash error\n");
        return ENV_FAIL;
    }
    i++;
    while(env_get_char(i) != '\0'){
        i+=strcpy2mem(buf, i);
        i++;
        lprintf("%x:%s\n", i, buf);
        if(i>ENV_STORE_SIZE){
            return;
        }
    }
}
