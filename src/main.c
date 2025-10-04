#include <stdio.h>
#include <string.h>
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"
#include "mock_uart.h"
#include "commctr.h"

#define MRXBF_SIZE 128
char mrx_bf[MRXBF_SIZE];
char m_value[ENV_MAX_VALUE_LEN];
char default_token[]="88888888999999992222222255555555";
char svr_info[25]={0};
#ifndef SVR
unsigned int state=REQ_INFO;
char asc_random()
{
    char ret;
    uint32_t val;
    delay_us(adc_random(0));
    val=SysTick->VAL;
    val/=95;
    ret=0x20+val;
    if(ret==0x7f)ret=0x7e;
    return ret;
}
void generate_token(char*out)
{
    char c;
    for(int i=0;i<32;i++){
        c=asc_random();
        if(c==' ')c=' '+1;
        if(c=='=')c='='+1;
        out[i++]=c;
    }
}
#endif
int main()
{
    mupk * mkp=(mupk*)mrx_bf;
    main_init();
    while(1){
        lmemset(mrx_bf, 0, MRXBF_SIZE);

#ifdef SVR

        lprintf("waiting req...\n");
        if(0!=mock_uart_rx(mrx_bf, MRXBF_SIZE-1, 2000)){
            lprintf("Got:reqrsp is %x, len %d\n", mkp->reqrsp, mkp->len);
            lprintf("str=%s\n", mkp->data);
            if(mkp->reqrsp==REQ_INFO){
                lmemset(mrx_bf, 0, MRXBF_SIZE);
                mkp->reqrsp=RSP_ACK;
                mkp->len=24;
                slprintf(mkp->data, "%X%X%X",
                        device_serial0, device_serial1, device_serial2);
                mock_uart_sends((char*)mkp, sizeof(mupk)+mkp->len);
            }
        }

#else//client
        if(state==REQ_INFO){
            lprintf("req info\n");
            mkp->len=5;
            strcpy(mkp->data, "hello");
        }
        if(state==REQ_ACCESS){
            if(ENV_FAIL == get_env(svr_info, m_value)){
                lprintf("get_env fail, new one, use default\n");
                strcpy(m_value, default_token);
            }
            lprintf("req acc\n");
            lprintf("token=%s\n", m_value);
            mkp->len=strlen(m_value);
            strcpy(mkp->data, m_value);
        }
        if(state==REQ_UPDATE){
            lprintf("req upd\n");
            generate_token(m_value);
            lprintf("token=%s\n", m_value);
            mkp->len=strlen(m_value);
            strcpy(mkp->data, m_value);
        }
        if(state==REQ_PWN){
            lprintf("req pwn\n");
            mkp->len=3;
            strcpy(mkp->data, "bye");
        }
        mkp->reqrsp=state;
        lprintf("sending req...\n");
        mock_uart_sends((char*)mkp, sizeof(mupk)+mkp->len);

        lprintf("waiting response...\n");
        if(0!=mock_uart_rx(mrx_bf, MRXBF_SIZE-1, 2000)){
            lprintf("Got:reqrsp is %x, len %d\n", mkp->reqrsp, mkp->len);
            lprintf("str=%s\n", mkp->data);
            if(mkp->reqrsp==RSP_ACK){
                if(state==REQ_INFO){
                    if(strlen(svr_info)==0){
                        strcpy(svr_info, mkp->data);
                    }
                    else{
                        if(!strcmp(svr_info, mkp->data)){
                            state=REQ_ACCESS;
                        }
                        else{
                            memset(svr_info, 0, 25);
                        }
                    }
                }
                else if(state==REQ_ACCESS){
                    state=REQ_UPDATE;
                }
                else if(state==REQ_UPDATE){
                    state=REQ_PWN;
                }
                else if(state==REQ_PWN){
                    lprintf("end\n");
                    while(1);
                }
            }
        }
#endif
    }
    return 0;
}
