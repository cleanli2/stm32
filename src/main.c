#include <stdio.h>
#include <string.h>
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"

struct task all_tasks[]=
{
    {
        task_ui,
    },
    {
        task_timer,
    },
    {
        task_disp,
    },
    {
        task_music,
    },
#ifdef POWER_MONITOR
    {
        task_power,
    },
#endif
#ifndef ALIENTEK_MINI
    {
        task_lcd_bklight,
    },
#endif
    {
        task_misc,
    },
    {
        task_log,
    },
};
uint32_t task_mask = 0;
int main()
{
    int retry=9;
    main_init();
    while(get_env_uint("elocksts", 0)!=0x900d){
        pcf8574t_set(15, 1);
        pcf8574t_set(14, 1);
        pcf8574t_set(13, 1);
        pcf8574t_set(12, 1);
        lprintf("env elocksts not set!!!\r\n");
        run_cmd_interface();
        pcf8574t_set(15, 0);
        pcf8574t_set(14, 0);
        pcf8574t_set(13, 0);
        pcf8574t_set(12, 0);
        if(retry==0)power_off();
        retry--;
    }
    ui_start();
    while(1){
        for(unsigned int i = 0; i<sizeof(all_tasks)/sizeof(struct task); i++){
            if(!((1<<i)&task_mask)){
                all_tasks[i].t_func(&all_tasks[i]);
            }
        }
    }
    return 0;
}
