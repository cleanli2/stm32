#include "mock_uart.h"
#include "common.h"

/*********************HW related start**************************/
#define TX_BIT(D) {\
    if(D){\
        MOCK_UART_GP->BSRR = MOCK_UART_PIN;\
    }\
    else{\
        MOCK_UART_GP->BRR = MOCK_UART_PIN;\
    }\
}
#define RX_BIT() (!!(MOCK_UART_GP->IDR&MOCK_UART_PIN))

void hw_rx_init(int flag)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(MOCK_UART_PERIPH, ENABLE);

    GPIO_InitStructure.GPIO_Pin = MOCK_UART_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(flag){//rx
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    }
    else{//tx
        GPIO_SetBits(MOCK_UART_GP, MOCK_UART_PIN);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    }
    GPIO_Init(MOCK_UART_GP, &GPIO_InitStructure);
}

void time_start()
{
    dt_us_last();
}
uint32_t time_passed_ms()
{
    return dt_us_last()/1000;
}
/*********************HW related end  **************************/

static unsigned int BIT_DELAY=10000;

void mock_uart_init()
{
    hw_rx_init(1);
    BIT_DELAY=1000000u/MOCK_UART_BAUTRATE;
    prt_dec(BIT_DELAY);
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

void mock_uart_sends(char*da, int len)
{
    hw_rx_init(0);
    while(len--){
        mock_uart_tx(*da++);
    }
    hw_rx_init(1);
}

int mock_uart_rx(char*cp, int maxn, unsigned int timeout_ms)
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

        delay_us(BIT_DELAY/8);

        while(nbit--){
            delay_us(BIT_DELAY);
            da>>=1;
            if(RX_BIT())da+=0x80;
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
