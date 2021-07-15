#include "env.h"

uint8_t env_get_char(uint32_t offset)
{
    uint8_t ret;
    SPI_Flash_Read(&ret, ENV_STORE_START_ADDR+offset, 1);
    return ret;
}

void env_set_char(uint32_t offset, uint8_t d)
{
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

    while(env_get_char(i++) == 0xff);
    if(env_get_char(i-1) != 0){
        lprintf("env flash error\n");
        return ENV_FAIL;
    }
    for (; env_get_char(i) != '\0'; i=nxt+1) {
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
            lprintf("s env flash error\n");
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
        lprintf("s env flash error\n");
        return ENV_FAIL;
    }
    n = strlen(name)+strlen(value)+2;
    if(i<n){
        lprintf("env full -- tobe fix\n");
        return ENV_FAIL;
    }
    i -= n;
    env_set_char(i++, '\0');
    i = strcpy2env(i, name);
    env_set_char(i++, '=');
    i = strcpy2env(i, value);

    return ENV_FAIL;
}

/************************************************************************
 * Command interface: print one or all environment variables
 */

/*
 * state 0: finish printing this string and return (matched!)
 * state 1: no matching to be done; print everything
 * state 2: continue searching for matched name
 */
int printenv(char *name, int state)
{
    int i, j;
    char c, buf[17];

    i = 0;
    buf[16] = '\0';

    while (state && env_get_char(i) != '\0') {
        if (state == 2 && envmatch((uint8_t* )name, i) >= 0)
            state = 0;

        j = 0;
        do {
            buf[j++] = c = env_get_char(i++);
            if (j == sizeof(buf) - 1) {
                if (state <= 1)
                    lprintf(buf);
                j = 0;
            }
        } while (c != '\0');

        if (state <= 1) {
            if (j)
                lprintf(buf);
            lprintf("\n");
        }
    }

    if (state == 0)
        i = 0;
    return i;
}
