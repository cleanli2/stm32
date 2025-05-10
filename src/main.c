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

int restarted=0;
extern char fbf[512];
uint32_t g_fnn=0;
void prepare_pic_trsf()
{
    uint32_t n_s_fnn=0;
    char t_file_name[32];
    uint32_t t_fnn = get_env_uint("fsno", 0);
    memset(fbf, ' ', 512);
    if(restarted && t_fnn < g_fnn){
        n_s_fnn = g_fnn+1;
        slprintf(t_file_name, "V%d/YUV%d.BIN", n_s_fnn/100, n_s_fnn);
        if(FS_OK!=open_file_w(t_file_name)){
            lprintf_time("%s not exist\n", t_file_name);
            n_s_fnn = 0;
        }
        close_file();
        if(FS_OK==open_file_w("STAFN.TXT")){
            slprintf(fbf, "%d        ", n_s_fnn);
            write_sec_to_file((const char*)fbf);
            close_file();
            lprintf_time("\nFiles looped, update stafn.txt=%s done\n", fbf);
        }
    }
    if(FS_OK==open_file_w("ENDFN.TXT")){
        slprintf(fbf, "%d        ", g_fnn);
        write_sec_to_file((const char*)fbf);
        close_file();
        lprintf_time("\nendfn.txt=%s done\n", fbf);
    }
    if(FS_OK==open_file_w("DTIDN.TXT")){
        slprintf(fbf, "%s", get_rtc_time(NULL));
        slprintf(fbf+10, "_%X_", *(u32*)(0x1ffff7f0));
        write_sec_to_file((const char*)fbf);
        close_file();
        lprintf_time("\ndtidn.txt=%s done\n", fbf);
    }
}

#define MIN_YUV_FILES_NUM 100
void save_sd_log();
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
int cam_workingloop_on=0;
int cam_workloop_stucked=0;
int stuck_gfnn[RECORD_CAM_STUCK_SIZE]={0};
void file_to_lcd();

#define CAMBT_X 660
#define CAMBT_Y 80
#define CAMBT_W 50
#define CAMBT_H 50
#define CAMBT_I 20
char ss;
char camss_table[]={'1', '2', 'q', 'w', 'a', 's', 'z', 'x', '0'};
void cambt_detect(void*ip)
{
    int i = *(int*)ip;
    lprintf("cambt:%d\r\n", i);
    if((uint32_t)i>sizeof(camss_table))return;
    ss=camss_table[i];
}
button_t camui_button[]={
    {CAMBT_X, CAMBT_Y, CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X+CAMBT_W+CAMBT_I, CAMBT_Y, CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X, CAMBT_Y+1*(CAMBT_H+CAMBT_I), CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X+CAMBT_W+CAMBT_I, CAMBT_Y+1*(CAMBT_H+CAMBT_I), CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X, CAMBT_Y+2*(CAMBT_H+CAMBT_I), CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X+CAMBT_W+CAMBT_I, CAMBT_Y+2*(CAMBT_H+CAMBT_I), CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X, CAMBT_Y+3*(CAMBT_H+CAMBT_I), CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X+CAMBT_W+CAMBT_I, CAMBT_Y+3*(CAMBT_H+CAMBT_I), CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {CAMBT_X, CAMBT_Y+4*(CAMBT_H+CAMBT_I), CAMBT_W,  CAMBT_H, cambt_detect, -1, 0, NULL, 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
};

void draw_button(button_t*pbt);
void process_button(ui_t* uif, button_t*pbt);
void bus_to_lcd(int mode_to_lcd);
void check_ui()
{
    int s_fnn=g_fnn;
    uint16_t touch_x, touch_y;
    uint16_t touch_status = 0, lastts=0;
    char fs[19];
    draw_button(camui_button);
    //int touch_up=0;
    while(s_fnn){
        lcd_lprintf(1, 645, 43, "sfnn=%d", s_fnn);
        slprintf(fs, "V%d/YUV%d.BIN", s_fnn/100, s_fnn);
        if(FS_OK==open_file_r(fs)){
            lprintf("open file ok\n");
            file_to_lcd();
            close_file();
            bus_to_lcd(1);
            lprintf("\n===============file %s to lcd done\n", fs);
            lprintf("Anykey quit\r\n");
        }
        else{
            lprintf("open file fail:%s\n", fs);
            return;
        }
        if(con_is_recved()){
            ss=con_recv();
        }
        lastts=touch_status;
        if(get_TP_point(&touch_x, &touch_y)){
            touch_status = 1;
        }
        else{
            touch_status = 0;
            if(lastts==1){
                //touch_up=1;
            }
            else{
                //touch_up=0;
            }
        }
        if(touch_status==1){
            lprintf("touch down %d %d\r\n", touch_x, touch_y);
            if(touch_x==1022 && touch_y==0){
                break;
            }
            process_button(NULL, camui_button);
        }
        switch(ss){
            case '1':
                s_fnn--;
                lcd_lprintf(1, 645, 5, "back 1 frame");
                break;
            case '2':
                s_fnn+=2;
                lcd_lprintf(1, 645, 5, "forward 1 frame");
                break;
            case 'q':
                s_fnn-=8;
                lcd_lprintf(1, 645, 5, "back 1 min");
                break;
            case 'w':
                s_fnn+=10;
                lcd_lprintf(1, 645, 5, "forward 1 min");
                break;
            case 'a':
                s_fnn-=77;
                lcd_lprintf(1, 645, 5, "back 10 min");
                break;
            case 's':
                s_fnn+=79;
                lcd_lprintf(1, 645, 5, "forward 10 min");
                break;
            case 'z':
                s_fnn-=462;
                lcd_lprintf(1, 645, 5, "back 1 hour");
                break;
            case 'x':
                s_fnn+=464;
                lcd_lprintf(1, 645, 5, "forward 1 hour");
                break;
            default:
                lcd_lprintf(1, 645, 5, "                 ");
        }
        if(ss=='0')break;
        ss=' ';
        s_fnn--;
        if(s_fnn<0){
            s_fnn+=9999;
        }
        if(s_fnn>9999){
            s_fnn-=10000;
        }
    }
    LCD_Clear(WHITE);	//fill all screen with some color
}
int main()
{
    uint32_t end_loop=50;
    char file_name[32];
    char stopreason[64];
    memset(stopreason, 0, 64);
    main_init();
    g_fnn = get_env_uint("fsno", 0);
    lprintf_time("start g_fnn=%d\n", g_fnn);
    auto_time_correct2();
    cam_deinit();
    if(adc_test()){
        slprintf(stopreason, "%s\n", "Battery low, power off");
        lprintf_time(stopreason);
        prepare_pic_trsf();
        power_off();
    }
    if(FS_OK==open_file_w("STAFN.TXT")){
        slprintf(fbf, "%d        ", g_fnn);
        write_sec_to_file((const char*)fbf);
        close_file();
        lprintf_time("\nstafn.txt=%s done\n", fbf);
    }
    lprintf_time("loginflash:log start:0x%x end:0x%x size:0x%x\n",
            SPI_FLASH_LOG_START, SPI_FLASH_LOG_END, SPI_FLASH_LOG_SIZE);
    task_log(NULL);
    run_cmd_interface();
    check_ui();
    lprintf_time("start working loop.\n");
    cam_init(7);
    cam_workingloop_on=1;
    while(!loop_stop){
        slprintf(file_name, "V%d/YUV%d.BIN", g_fnn/100, g_fnn);
        if(g_fnn%20==0){
            lprintf_time("%s\n", get_rtc_time(NULL));
            lprintf_time("Version %s%s\n", VERSION, GIT_SHA1);
            if(strncmp("2025", get_rtc_time(NULL), 4)){
                lprintf_time("RTC err:%s\n", get_rtc_time(NULL));
                slprintf(stopreason, "RTC err@%d\n", g_fnn);
                lprintf_time(stopreason);
                break;
            }
            auto_time_correct2();
        }
        lprintf_time("open %s\n", file_name);
        if(FS_OK==open_file_w(file_name)){
            cam_save_1_frame(0);
            close_file();
            lprintf_time("\n====file %s done\n", file_name);
        }
        else{
            lprintf_time("open file fail:%s\n", file_name);
            if(g_fnn<MIN_YUV_FILES_NUM){
                slprintf(stopreason, "Too less files=%d\n", g_fnn);
                lprintf_time(stopreason);
                break;
            }
            else{
                lprintf_time("end of files, restart from YUV0.bin\n");
                restarted=1;
                g_fnn=0;
            }
        }
        g_fnn++;
        if(adc_test()){
            slprintf(stopreason, "%s\n", "Battery low, power off");
            loop_stop=1;
            break;
        }
        //task log
        task_log(NULL);
    }
    cam_workingloop_on=0;
    cam_deinit();
    lprintf_time("end working loop.\n");
    prepare_pic_trsf();
    task_log(NULL);
    if(loop_stop){
        save_sd_log();
        power_off();
    }
    while(end_loop--){
        lprintf(stopreason);
        run_cmd_interface();
    }
    lprintf_time("Shut down\n");
    power_off();
    while(1);
    return 0;
}

#define FPM_LOW_LIMIT 4
//called every 1/2 minute
void cam_working_confirm()
{
    static uint32_t last_g_fnn=0;
    if(cam_workingloop_on){
        if((last_g_fnn+(FPM_LOW_LIMIT/2))>g_fnn){
            stuck_gfnn[cam_workloop_stucked%RECORD_CAM_STUCK_SIZE]=g_fnn;
            cam_workloop_stucked++;
        }
        last_g_fnn=g_fnn;
    }
}
