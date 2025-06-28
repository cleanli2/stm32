#ifndef __EG__H
#define __EG__H


enum egidx{
    EG_LED0,
    EG_OUTEN,
    EG_POFF,
    EG_CAMRST,
    EG_SDPWR,
    EG_LCDRST,
    EG_KEY1,
    EG_KEY2
};

void eg_init();
void  eg_set(int bit, int v);
void  eg_revert(int bit);
int eg_get(int bit);

#endif
