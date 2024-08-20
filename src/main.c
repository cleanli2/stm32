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
    main_init();
    run_cmd_interface();
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
