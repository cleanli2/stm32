#include "display.h"
#include "common.h"

void os_task_display(void*p)
{
    (void)p;
    while(1){
        os_10ms_delay(500);
        lprintf("display task\n");
    }
}
