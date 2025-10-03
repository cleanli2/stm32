#include <stdio.h>
#include <string.h>
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"
#include "mock_uart.h"

#define MRXBF_SIZE 128
char mrx_bf[MRXBF_SIZE];
int main()
{
    main_init();
    while(1){
        lmemset(mrx_bf, 0, MRXBF_SIZE);
        if(0!=mock_uart_rx(mrx_bf, MRXBF_SIZE-1, 2000)){
            lprintf("Got:%s\n", mrx_bf);
        }
        delay_ms(adc_random(0)*4);
        mock_uart_sends("hello", 5);
    }
    return 0;
}
