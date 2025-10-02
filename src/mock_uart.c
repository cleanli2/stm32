#include "mock_uart.h"

void mock_uart_init()
{
}

void mock_uart_tx(char da)
{
    int nbit=8;

    //start bit
    TX_BIT(0);
    delay_us(BIT_DELAY);

    while(nbit--){
        TX_BIT(da&0x01);
        delay_us(BIT_DELAY);
        da>>=1;
    }

    //stop bit
    TX_BIT(1);
    delay_us(BIT_DELAY);
}


int mock_uart_rx(char*cp, int maxn, int timeout_ms)
{
    int nrv=0, nbit=8;
    char da=0;

    time_start();

    while(nrv<maxn){
        da=0;
        nbit=8;

        //start bit detect
        while(1){
            if(RX_BIT()==0)break;
            if(time_passed_ms()>timeout_ms)break;
        }
        if(RX_BIT()==1)return nrv;

        while(nbit--){
            delay_us(BIT_DELAY);
            if(RX_BIT())da+=0x80;
            da>>=1;
        }

        //stop bit detect
        delay_us(BIT_DELAY);
        if(RX_BIT()){
            cp[nrv++]=da;
            time_start();
        }
    }
    return nrv;
}
