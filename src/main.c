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
    {
        task_power,
    },
    {
        task_lcd_bklight,
    },
    {
        task_misc,
    },
};

void main()
{
    main_init();
    ui_init();
    while(1){
        for(int i = 0; i<sizeof(all_tasks)/sizeof(struct task); i++){
            all_tasks[i].t_func(&all_tasks[i]);
        }
    }
}
