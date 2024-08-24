#include <stdio.h>
#include <string.h>
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"
#include "fs.h"

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
#ifndef ALIENTEK_MINI
    {
        task_power,
    },
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

#define MIN_YUV_FILES_NUM 100
void cam_init(int);
void cam_read_frame(int);
void cam_read_line(int);
void set_xclk(uint32_t fct);
void cam_set_rfn(u32 irn, u32 ifn);
uint8_t cam_r_reg(uint8_t addr);
void cam_save_1_frame(u32 only_uart_dump);
int cam_w_reg(uint8_t addr, uint8_t data);
int main()
{
    uint32_t fnn=0;
    char file_name[32];
    char stopreason[64];
    main_init();
    run_cmd_interface();
    cam_init(7);
    while(1){
        slprintf(file_name, "YUV%d", fnn);
        lprintf("save frame to file %s.bin\n", file_name);
        if(FS_OK==open_file_for_write(file_name, "BIN")){
            cam_save_1_frame(0);
            close_file();
            lprintf("\n===============save frame to file %s.bin done\n", file_name);
        }
        else{
            lprintf("open file fail:%s.BIN\n", file_name);
            if(fnn<MIN_YUV_FILES_NUM){
                slprintf(stopreason, "%s\n", "Too less files=%d", fnn);
                break;
            }
            else{
                lprintf("end of files, restart from YUV0.bin\n");
                fnn=0;
            }
        }
        fnn++;
        if(adc_test()){
            slprintf(stopreason, "%s\n", "Battery low, power off");
            power_off();
            break;
        }
    }
    while(1){
        lprintf(stopreason);
        delay_ms(1000);
    }
    return 0;
}
