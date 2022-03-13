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
uint32_t task_mask = 0;
void main()
{
    main_init();
#ifdef WRITE_W25F
    while(1)
        run_cmd_interface();
#else
    ui_start();
    while(1){
        for(int i = 0; i<sizeof(all_tasks)/sizeof(struct task); i++){
            if(!((1<<i)&task_mask)){
                all_tasks[i].t_func(&all_tasks[i]);
            }
        }
    }
#endif
}
