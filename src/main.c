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
        }

#else//client
        mkp->reqrsp=REQ_INFO;
        mkp->len=5;
        strcpy(mkp->data, "hello");
        lprintf("sending req...\n");
        mock_uart_sends((char*)mkp, sizeof(mupk)+mkp->len);

        lprintf("waiting response...\n");
        if(0!=mock_uart_rx(mrx_bf, MRXBF_SIZE-1, 2000)){
            lprintf("Got:reqrsp is %x, len %d\n", mkp->reqrsp, mkp->len);
            lprintf("str=%s\n", mkp->data);
        }
#endif
    }
    return 0;
}
