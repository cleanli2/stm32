#include "eg.h"
#include "common.h"

/*init value
enum egidx{ EG_LED0, EG_OUTEN, EG_POFF, EG_CAMRST, EG_SDPWR, EG_LCDRST, EG_KEY1, EG_KEY2 };
enum egidx{       0,        0,       1,         1,        0,         1,       1,       1 };
 * */
static uint8_t eg_cache[1]={0xec};
void eg_init()
{
    eg_cache[0] = 0xec;
    pcf8574t_writeData(EG_ADDR, eg_cache[0]);
}
void  eg_set(int bit, int v)
{
    int index=bit/8;
    if(index==1)bit-=8;
    if(v){//set 1
        eg_cache[index]|=1<<bit;
    }
    else{
        eg_cache[index]&=~(1<<bit);
    }
    pcf8574t_writeData(EG_ADDR+index, eg_cache[index]);
}
void  eg_revert(int bit)
{
    int v;
    int index=bit/8;
    if(index==1)bit-=8;
    v=!(eg_cache[index]&(1<<bit));
    if(v){//set 1
        eg_cache[index]|=1<<bit;
    }
    else{
        eg_cache[index]&=~(1<<bit);
    }
    pcf8574t_writeData(EG_ADDR+index, eg_cache[index]);
}
int eg_get(int bit)
{
    u8 tv=1;
    int index=bit/8;
    if(index==1)bit-=8;
    pcf8574t_readData1(EG_ADDR+index, &tv);
    //lprintf("read i=%d = %x\r\n", index, tv);
    tv>>=bit;
    return tv&0x1;
}
