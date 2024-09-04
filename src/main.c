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
void cam_deinit();
void cam_read_frame(int);
void cam_read_line(int);
void set_xclk(uint32_t fct);
void cam_set_rfn(u32 irn, u32 ifn);
uint8_t cam_r_reg(uint8_t addr);
void cam_save_1_frame(u32 only_uart_dump);
int cam_w_reg(uint8_t addr, uint8_t data);
int loop_stop=0;
int main()
{
    uint32_t fnn=0;
    uint32_t end_loop=50;
    char file_name[32];
    char stopreason[64];
    main_init();
    cam_deinit();
    if(adc_test()){
        slprintf(stopreason, "%s\n", "Battery low, power off");
        lprintf_time(stopreason);
        power_off();
    }
    run_cmd_interface();
    task_log(NULL);
    lprintf_time("start working loop.\n");
    cam_init(7);
    while(!loop_stop){
        slprintf(file_name, "YUV%d", fnn);
        if(fnn%20==0){
            lprintf_time("%s\n", get_rtc_time(NULL));
        }
        lprintf_time("open file %s.bin\n", file_name);
        if(FS_OK==open_file_for_write(file_name, "BIN")){
            cam_save_1_frame(0);
            close_file();
            lprintf_time("\n===============file %s.bin done\n", file_name);
        }
        else{
            lprintf_time("open file fail:%s.BIN\n", file_name);
            if(fnn<MIN_YUV_FILES_NUM){
                slprintf(stopreason, "Too less files=%d\n", fnn);
                lprintf_time(stopreason);
                break;
            }
            else{
                lprintf_time("end of files, restart from YUV0.bin\n");
                fnn=0;
            }
        }
        fnn++;
        if(adc_test()){
            slprintf(stopreason, "%s\n", "Battery low, power off");
            power_off();
            break;
        }
        //task log
        task_log(NULL);
    }
    cam_deinit();
    lprintf_time("end working loop.\n");
    task_log(NULL);
    while(end_loop--){
        lprintf(stopreason);
        run_cmd_interface();
    }
    lprintf_time("Shut down\n");
    power_off();
    while(1);
    return 0;
}
