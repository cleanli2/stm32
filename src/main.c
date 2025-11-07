#include <stdio.h>
#include <string.h>
#include "sha256.h"
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"
#include "mock_uart.h"
#include "commctr.h"

#define MRXBF_SIZE 128
#define FRAME_INTV 50
char mrx_bf[MRXBF_SIZE];
char m_value[ENV_MAX_VALUE_LEN];
const char default_token[]="88888888999999992222222255555555";
char token[33]={0};
unsigned int state=REQ_INFO;
mupk * mkp=(mupk*)mrx_bf;
unsigned int empty_loops=0;
#define MAX_EMPTYLOOP 2u
#define MOS_OPEN() GPIO_SetBits(MOS_GP,MOS_PIN);
#define MOS_CLOSE() GPIO_ResetBits(MOS_GP,MOS_PIN);
#define WAITLMT 100
char sha_token[ENV_MAX_VALUE_LEN];

void poweroff(char *p);
int main()
{
    int wt=0;
    main_init();
    if (BKP_ReadBackupRegister(BKP_DR2) != 0x2031){
        MOS_OPEN();
    }
    prt_hex(GPIO_ReadInputDataBit(LOCKPOSI_GP, LOCKPOSI_PIN));
    prt_hex(g_lockposi);
    prt_hex(LOCKPOSI_LOCKED());
    while(!LOCKPOSI_LOCKED()){
        if(wt++>WAITLMT){
            MOS_OPEN();
        }
        prt_hex(GPIO_ReadInputDataBit(LOCKPOSI_GP, LOCKPOSI_PIN));
        prt_hex(g_lockposi);
        prt_hex(LOCKPOSI_LOCKED());
        delay_ms(1000);
    }
    mcu_printer(RTC_Get());
    mcu_printer(" off ");
    slprintf(mrx_bf, "%dms", g_ms_count);
    mcu_printer(mrx_bf);
    mcu_printer("\r\n");
    set_env("close", RTC_Get());
    mcu_printer("-~-~-~-~-~-~-~-~-~-~-~-~-~-~\r\n");
    if (BKP_ReadBackupRegister(BKP_DR2) != 0x2031){
        MOS_OPEN();
        wt=10;
        while(wt--){
            lprintf("alert:no 5v power\n");
            delay_ms(1000);
        }
    }
    lprintf("end %s\n", mrx_bf);
    poweroff("standby");
    lprintf("standby failed..\n");
    while(1);
    return 0;
}
