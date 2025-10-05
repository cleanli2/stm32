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
        tmpc = env_get_char(FM_ENV_STORE_SIZE-1);
        //lprintf("last byte=0x%b\n", tmpc);
        if(tmpc==0xff){
            //will use help env
            lprintf("hlep env %X\n", FM_ENV_STORE_START_ADDR);
            set_cur_env_area(USE_HELP_ENV);
        }
        //lprintf("env use %X\n", env_start_addr);
    }
    return env_start_addr;
}

int erase_env_area()
{
    uint8_t r;
    int ct = FM_SECTORS_PER_ENV_BLOCK;
    uint32_t i=0;
    uint32_t sector_addr = FM_GET_SECTOR_ADDR(get_env_start_addr());
    FLASH_Unlock();
    while(ct--){
        //lprintf("env_erase:sector %x\n", sector_addr);
        if(FLASH_COMPLETE != FLASH_ErasePage(sector_addr)){
            lprintf("env_erase:sector 0x%x fail!\n", sector_addr);
            FLASH_Lock();
            return ENV_FAIL;
        }
        sector_addr+=FM_FLASH_SECTOR_SIZE;
    }
    FLASH_Lock();
    ct = FM_ENV_STORE_SIZE;
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
        lprintf("env>main %X\n", FM_ENV_STORE_START_ADDR);
        env_start_addr=FM_ENV_STORE_START_ADDR;
    }
    else if(env_area == USE_HELP_ENV){
        lprintf("env>help %X\n", FM_ENV_HELP_STORE_START_ADDR);
        env_start_addr=FM_ENV_HELP_STORE_START_ADDR;
    }
}

int get_idle_env_area()
{
    if( get_env_start_addr()==FM_ENV_STORE_START_ADDR){
        //lprintf("curenv is main %X\n", FM_ENV_STORE_START_ADDR);
        return USE_HELP_ENV;
    }
    else if(get_env_start_addr()==FM_ENV_HELP_STORE_START_ADDR){
        //lprintf("curenv is help %X\n", FM_ENV_HELP_STORE_START_ADDR);
        return USE_MAIN_ENV;
    }
    return ENV_INVALID;
}
int get_cur_env_area()
{
    if(get_env_start_addr()==FM_ENV_STORE_START_ADDR){
        //lprintf("cur env is main %X\n", FM_ENV_STORE_START_ADDR);
        return USE_MAIN_ENV;
    }
    else if(get_env_start_addr()==FM_ENV_HELP_STORE_START_ADDR){
        //lprintf("cur env is help %X\n", FM_ENV_HELP_STORE_START_ADDR);
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
    uint8_t*tbp=(uint8_t*)(get_env_start_addr()+offset);
    if((uint32_t)tbp>=HW_FLASH_END_FORBIDEN){
        lprintf("forbidden:%x\r\n", tbp);
        while(1);
    }
    return *tbp;
}

int env_set_2char(uint32_t offset, const char*dp)
{
    uint32_t faddr;
    const uint16_t*wdp=(const uint16_t*)dp;
    faddr=offset+get_env_start_addr();
    //lprintf("fphw:%x=%b %b\r\n", faddr, dp[1],dp[0]);
    FLASH_Unlock();
    if(FLASH_COMPLETE != FLASH_ProgramHalfWord(faddr, *wdp)){
        lprintf("halfword fail!addr=%x\r\n", faddr);
        FLASH_Lock();
        return ENV_FAIL;
    }
    else{
        //lprintf("flash ok\r\n");
    }
    FLASH_Lock();
    //lprintf("read %x from %x\r\n", *(uint16_t*)faddr, faddr);
    return ENV_OK;
}

uint32_t strcpy2mem(uint8_t *s, uint32_t env_offset)
{
    uint32_t len = 0;
    uint8_t c;
    while((c = env_get_char(env_offset))){
        //lprintf("c=%b off=%x\r\n", c, env_offset);
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
    uint32_t i_down=0, i_up=FM_ENV_STORE_SIZE-1, i;
    if(env_get_char(i_down) != 0xff){
        lprintf("env full\n");
        return ENV_INVALID;
    }
    if(env_get_char(i_up) == 0xff){
        lprintf("empty env\n");
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
        lprintf("env_store_start %x size %x\n", get_env_start_addr(), FM_ENV_STORE_SIZE);
        return ENV_FAIL;
    }
    return i_up;
#if 0
    uint32_t i = 0, ff_i = 0;
    //go through not 0xff
    if(env_get_char(i) != 0xff){
        while(env_get_char(i) != 0xff){
            i++;
            if(i == FM_ENV_STORE_SIZE){
                lprintf("env data is full\n");
                return ENV_INVALID;
            }
        }
        if(env_get_char(i-1) != 0){
            lprintf("00FF g env flash error %x\n", i-2);
            lprintf("env_store_start %x size %x\n", get_env_start_addr(), FM_ENV_STORE_SIZE);
            return ENV_FAIL;
        }
    }

    //go through 0xff
    while(env_get_char(i) == 0xff){
        i++;
        ff_i++;
        if(i == FM_ENV_STORE_SIZE){//new ENV BLOCK, all 0xff
            lprintf("end of env store is 0xff\n");
            if(i == FM_ENV_STORE_SIZE){
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
    }
    ret = find_env_data_start_raw();
    if(ret > ENV_ABNORMAL && ret != ENV_EMPTY_DATA){
        ret = find_env_data_start_raw();
    }
    env_data_start_i = ret;
    lprintf("env used %d%\n", (FM_ENV_STORE_SIZE - ret)*100/FM_ENV_STORE_SIZE);
    return ret;
}

uint32_t get_env_raw(const char* name, char*value, uint32_t * p_position)
{
    uint32_t i = 0, nxt, ret = ENV_FAIL;
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
            if (nxt >= FM_ENV_STORE_SIZE) {
                ret = ENV_FAIL;
                goto end;
            }
        }
        if(env_get_char(nxt+1)=='\0')nxt++;
        if ((val=envmatch((uint8_t *)name, i)) < 0){
            if(nxt==FM_ENV_STORE_SIZE-1){
                ret = ENV_FAIL;
                goto end;
            }
            continue;
        }
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

int fm_save(uint32_t i, const char*name, const char*value)
{
    unsigned int len, ci;
    char tbt[2]={0};
    //lprintf("fmsave:%s=%s\r\n", name, value);
    len = strlen(name)+strlen(value)+2;
    if(len&1){
        len+=1;
    }
    if(i<len){
        lprintf("env full\n");
        lprintf("--enverr%d\n",__LINE__);
        return ENV_FULL;
    }
    i -= len;
    len=strlen(name);
    ci=len;
    if(ci==0){
        return ENV_FAIL;
    }
    tbt[1]=name[len-ci--];
    tbt[0]='\0';
    if(ENV_FAIL==env_set_2char(i, tbt)){
        return ENV_FAIL;
    }
    i+=2;
    while(ci){
        tbt[0]=name[len-ci--];
        if(ci==0){
            tbt[1]='=';
        }
        else{
            tbt[1]=name[len-ci--];
        }
        if(ENV_FAIL==env_set_2char(i, tbt)){
            return ENV_FAIL;
        }
        i+=2;
    }
    len=strlen(value);
    ci=len;
    if(len==0){
        if(tbt[1]=='='){
            return ENV_OK;
        }
        else{
            tbt[1]='=';
            tbt[0]='\0';
            if(ENV_FAIL==env_set_2char(i, tbt)){
                return ENV_FAIL;
            }
            else{
                return ENV_OK;
            }
        }
    }
    else{
        if(tbt[1]!='='){
            tbt[0]='=';
            tbt[1]=value[len-ci--];
            if(ENV_FAIL==env_set_2char(i, tbt)){
                return ENV_FAIL;
            }
            i+=2;
        }
        while(ci){
            tbt[0]=value[len-ci--];
            if(ci==0){
                tbt[1]='\0';
            }
            else{
                tbt[1]=value[len-ci--];
            }
            if(ENV_FAIL==env_set_2char(i, tbt)){
                return ENV_FAIL;
            }
            i+=2;
        }
    }
    return ENV_OK;
}

uint32_t set_env_raw(const char* name, const char*value)
{
    uint32_t i = 0, ret = ENV_OK;
    uint8_t zero_str = 0;
    uint16_t wd=0;

    if(!name){
        lprintf("name=NULL\n");
        ret = ENV_FAIL;
        //lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    if(!value){
        value = (const char*)&zero_str;
    }
    if(strchr(name, '=')!=NULL || strchr(value, '=')){
        lprintf("'=' can't be in name or value\n");
        ret = ENV_FAIL;
        //lprintf("--enverr%d\n",__LINE__);
        goto end;
    }

    if(strlen(name)>ENV_MAX_VALUE_LEN-1 ||
            strlen(value)>ENV_MAX_VALUE_LEN-1){
        lprintf("len cant>%d\n", ENV_MAX_VALUE_LEN-1);
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
        i = FM_ENV_STORE_SIZE - 2;
        env_set_2char(i, (const char*)&wd);
    }
    else if(i > ENV_ABNORMAL){
        ret = ENV_FAIL;
        lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    return fm_save(i, name, value);

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

char buf[ENV_MAX_VALUE_LEN*2+2];
/************************************************************************
 * Command interface: print one or all environment variables
 */
int go_through_env(int operation)
{
    uint32_t i, ret=ENV_OK, posi_name, posi;
    char *name, *value, *posi_eq;

    i = 0;
    memset(buf, 0, ENV_MAX_VALUE_LEN*2+2);
    get_cur_env_area();
    //lprintf("env_store_start %x size %x\n\n", get_env_start_addr(), FM_ENV_STORE_SIZE);

    i = find_env_data_start();
    if(i > ENV_ABNORMAL){
        ret = ENV_FAIL;
        //lprintf("--enverr%d\n",__LINE__);
        goto end;
    }
    i++;
    while(env_get_char(i) != '\0'){
        posi = i;
        i+=strcpy2mem((uint8_t*)buf, i);
        i++;
        if(env_get_char(i)=='\0')i++;
        if(PRINT_RAW_ENV==operation){
            lprintf("%x:", posi);
            //mem_print(buf, 0, 64);
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
                                lprintf("switch env fail\n");
                                return ret;
                            }
                        }
                    }
                }
            }
        }
        if(i>=FM_ENV_STORE_SIZE){
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
        lprintf("env copied, erase cur\n");
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
        //lprintf("envgetint %s=%x\n", name, ret);
        return ret;
    }
    else{
        //lprintf("envgetint %s fail, use default\n", name);
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
#if 0
    while(1)
    {
        if(0xff==XSPI_Flash_Read_Byte(flash_log_write_addr))
        {
            return;
        }
        flash_log_write_addr++;
        if(SPI_FLASH_LOG_END <= flash_log_write_addr){
            lprintf("full of log, erase first sector to start\n");
            flash_log_write_addr = SPI_FLASH_LOG_START;
            XSPI_Flash_Erase_Sector(flash_log_write_addr/SPI_FLASH_SECTOR_SIZE);//erase sector
            return;
        }
    }
#endif
}
