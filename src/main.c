#include <stdio.h>
#include <string.h>
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"
#include "mock_uart.h"
#include "commctr.h"

#define SVR_MAX_EMPTYLOOP 100u
#define MRXBF_SIZE 128
#define FRAME_INTV 100
char mrx_bf[MRXBF_SIZE];
char m_value[ENV_MAX_VALUE_LEN];
const char default_token[]="88888888999999992222222255555555";
char token[33]={0};
unsigned int state=REQ_INFO;
mupk * mkp=(mupk*)mrx_bf;
unsigned int empty_loops=0;
#ifdef SVR
#else
char svr_info[25]={0};
#endif
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
    //lprintf("\n");
    for(int i=0;i<32;i++){
        c=asc_random();
        if(c==' ')c=' '+1;
        if(c=='=')c='='+1;
        out[i]=c;
        //lprintf("%c ", c);
    }
    //lprintf("\n");
}

unsigned char getsum(int len)
{
    unsigned char sum=0;
    for(int zi=0;zi<len;zi++){
        sum+=(unsigned char)mrx_bf[zi];
    }
    return sum;
}

void check_send()
{
    int len=sizeof(mupk)+mkp->len;
    mrx_bf[len]=0;
    mrx_bf[len+1]=getsum(len);
    //lprintf("----send@%d:\r\n", g_ms_count);
    //mem_print(mrx_bf, 0, len+2);
    mock_uart_sends((char*)mkp, len+2);
}
int checked_recv()
{
    int len;
    if(0!=mock_uart_rx(mrx_bf, MRXBF_SIZE-1, FRAME_INTV)){
        empty_loops=0;
#ifdef SVR
        if(mkp->reqrsp<REQ_INFO||mkp->reqrsp>REQ_PWN){
            return 0;
        }
#else
        if(mkp->reqrsp!=RSP_ACK&&mkp->reqrsp!=RSP_NACK){
            return 0;
        }
#endif
        if(mkp->len>=MRXBF_SIZE){
            return 0;
        }
        len=sizeof(mupk)+mkp->len;
        //lprintf("----@%d-Got:reqrsp is %x, len %d\n", g_ms_count,
                //mkp->reqrsp, mkp->len);
        //mem_print(mrx_bf, 0, len+2);
        if(getsum(len)==mrx_bf[len+1] && 0==mrx_bf[len]){
            return 1;
        }
        else{
            return 0;
        }
    }
    return 0;
}
void poweroff(char *p);
int main()
{
    int stop=0;
    main_init();
    while(!stop){
        toggle_led(0);
        lmemset(mrx_bf, 0, MRXBF_SIZE);

#ifdef SVR

        if(checked_recv()){
            if(mkp->reqrsp>state){
                lprintf("state error\n");
                continue;
            }
            if(mkp->reqrsp==REQ_INFO){
                lmemset(mrx_bf, 0, MRXBF_SIZE);
                mkp->reqrsp=RSP_ACK;
                mkp->len=24;
                slprintf(mkp->data, "%X%X%X",
                        device_serial0, device_serial1, device_serial2);
                state=REQ_ACCESS;
            }
            else if(mkp->reqrsp==REQ_ACCESS){
                if(ENV_FAIL == get_env("token", m_value)){
                    lprintf("get_env fail, new one, use default\n");
                    strcpy(m_value, default_token);
                }
                //lprintf("token=%s\n", m_value);
                if(!strcmp(m_value, mkp->data)){
                    lprintf("token match!\n");
                    mkp->reqrsp=RSP_ACK;
                    mkp->len=4;
                    strcpy(mkp->data, "pass");
                    token[0]=0;
                    state=REQ_UPDATE;
                }
                else{
                    lprintf("token wrong!\n");
                    mkp->reqrsp=RSP_NACK;
                    mkp->len=3;
                    strcpy(mkp->data, "err");
                    delay_ms(1000);
                }
            }
            else if(mkp->reqrsp==REQ_UPDATE){
                prt_dec(strlen(token));
                //lprintf("token=%s\n", token);
                if(0==strlen(token)){
                    strcpy(token, mkp->data);
                }
                else{
                    if(!strcmp(token, mkp->data)){
                        if(ENV_FAIL == set_env("token", token)){
                            lprintf("set_env fail, fatal\n");
                            while(1);
                        }
                        mkp->reqrsp=RSP_ACK;
                        mkp->len=4;
                        strcpy(mkp->data, "pass");
                        state=REQ_PWN;
                    }
                    else{
                        mkp->reqrsp=RSP_NACK;
                        mkp->len=3;
                        strcpy(mkp->data, "err");
                        token[0]=0;
                    }
                }
            }
            else if(mkp->reqrsp==REQ_PWN){
                mkp->reqrsp=RSP_ACK;
                mkp->len=4;
                strcpy(mkp->data, "pass");
            }
            delay_ms(FRAME_INTV/2);
            check_send();
        }

#else//client
        if(state==REQ_INFO){
            lprintf("req info\n");
            mkp->len=5;
            strcpy(mkp->data, "hello");
            token[0]=0;
        }
        if(state==REQ_ACCESS){
            if(ENV_FAIL == get_env(svr_info, m_value)){
                lprintf("get_env fail, new one, use default\n");
                strcpy(m_value, default_token);
            }
            lprintf("req acc\n");
            //lprintf("token=%s\n", m_value);
            mkp->len=strlen(m_value);
            strcpy(mkp->data, m_value);
        }
        if(state==REQ_UPDATE){
            lprintf("req upd\n");
            prt_dec(strlen(token));
            if(strlen(token)!=32){
                generate_token(token);
            }
            //lprintf("token=%s\n", token);
            mkp->len=strlen(token);
            strcpy(mkp->data, token);
        }
        if(state==REQ_PWN){
            lprintf("req pwn\n");
            mkp->len=3;
            strcpy(mkp->data, "bye");
        }
        mkp->reqrsp=state;
        check_send();

        if(checked_recv()){
            if(mkp->reqrsp==RSP_ACK){
                if(state==REQ_INFO){
                    if(strlen(svr_info)==0){
                        strcpy(svr_info, mkp->data);
                    }
                    else{
                        if(!strcmp(svr_info, mkp->data)){
                            state=REQ_ACCESS;
                            token[0]=0;
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
                    if(ENV_FAIL == set_env(svr_info, token)){
                        lprintf("set_env fail, fatal\n");
                        while(1);
                    }
                    state=REQ_PWN;
                }
                else if(state==REQ_PWN){
                    stop=1;
                }
            }
        }
#endif
        else{
            lprintf("Empty %d\n", empty_loops);
            if(empty_loops++>SVR_MAX_EMPTYLOOP){
                lprintf("max empty loops reached\n");
                stop=1;
            }
        }
    }
    lprintf("end\n");
    poweroff("standby");
    lprintf("standby failed..\n");
    while(1);
    return 0;
}
