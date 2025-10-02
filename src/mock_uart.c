#include "mock_uart.h"
#include "common.h"

/*********************HW related start**************************/
#define TX_BIT(D) {\
    if(D){\
        MOCK_UART_TX_GP->BSRR = MOCK_UART_TX_PIN;\
    }\
    else{\
        MOCK_UART_TX_GP->BRR = MOCK_UART_TX_PIN;\
    }\
}
#define RX_BIT() (!!(MOCK_UART_RX_GP->IDR&MOCK_UART_RX_PIN))

void hw_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(MOCK_UART_TX_GP, ENABLE);
    RCC_APB2PeriphClockCmd(MOCK_UART_RX_GP, ENABLE);

    GPIO_InitStructure.GPIO_Pin = MOCK_UART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_SetBits(MOCK_UART_TX_GP, MOCK_UART_TX_PIN);
    GPIO_Init(MOCK_UART_TX_GP, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MOCK_UART_RX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(MOCK_UART_RX_GP, &GPIO_InitStructure);
}
/*********************HW related end  **************************/

static unsigned int BIT_DELAY=10000;

void mock_uart_init()
{
    hw_init();
    BIT_DELAY=1000000u/MOCK_UART_BAUTRATE;
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
