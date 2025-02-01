#include "common.h"
#include "fs.h"
#include "music.h"
#include "ui.h"
#include "pyinput.h"

#define POWER_INDICATOR_CYCLE 30
ui_t working_ui_t;
int cur_ui_index = 0;
int last_ui_index = 0;
ui_t*current_ui;
uint32_t ui_buf[8];
int prgb_color = BLACK;

static uint32_t power_indicator_counter = 0;
uint32_t power_max_display = BATT_MAX - BATT_LOW_LIMIT;
uint32_t power_display = 0;
prgb_t power_prgb[]={
    {185, 2, 40, 13, NULL, NULL, NULL, 0},
    {-1, -1,  -1,  -1, NULL, NULL, NULL, 0},
};

void PutPixel(int x, int y, int color);
void often_used_timer();
void f3mins_timer();
void common_process_event(void*vp);
void ui_transfer(uint8 ui_id);
void common_ui_init(void*vp);
void draw_prgb(prgb_t*pip);
void update_prgb(ui_t* uif, prgb_t*pip);
void draw_prgb_raw(prgb_t*pip);
void timer_ui_transfer_with_para(uint32_t timeout,
        uint32_t repeat, const int8_t* music);
void draw_button(button_t*pbt);
void draw_sq(int x1, int y1, int x2, int y2, int color);
void draw_sq2(int x1, int y1, int w, int h, int color);
void common_ui_uninit(void*vp);

void timer_set_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    common_ui_init(vp);
    ui_buf[8] = 1;
#ifdef LARGE_SCREEN
    lcd_lprintf(60, 280, "Repeat %d times   ", ui_buf[8]);
#else
    lcd_lprintf(60, 180, "Repeat %d times   ", ui_buf[8]);
#endif
}
void timer_set_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    common_process_event(vp);
}
void main_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    lprintf_time("mainmenu\n");
#ifdef RTC_8563
    if(check_rtc_alert_and_clear()){
        often_used_timer();
    }
#endif
    common_ui_init(vp);
}
void main_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    if(cur_task_event_flag & (1<<EVENT_NOKEYCT_MAXREACHED)){
        lprintf_time("evt:no keyinput timeout\n");
        ui_transfer(UI_POFF_CTD);
    }
    common_process_event(vp);
}
void poff_ctd_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    common_ui_init(vp);
    lcd_lprintf(20, 100, "Version:%s%s", VERSION, GIT_SHA1);
#ifdef RTC_8563
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, 20, 140);
    get_t_t_n_c(1);
#endif
}
void poff_ctd_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    if(cur_task_event_flag & (1<<EVENT_BATT_LOW)){
        LCD_PRINT_FRONT_COLOR = RED;
        set_LCD_Char_scale(3);
        lcd_lprintf(90, 600, "LOW POWER");
        LCD_PRINT_FRONT_COLOR = BLACK;
        set_LCD_Char_scale(1);
    }
    if(cur_task_event_flag & (1<<EVENT_MUSIC_PLAY_END)){
        power_off();
    }
    if(g_flag_1s){
        play_music_note(8, 100);
    }
    common_process_event(vp);
}

prgb_t timer_prgb[]={
#ifdef LARGE_SCREEN
    {20, 200, 440, 30, NULL, NULL, "Counter", 0},
    {20, 280, 440, 30, NULL, NULL, "Repeat", 0},
#else
    {10, 100, 200, 15, NULL, NULL, "Counter", 0},
    {10, 140, 200, 15, NULL, NULL, "Repeat", 0},
#endif
    {-1, -1,  -1,  -1, NULL, NULL, NULL, 0},
};

#define TMR_TMOUT_INDX 0
#define TMR_REPET_INDX 1
#define TMR_TMOUTCT_INDX 2
#define TMR_REPETCT_INDX 3
#define TMR_MAGIC_INDX 4
#define TMR_MAGIC 0xF1EE4u
void timer_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    play_music(notice_music, 0);
    if(ui_buf[TMR_MAGIC_INDX] != TMR_MAGIC){
        ui_buf[TMR_TMOUT_INDX] = uif->timeout;
        ui_buf[TMR_REPET_INDX] = 2;
        ui_buf[TMR_REPETCT_INDX] = 2;
        ui_buf[TMR_TMOUTCT_INDX] = ui_buf[TMR_TMOUT_INDX];
    }
    else{
        ui_buf[TMR_MAGIC_INDX] = 0;
    }
    timer_prgb[0].max = &ui_buf[TMR_TMOUT_INDX];
    timer_prgb[0].data = &ui_buf[TMR_TMOUTCT_INDX];
    timer_prgb[0].last_data = ui_buf[TMR_TMOUTCT_INDX]+2;
    timer_prgb[1].max = &ui_buf[TMR_REPET_INDX];
    timer_prgb[1].data = &ui_buf[TMR_REPETCT_INDX];
    timer_prgb[1].last_data = ui_buf[TMR_REPETCT_INDX]+2;
    common_ui_init(vp);
    update_prgb(uif, uif->prgb_info);
}
void timer_ui_process_event(void*vp)
{
    uint32_t mins, secs;
    ui_t* uif =(ui_t*)vp;
    if(cur_task_event_flag & (1<<EVENT_MUSIC_PLAY_END)
            && ui_buf[TMR_REPETCT_INDX]==0){
        lprintf_time("timer timeout\n");
        ui_transfer(UI_MAIN_MENU);
    }
    if(g_flag_1s){
#ifdef LARGE_SCREEN
        set_LCD_Char_scale(6);
#else
        set_LCD_Char_scale(2);
#endif
        mins = ui_buf[TMR_TMOUTCT_INDX]/60;
        secs = ui_buf[TMR_TMOUTCT_INDX] -  mins*60;
#ifdef LARGE_SCREEN
        lcd_lprintf(100, 400, "%d:%d   ", mins, secs);
        lcd_lprintf(100, 520, "RP:%d ", ui_buf[TMR_REPETCT_INDX]);
#else
        lcd_lprintf(60, 180, "%d:%d   ", mins, secs);
        lcd_lprintf(60, 220, "RP:%d ", ui_buf[TMR_REPETCT_INDX]);
#endif
        set_LCD_Char_scale(1);
        if(ui_buf[TMR_REPETCT_INDX]==0){
            update_prgb(uif, uif->prgb_info);
            if(uif->timeout_music){
                //lprintf_time("timeout_music, start play\n");
                play_music(uif->timeout_music, 0);
            }
        }
        else{
            //lcd_lprintf(50, 200, "Counts :%d  ", ui_buf[TMR_TMOUTCT_INDX]);
            //lcd_lprintf(50, 250, "Repeats:%d  ", ui_buf[TMR_REPETCT_INDX]);
            update_prgb(uif, uif->prgb_info);
            if(ui_buf[TMR_TMOUTCT_INDX] == 0){
                ui_buf[TMR_REPETCT_INDX]--;
                ui_buf[TMR_TMOUTCT_INDX] = ui_buf[TMR_TMOUT_INDX];
                draw_prgb_raw(&timer_prgb[0]);
                play_music(notice_music, 0);
                lprintf_time("play notice_music\n");
            }
            ui_buf[TMR_TMOUTCT_INDX]--;
        }
    }
    common_process_event(vp);
}
void timer_ui_uninit(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    ui_buf[TMR_MAGIC_INDX] = 0;
    lprintf("clr TMR_MAGIC_INDX\n");
}
button_t common_button[]={
#ifdef LARGE_SCREEN
    {10,730,200, 60, NULL, UI_LAST, 0, "RETURN", 0, return_cch_str},
    {270,730,200, 60, NULL, 0, 0, "HOME", 0, home_cch_str},
#else
    {10,295,60, 20, NULL, UI_LAST, 0, "RETURN", 0, return_cch_str},
    {170,295,60, 20, NULL, 0, 0, "HOME", 0, home_cch_str},
#endif
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
};

void music_test()
{
    static int music_index = 0;
    play_music(music_lst[music_index], 0);
    music_index = add_with_limit(music_index, 1, get_music_lst_size());
}

#ifdef LARGE_SCREEN
#define SOUND_STA_X 215
#define SOUND_STA_Y 730
#else
#define SOUND_STA_X 75
#define SOUND_STA_Y 295
#endif
#define SOUND_STA_SIZE 16
#define SOUND_STA_SMALL 10
void show_sound_status()
{
    int dt = (SOUND_STA_SIZE-SOUND_STA_SMALL)/2;
    lcd_clr_window(WHITE, SOUND_STA_X, SOUND_STA_Y, SOUND_STA_X+SOUND_STA_SIZE, SOUND_STA_Y+SOUND_STA_SIZE);
    draw_sq(SOUND_STA_X, SOUND_STA_Y+dt,
            SOUND_STA_X+SOUND_STA_SIZE/2, SOUND_STA_Y+SOUND_STA_SMALL+dt,0);
    draw_sq(SOUND_STA_X+SOUND_STA_SIZE/2, SOUND_STA_Y,
            SOUND_STA_X+SOUND_STA_SIZE, SOUND_STA_Y+SOUND_STA_SIZE,0);
    if(get_sound_sta()==0){
        LCD_DrawLine(SOUND_STA_X, SOUND_STA_Y, SOUND_STA_X+16, SOUND_STA_Y+16);
    }
}

void sound_ctrl()
{
    enable_sound(!get_sound_sta());
    show_sound_status();
}

button_t main_menu_button[]={
#ifdef LARGE_SCREEN
    {130,40,200, 60, often_used_timer, -1, 0, "OftenTimer", 0, often_used_timer_cch_str},
    {130,110,200, 60, NULL, UI_POFF_CTD, 0, "PowerOffCountDown", 0, poff_countdown_cch_str},
    {130,180,200, 60, soft_reset_system, -1, 0, "Reboot", 0, reboot_cch_str},
    {130,250,200, 60, reboot_download, -1, 0, "RebootDownload", 0, rebootd_cch_str},
    {130,320,200, 60, power_off, -1, 0, "PowerOff", 0, poff_cch_str},
    {130,390,200, 60, music_test, -1, 0, "MusicTest", 0, music_test_cch_str},
    {130,460,200, 60, f3mins_timer, -1, 0, "3x1mins TIMER", 0, _3x1mins_timer_cch_str},
    {130,530,200, 60, NULL, UI_TIMER_SET, 0, "More Timer", 0, more_timer_cch_str},
    {130,600,200, 60, NULL, UI_DATE, 0, "Date&Time", 0, date_cch_str},
#ifdef DAC_SUPPORT
    {350,180,80, 60, NULL, UI_WAV, 0, "WavFile", 0, NULL},
#endif
    {350,250,80, 60, NULL, UI_TIPT, 0, "TextInput", 0, NULL},
    {350,460,80, 60, NULL, UI_POWER, 0, "PowerMonitor", 0, power_cch_str},
    {350,390,80, 60, NULL, UI_RANDOM, 0, "Random", 0, NULL},
    {350,320,80, 60, NULL, UI_SET, 0, "Settings", 0, NULL},
    {350,600,80, 60, NULL, UI_SD, 0, "SDCard", 0, sd_card_cch_str},
    {350,530,80, 60, sound_ctrl, -1, 0, "Sound", 0, sound_cch_str},
#else
    {20,30,120, 20, often_used_timer, -1, 0, "OftenTimer", 0, often_used_timer_cch_str},
    {20,60,120, 20, NULL, UI_POFF_CTD, 0, "PowerOffCountDown", 0, poff_countdown_cch_str},
    {20,90,120, 20, soft_reset_system, -1, 0, "Reboot", 0, reboot_cch_str},
    {20,120,120, 20, reboot_download, -1, 0, "RebootDownload", 0, rebootd_cch_str},
    {20,150,120, 20, power_off, -1, 0, "PowerOff", 0, poff_cch_str},
    {20,180,120, 20, music_test, -1, 0, "MusicTest", 0, music_test_cch_str},
    {20,210,120, 20, f3mins_timer, -1, 0, "3x1mins TIMER", 0, _3x1mins_timer_cch_str},
    {20,240,120, 20, NULL, UI_TIMER_SET, 0, "More Timer", 0, more_timer_cch_str},
    {20,270,120, 20, NULL, UI_DATE, 0, "Date&Time", 0, date_cch_str},
#ifdef DAC_SUPPORT
    {150,120,80, 20, NULL, UI_WAV, 0, "WavFile", 0, NULL},
#endif
    {150,210,80, 20, NULL, UI_POWER, 0, "PowerMonitor", 0, power_cch_str},
    {150,180,80, 20, NULL, UI_RANDOM, 0, "Random", 0, NULL},
    {150,150,80, 20, NULL, UI_SET, 0, "Settings", 0, NULL},
    {150,270,80, 20, NULL, UI_SD, 0, "SDCard", 0, sd_card_cch_str},
    {150,240,80, 20, sound_ctrl, -1, 0, "Sound", 0, sound_cch_str},
#endif
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
};

void mins_2()
{
    timer_ui_transfer_with_para(60*2, ui_buf[8], xianglian);
}

void mins_3()
{
    timer_ui_transfer_with_para(60*3, ui_buf[8], xianglian);
}

void mins_5()
{
    timer_ui_transfer_with_para(60*5, ui_buf[8], xianglian);
}

void mins_10()
{
    timer_ui_transfer_with_para(60*10, ui_buf[8], xianglian);
}

void mins_20()
{
    timer_ui_transfer_with_para(60*20, ui_buf[8], xianglian);
}

void mins_30()
{
    timer_ui_transfer_with_para(60*30, ui_buf[8], xianglian);
}

void mins_45()
{
    timer_ui_transfer_with_para(60*45, ui_buf[8], xianglian);
}

void mins_1h()
{
    timer_ui_transfer_with_para(60*63, ui_buf[8], xianglian);
}

void rp_2()
{
    ui_buf[8]=2;
    lcd_lprintf(60, 280, "Repeat %d times   ", ui_buf[8]);
}

void rp_3()
{
    ui_buf[8]=3;
    lcd_lprintf(60, 280, "Repeat %d times   ", ui_buf[8]);
}

void rp_4()
{
    ui_buf[8]=4;
    lcd_lprintf(60, 280, "Repeat %d times   ", ui_buf[8]);
}

void rp_1()
{
    ui_buf[8]=1;
    lcd_lprintf(60, 280, "Repeat %d times   ", ui_buf[8]);
}

button_t timer_set_button[]={
#ifdef LARGE_SCREEN
    { 60, 60, 80,  40, mins_2, -1, 0, "2 mins",     0, mins_2_cch_str},
    {160, 60, 80,  40, mins_3, -1, 0, "3 mins",     0, mins_3_cch_str},
    {260, 60, 80,  40, mins_5, -1, 0, "5 mins",     0, mins_5_cch_str},
    {360, 60, 80,  40, mins_10, -1, 0, "10 mins",   0, mins_10_cch_str},
    { 60,120, 80,  40, mins_20, -1, 0, "20 mins",   0, mins_20_cch_str},
    {160,120, 80,  40, mins_30, -1, 0, "30 mins",   0, mins_30_cch_str},
    {260,120, 80,  40, mins_45, -1, 0, "45 mins",   0, mins_45_cch_str},
    {360,120, 80,  40, mins_1h, -1, 0, "1h+3 mins", 0, mins_1h_cch_str},
    { 60,200, 80,  40, rp_2, -1, 0, "repeat 2",     0, rp_2_cch_str},
    {160,200, 80,  40, rp_3, -1, 0, "repeat 3",     0, rp_3_cch_str},
    {260,200, 80,  40, rp_4, -1, 0, "repeat 4",     0, rp_4_cch_str},
    {360,200, 80,  40, rp_1, -1, 0, "repeat 1",     0, rp_1_cch_str},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#else
    { 5, 20, 50,  20, mins_2, -1, 0, "2 mins",     0, mins_2_cch_str},
    {60, 20, 50,  20, mins_3, -1, 0, "3 mins",     0, mins_3_cch_str},
    {115, 20, 50,  20, mins_5, -1, 0, "5 mins",     0, mins_5_cch_str},
    {170, 20, 50,  20, mins_10, -1, 0, "10mins",   0, mins_10_cch_str},
    { 5, 60, 50,  20, mins_20, -1, 0, "20mins",   0, mins_20_cch_str},
    {60, 60, 50,  20, mins_30, -1, 0, "30mins",   0, mins_30_cch_str},
    {115, 60, 50,  20, mins_45, -1, 0, "45mins",   0, mins_45_cch_str},
    {170, 60, 50,  20, mins_1h, -1, 0, "1h+3ms", 0, mins_1h_cch_str},
    { 5, 100, 50,  20, rp_2, -1, 0, "rept 2",     0, rp_2_cch_str},
    {60,100, 50,  20, rp_3, -1, 0, "rept 3",     0, rp_3_cch_str},
    {115,100, 50,  20, rp_4, -1, 0, "rept 4",     0, rp_4_cch_str},
    {170,100, 50,  20, rp_1, -1, 0, "rept 1",     0, rp_1_cch_str},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#endif
};

/*UI DATE*/
struct point
{
    uint16_t px;
    uint16_t py;
};
button_t date_button[];

#define LAST_SEC_INDX 0
#define LAST_MIN_INDX 1
#define LAST_HOR_INDX 2

#ifdef ALIENTEK_MINI
#define CLOCK_CX 120
#define CLOCK_CY 160
struct point clock_point[8]={
    {0,0},
    {10,1},
    {21,2},
    {31,5},
    {41,9},
    {50,13},
    {59,19},
    {67,26},
};
#define CLOCK_R 100
#else
#define CLOCK_CX 240
#define CLOCK_CY 400
struct point clock_point[8]={
    {0,0},
    {24,1},
    {48,5},
    {71,11},
    {94,20},
    {115,31},
    {135,44},
    {154,59},
};
#define CLOCK_R 230
#endif

struct point* get_clock_point(int pinx)
{
    pinx %= 15;
    if(pinx<8){
        return &clock_point[pinx];
    }
    else{
        return &clock_point[15-pinx];
    }
}

void get_real_clock_point(u16 pinx, struct point*rslt)
{
    u16 xc = rslt->px, yc = rslt->py;
    pinx%=60;
    if(pinx<8){
        rslt->px=xc+get_clock_point(pinx)->px;
        rslt->py=yc-CLOCK_R+get_clock_point(pinx)->py;
    }
    else if(pinx>7&&pinx<15){
        rslt->px=xc+CLOCK_R-get_clock_point(pinx)->py;
        rslt->py=yc-get_clock_point(pinx)->px;
    }
    else if(pinx>14&&pinx<23){
        rslt->px=xc+CLOCK_R-get_clock_point(pinx)->py;
        rslt->py=yc+get_clock_point(pinx)->px;
    }
    else if(pinx>22&&pinx<30){
        rslt->px=xc+get_clock_point(pinx)->px;
        rslt->py=yc+CLOCK_R-get_clock_point(pinx)->py;
    }
    else if(pinx>29&&pinx<38){
        rslt->px=xc-get_clock_point(pinx)->px;
        rslt->py=yc+CLOCK_R-get_clock_point(pinx)->py;
    }
    else if(pinx>37&&pinx<46){
        rslt->px=xc-CLOCK_R+get_clock_point(pinx)->py;
        rslt->py=yc+get_clock_point(pinx)->px;
    }
    else if(pinx>45&&pinx<53){
        rslt->px=xc-CLOCK_R+get_clock_point(pinx)->py;
        rslt->py=yc-get_clock_point(pinx)->px;
    }
    else{
        rslt->px=xc-get_clock_point(pinx)->px;
        rslt->py=yc-CLOCK_R+get_clock_point(pinx)->py;
    }
}

void draw_clock_face(int xc, int yc)
{
    int ll;
    gui_circle(xc, yc, BLACK, CLOCK_R, 0);
    for(int i = 0;i < 8; i++){
        if(i==0){
            ll = 3;
        }
        else if(i==5){
            ll = 2;
        }
        else{
            ll = 1;
        }
        LCD_DrawLine_direction(xc, yc, xc+clock_point[i].px, yc-CLOCK_R+clock_point[i].py, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
        LCD_DrawLine_direction(xc, yc, xc-clock_point[i].px, yc-CLOCK_R+clock_point[i].py, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
        LCD_DrawLine_direction(xc, yc, xc+clock_point[i].px, yc+CLOCK_R-clock_point[i].py, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
        LCD_DrawLine_direction(xc, yc, xc-clock_point[i].px, yc+CLOCK_R-clock_point[i].py, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
        LCD_DrawLine_direction(xc, yc, xc-CLOCK_R+clock_point[i].py, yc+clock_point[i].px, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
        LCD_DrawLine_direction(xc, yc, xc-CLOCK_R+clock_point[i].py, yc-clock_point[i].px, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
        LCD_DrawLine_direction(xc, yc, xc+CLOCK_R-clock_point[i].py, yc+clock_point[i].px, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
        LCD_DrawLine_direction(xc, yc, xc+CLOCK_R-clock_point[i].py, yc-clock_point[i].px, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
    }
#if 0
    for(int i = 0;i < 60; i++){
        struct point tmp_point;
        tmp_point.px=xc;
        tmp_point.py=yc;
        get_real_clock_point(i, &tmp_point);
        if(i%5==0){
            ll = 2;
        }
        else{
            ll = 1;
        }
        LCD_DrawLine_direction(xc, yc, tmp_point.px, tmp_point.py, RATIO_BASE_OF_LENGTH-ll, RATIO_BASE_OF_LENGTH);
    }
#endif
}

void draw_clock_pointer(int xc, int yc, int pt_inx, int len)
{
    struct point tmp_point;
    tmp_point.px=xc;
    tmp_point.py=yc;
    get_real_clock_point(pt_inx, &tmp_point);
    LCD_DrawLine_direction(xc, yc, tmp_point.px, tmp_point.py, -4, len);
}

void date_ui_init(void*vp)
{
#ifdef LARGE_SCREEN
    char t[ENV_MAX_VALUE_LEN];
#endif
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    lprintf_time("data ui\n");
    common_ui_init(vp);
#ifdef LARGE_SCREEN
    lcd_lprintf(0, 20, "Version:%s%s", VERSION, GIT_SHA1);
    if(ENV_OK == get_env("LastTimeAdj", t)){
        lcd_lprintf(15, 635, "last adj date:%s", t);
    }
    if(ENV_OK == get_env("HsAdj1Min", t)){
        lcd_lprintf(320, 630, "AutoCorrect:%s", t);
    }
#endif
#ifdef RTC_8563
#ifdef LARGE_SCREEN
    if(check_rtc_alert_and_clear()){
        often_used_timer();
    }
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, 260, 45);
#else
    if(check_rtc_alert_and_clear()){
        often_used_timer();
    }
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, 260, 45);
#endif
#endif
    draw_clock_face(CLOCK_CX, CLOCK_CY);
    ui_buf[LAST_SEC_INDX]=60;
    ui_buf[LAST_MIN_INDX]=60;
    ui_buf[LAST_HOR_INDX]=60;
#ifdef RTC_8563
    get_t_t_n_c(1);
#endif
}
#define SEC_PTER_LEN 17
#define MIN_PTER_LEN 13
#define HOR_PTER_LEN 10
void date_ui_process_event(void*vp)
{
    uint32_t h_ix;
    date_info_t t_cur_date = {0};
    ui_t* uif =(ui_t*)vp;
    (void)uif;

    if(g_flag_1s){
#ifdef LARGE_SCREEN
        set_LCD_Char_scale(2);
        lcd_lprintf(10, 100, "%s  ", get_rtc_time(&t_cur_date));
        set_LCD_Char_scale(1);
#else
        get_rtc_time(&t_cur_date);
#endif
        lprintf("task timect %x\r\n", cur_task_timeout_ct);


        if(t_cur_date.second != ui_buf[LAST_SEC_INDX]){
            POINT_COLOR=WHITE;//clear old one
            draw_clock_pointer(CLOCK_CX, CLOCK_CY,ui_buf[LAST_SEC_INDX],SEC_PTER_LEN);
            POINT_COLOR=BLACK;
            draw_clock_pointer(CLOCK_CX, CLOCK_CY,t_cur_date.second,SEC_PTER_LEN);
        }
        if(t_cur_date.minute != ui_buf[LAST_MIN_INDX] ||
                (60+t_cur_date.minute-ui_buf[LAST_SEC_INDX])%30==0 ||
                t_cur_date.minute==ui_buf[LAST_SEC_INDX]){//cleared by second pointer
            POINT_COLOR=WHITE;//clear old one
            draw_clock_pointer(CLOCK_CX, CLOCK_CY,ui_buf[LAST_MIN_INDX],MIN_PTER_LEN);
            POINT_COLOR=BLACK;
            draw_clock_pointer(CLOCK_CX, CLOCK_CY,t_cur_date.minute,MIN_PTER_LEN);
        }
        h_ix = (t_cur_date.hour%12)*5+t_cur_date.minute/12;
        if(h_ix != ui_buf[LAST_HOR_INDX] ||
                h_ix==ui_buf[LAST_MIN_INDX] ||
                (60+h_ix-ui_buf[LAST_MIN_INDX])%30==0 ||
                (60+h_ix-ui_buf[LAST_SEC_INDX])%30==0 ||
                h_ix==ui_buf[LAST_SEC_INDX]){//cleared by second pointer
            POINT_COLOR=WHITE;//clear old one
            draw_clock_pointer(CLOCK_CX, CLOCK_CY,ui_buf[LAST_HOR_INDX],HOR_PTER_LEN);
            POINT_COLOR=BLACK;
            draw_clock_pointer(CLOCK_CX, CLOCK_CY,h_ix,HOR_PTER_LEN);
        }
        ui_buf[LAST_SEC_INDX]=t_cur_date.second;
        ui_buf[LAST_MIN_INDX]=t_cur_date.minute;
        ui_buf[LAST_HOR_INDX]=h_ix;
        gui_circle(CLOCK_CX, CLOCK_CY, BLACK, 15, 1);
    }
    common_process_event(vp);
}

nedt_t date_set_nedt[]={
#ifdef LARGE_SCREEN
    {80, 100, 350, 40, 2099, 2000, 10, 1, NULL, "Year"},
    {80, 160, 350, 40, 12, 1, 6, 1, NULL, "Mon"},
    {80, 220, 350, 40, 31, 1, 10, 1, NULL, "Day"},
    {80, 280, 350, 40, 23, 0, 10, 1, NULL, "Hour"},
    {80, 340, 350, 40, 59, 0, 10, 1, NULL, "Min"},
    {80, 400, 350, 40, 6, 0, 2, 1, NULL, "Week"},
    {80, 520, 350, 40, 23, 0, 10, 1, NULL, "HAlert"},
    {80, 600, 350, 40, 59, 0, 10, 1, NULL, "MAlert"},
    {-1, -1,-1, -1, 0, 0, 0, 0, NULL, NULL},
#else
    {40, 20, 195, 20, 2099, 2000, 10, 1, NULL, "Year"},
    {40, 45, 195, 20, 12, 1, 6, 1, NULL, "Month"},
    {40, 70, 195, 20, 31, 1, 10, 1, NULL, "Day"},
    {40, 95, 195, 20, 23, 0, 10, 1, NULL, "Hour"},
    {40, 120, 195, 20, 59, 0, 10, 1, NULL, "Min"},
    {40, 145, 195, 20, 6, 0, 2, 1, NULL, "Week"},
    {40, 205, 195, 20, 23, 0, 10, 1, NULL, "HAlt"},
    {40, 230, 195, 20, 59, 0, 10, 1, NULL, "MAlt"},
    {-1, -1,-1, -1, 0, 0, 0, 0, NULL, NULL},
#endif
};

#define DATE_SET_Y_START 200
void date_set_ui_init(void*vp)
{
    date_info_t t_date = {0};
    date_set_nedt[0].data=&ui_buf[0];
    date_set_nedt[1].data=&ui_buf[1];
    date_set_nedt[2].data=&ui_buf[2];
    date_set_nedt[3].data=&ui_buf[3];
    date_set_nedt[4].data=&ui_buf[4];
    date_set_nedt[5].data=&ui_buf[5];
    date_set_nedt[6].data=&ui_buf[6];
    date_set_nedt[7].data=&ui_buf[7];
    get_rtc_time(&t_date);
    ui_buf[0]=t_date.year;
    ui_buf[1]=t_date.month;
    ui_buf[2]=t_date.day;
    ui_buf[3]=t_date.hour;
    ui_buf[4]=t_date.minute;
    ui_buf[5]=t_date.weekday;
    ui_buf[6]=bcd2hex(rtc_read_reg(0xa));
    ui_buf[7]=bcd2hex(rtc_read_reg(0x9));
    common_ui_init(vp);
}
void date_set_ui_process_event(void*vp)
{
    if(g_flag_1s){
#ifdef LARGE_SCREEN
        set_LCD_Char_scale(2);
        lcd_lprintf(10, 40, "%s  ", get_rtc_time(NULL));
        set_LCD_Char_scale(1);
#endif
    }
    common_process_event(vp);
}

void date_set_enable()
{
    uint8_t dat[6];
    dat[0]=hex2bcd(ui_buf[0]-2000);
    dat[1]=hex2bcd(ui_buf[1]);
    dat[2]=hex2bcd(ui_buf[2]);
    dat[3]=hex2bcd(ui_buf[3]);
    dat[4]=hex2bcd(ui_buf[4]);
    dat[5]=hex2bcd(ui_buf[5]);
    lcd_lprintf(20, DATE_SET_Y_START+500, "%x %x %x %x %x %x",
            dat[0],
            dat[1],
            dat[2],
            dat[3],
            dat[4],
            dat[5]);
    rtc_write(dat);
    set_env("LastTimeAdj", get_rtc_time(NULL));
}

void alert_set_enable()
{
    rtc_write_reg(0xa, hex2bcd(ui_buf[6]));
    rtc_write_reg(0x9, hex2bcd(ui_buf[7]));
}

button_t date_set_button[]={
#ifdef LARGE_SCREEN
    {375, 460, 100,  40, date_set_enable, -1, 0, "OK", 0, NULL},
    {375, 660, 100,  40, alert_set_enable, -1, 0, "OK", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#else
    {205, 180, 30,  20, date_set_enable, -1, 0, "OK", 0, NULL},
    {205, 255, 30,  20, alert_set_enable, -1, 0, "OK", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#endif
};

void scaleplate_drawing()
{
#ifdef LARGE_SCREEN
    ui_buf[0] = 640;
    lcd_lprintf(10, 655, "I 400   0  -400(mA)");
    lcd_lprintf(175, 655, " 4.0  3.6  3.2(v)");
    lcd_lprintf(330, 655, "VCore(0.2v/d) 3.0v");
    //i=0
    POINT_COLOR=LIGHTGRAY;
    LCD_DrawLine(80, 640, 80, 30);
    for(int i=1;i<24;i++){
        LCD_DrawLine(20*i, 640, 20*i, 30);
    }
    POINT_COLOR=BLACK;
    LCD_DrawLine(80, 650, 80, 30);
    LCD_DrawLine(200, 650, 200, 30);
    LCD_DrawLine(240, 650, 240, 30);
    LCD_DrawLine(280, 650, 280, 30);
    LCD_DrawLine(460, 650, 460, 30);
    LCD_DrawLine(160, 660, 160, 30);
    LCD_DrawLine(320, 660, 320, 30);
#else
    ui_buf[0] = 250;
    lcd_lprintf(5, 250, "400 0-400");
    lcd_lprintf(85, 250, "4.0  3.4");
    lcd_lprintf(156, 250, "VCo(.2)3.0");
    //i=0
    POINT_COLOR=LIGHTGRAY;
    LCD_DrawLine(40, 250, 40, 30);
    for(int i=1;i<24;i++){
        LCD_DrawLine(10*i, 250, 10*i, 30);
    }
    POINT_COLOR=BLACK;
    LCD_DrawLine(40, 250, 40, 30);
    LCD_DrawLine(100, 250, 100, 30);
    LCD_DrawLine(120, 250, 120, 30);
    LCD_DrawLine(140, 250, 140, 30);
    LCD_DrawLine(230, 250, 230, 30);
    LCD_DrawLine(80, 250, 80, 30);
    LCD_DrawLine(160, 250, 160, 30);
#endif
}

void clr_wave()
{
#ifdef LARGE_SCREEN
    lcd_clr_window(WHITE, 0, 30, 480, 650);
#else
    lcd_clr_window(WHITE, 0, 30, 240, 250);
#endif
    scaleplate_drawing();
}

button_t power_ui_button[]={
#ifdef LARGE_SCREEN
    {15, 680, 100,  40, clr_wave, -1, 0, "Clear", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#else
    {10, 270, 60,  20, clr_wave, -1, 0, "Clear", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#endif
};

void power_ui_init(void*vp)
{
    lprintf("power ui\n");
    common_ui_init(vp);
    scaleplate_drawing();
}
void power_ui_process_event(void*vp)
{
    uint32_t vcore_mv, vbat_mv;
    int32_t i_mA;
#ifdef LARGE_SCREEN
    if(ui_buf[0]<=30){
        ui_buf[0] = 640;
    }
    get_myadc_value(&vcore_mv, &vbat_mv, &i_mA);
    i_mA += 800;
    i_mA /= 10;
    PutPixel(160-i_mA, ui_buf[0], RED);
    vbat_mv -= 2800;
    vbat_mv /= 10;
    PutPixel(320-vbat_mv, ui_buf[0], BLUE);
    vcore_mv -= 2800;
    vcore_mv /= 10;
    PutPixel(480-vcore_mv, ui_buf[0], BROWN);
    ui_buf[0]--;
#else
    if(ui_buf[0]<=30){
        ui_buf[0] = 250;
    }
    get_myadc_value(&vcore_mv, &vbat_mv, &i_mA);
    i_mA += 800;
    i_mA /= 20;
    PutPixel(80-i_mA, ui_buf[0], RED);
    vbat_mv -= 2800;
    vbat_mv /= 20;
    PutPixel(160-vbat_mv, ui_buf[0], BLUE);
    vcore_mv -= 2800;
    vcore_mv /= 20;
    PutPixel(240-vcore_mv, ui_buf[0], BROWN);
    ui_buf[0]--;
#endif
    common_process_event(vp);
}

int is_english()
{
    char t[ENV_MAX_VALUE_LEN];
    if(ENV_OK == get_env("language", t)){
        lprintf("getenv laguage %s\n", t);
        if(t[0] == 'E'){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
            return 1;
    }
}

void language_set()
{
    if(is_english()){
        lprintf("iseng=1, set to C\n");
        set_env("language", "C");
    }
    else{
        lprintf("iseng=0, set to E\n");
        set_env("language", "E");
    }
    draw_button(&date_button[0]);
}

void adjust_enable()
{
    if(date_button[1].disable &&
            date_button[2].disable &&
            date_button[3].disable){
        date_button[1].disable = 0;
        date_button[2].disable = 0;
        date_button[3].disable = 0;
    }
    else{
        date_button[1].disable = 1;
        date_button[2].disable = 1;
        date_button[3].disable = 1;
    }
    draw_button(&date_button[1]);
}

void slow_1()
{
    if(RTC_OK==adjust_1min(0)){
        date_button[2].disable = 1;
        draw_button(&date_button[2]);
        set_env("LastTimeAdj", get_rtc_time(NULL));
    }
}

void fast_1()
{
    if(RTC_OK==adjust_1min(1)){
        date_button[1].disable = 1;
        draw_button(&date_button[1]);
        set_env("LastTimeAdj", get_rtc_time(NULL));
    }
}

void clr_s()
{
    clear_second();
    date_button[3].disable = 1;
    draw_button(&date_button[3]);
    set_env("LastTimeAdj", get_rtc_time(NULL));
}

void toggle_auto_power_on()
{
    if(0!=get_env_uint("autopon", 0)){
        set_env_uint("autopon", 0);
    }
    else{
        set_env_uint("autopon", 1);
    }
#ifdef LARGE_SCREEN
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, 260, 45);
#else
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, 5, 30);
#endif
}

button_t date_button[]={
#ifdef LARGE_SCREEN
    {15, 660, 100,  40, adjust_enable, -1, 0, "time adjust", 0, time_adjust_cch_str},
    {135, 660, 100,  40, fast_1, -1, 0, "faster 1min", 1, NULL},
    {255, 660, 100,  40, slow_1, -1, 0, "slower 1min", 1, NULL},
    {375, 660, 100,  40, clr_s, -1, 0, "clear second", 1, NULL},
    {15, 150, 100,  40, language_set, -1, 0, language_cch_str, 0, "English"},
    {15, 590, 90,  40, toggle_auto_power_on, -1, 0, "autopon", 0, NULL},
    {375, 590, 90,  40, NULL, UI_DATE_SET, 0, "dateset", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#else
    {10, 270, 60,  20, adjust_enable, -1, 0, "timeadj", 0, time_adjust_cch_str},
    {85, 270, 40,  20, fast_1, -1, 0, "+1min", 1, NULL},
    {135, 270, 40,  20, slow_1, -1, 0, "-1min", 1, NULL},
    {185, 270, 40,  20, clr_s, -1, 0, "sec=0", 1, NULL},
    {10, 55, 55,  20, language_set, -1, 0, language_cch_str, 0, "English"},
    {5, 245, 50,  20, toggle_auto_power_on, -1, 0, "autopon", 0, NULL},
    {180, 245, 50, 20, NULL, UI_DATE_SET, 0, "dateset", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#endif
};
/*UI DATE END*/

/*UI SD*/
#ifdef LARGE_SCREEN
static int text_scale = 2;
#define BOOK_SHOW_WIN_X 5
#define BOOK_SHOW_WIN_Y 60
#define BOOK_SHOW_WIN_DX 1
#define BOOK_SHOW_WIN_DY 8
#define BOOK_SHOW_WIN_W 469
#define BOOK_SHOW_WIN_H 598
#else
static int text_scale = 1;
#define BOOK_SHOW_WIN_X 6
#define BOOK_SHOW_WIN_Y 35
#define BOOK_SHOW_WIN_DX 1
#define BOOK_SHOW_WIN_DY 8
#define BOOK_SHOW_WIN_W 226
#define BOOK_SHOW_WIN_H 225
#endif

#define LCD_SHOW_DUMMY 0x1
#define CHECK_CHINESE 0x2
#define SDDISP_BUF_SIZE 512
#define SHOW_FILE_NAME "BOOK"
static char book_buf[SDDISP_BUF_SIZE];
static uint32_t book_file_offset= 0;
static uint32_t page_start_offset= 0;
static uint32_t last_page_start_offset= 0;
static uint32_t page_end_offset= ~0;
static const char*next_show_char=0;
int update_percentage();

void sd_detect();
void sd_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    lprintf("sd ui\n");
    ui_buf[0]=0xffffffff;
    common_ui_init(vp);
    lcd_lprintf(0, 20, "Version:%s%s", VERSION, GIT_SHA1);
    page_start_offset = get_env_uint("book_posi", 0);
    lprintf("getenv page_start_offset %d\n", page_start_offset);
    page_end_offset = page_start_offset - 1;
    sd_detect();
}
void sd_ui_uninit(void*vp)
{
    if(0xffffffff!=ui_buf[0]){
        close_file(ui_buf[0]);
    }
    common_ui_uninit(vp);
    lprintf("setenv page_start_offset %d\n", page_start_offset);
    set_env_uint("book_posi", page_start_offset);
}
void sd_ui_process_event(void*vp)
{
    common_process_event(vp);
}

int chs_need_adjust(const char*s)
{
    int ct = 0;
    while(1){
        if(*s == 0 || (*s & 0x80) == 0){
            if(ct & 0x1){
                return 1;
            }
        }
        s++;
        ct++;
    }
}

int init_sd(int is_dummy)
{
    SD_CardInfo mycard;
    if(!is_dummy)lcd_lprintf(0, 40, "Detecting sd card...");
    if(SD_Init() != SD_OK && SD_Init() != SD_OK)
    {
        //retry once
        lprintf("Fail\n");
        if(!is_dummy)lcd_lprintf(0, 60, "Failed.");
        return -1;
    }
    else{
        lprintf("OK\n");
    }
    if(!is_dummy)lcd_lprintf(0, 60, "OK.    ");
    if(SD_GetCardInfo(&mycard) != SD_OK)
    {
        lprintf("get card info Fail\n");
        if(!is_dummy)lcd_lprintf(0, 80, "Get card info Fail            ");
        if(!is_dummy)lcd_lprintf(0, 100, "                              ");
        return -1;
    }
    else{
        lprintf("block size %d\n", mycard.CardBlockSize);
        if(!is_dummy)lcd_lprintf(0, 80, "block size %d", mycard.CardBlockSize);
        lprintf("block capacity %d\n", mycard.CardCapacity);
        if(!is_dummy)lcd_lprintf(0, 100, "block capacity %d", mycard.CardCapacity);
        return 0;
    }
}

uint32_t get_percentage()
{
    int filesize, fd;
    if(0xffffffff==ui_buf[0]){
        fd = open_file(SD_ReadBlock, "BOOK", "TXT", &filesize);
        if(fd == FS_DISK_ERR){
            init_sd(0);
            fd = open_file(SD_ReadBlock, "BOOK", "TXT", &filesize);
        }
    }
    if(fd<0){
        lprintf("get file size error %x\n", filesize);
        return 0xffffffff;
    }
    ui_buf[0]=fd;
    ui_buf[1]=filesize;
    uint32_t ptg = (page_start_offset+5)*1000 / filesize / 10;
    return ptg;
}

int show_book(int flag)
{
    int ret, showed_chars, fd;
    int is_dummy = flag & LCD_SHOW_DUMMY;
    memset(book_buf, 0, 512);
    if(-1 == update_percentage())
    {
        lprintf("update percentage fail\n");
        return -1;
    }
    fd=ui_buf[0];
    ret = read_file(fd, book_buf, book_file_offset, 511);
    lprintf("----get file ret %d dummy %x bfo %d\n", ret, is_dummy, book_file_offset);
    if(ret != FS_OK){
        if(-1==init_sd(is_dummy))
        {
            return -1;
        }
        ret = read_file(fd, book_buf, book_file_offset, 511);
        lprintf("retry after init sd:get file ret %d\n", ret);
    }
    if(ret == FS_OK){
        u32 show_x=BOOK_SHOW_WIN_X, show_y=BOOK_SHOW_WIN_Y;
        win bookw={BOOK_SHOW_WIN_X, BOOK_SHOW_WIN_Y, BOOK_SHOW_WIN_W, BOOK_SHOW_WIN_H,
                BOOK_SHOW_WIN_DX, BOOK_SHOW_WIN_DY};
        if(!is_dummy)lcd_lprintf(BOOK_SHOW_WIN_X-5, BOOK_SHOW_WIN_Y-20, " book.txt               ");
        if(!is_dummy)lcd_clr_window(WHITE, BOOK_SHOW_WIN_X-5, BOOK_SHOW_WIN_Y-5,
                BOOK_SHOW_WIN_X+BOOK_SHOW_WIN_W+5, BOOK_SHOW_WIN_Y+BOOK_SHOW_WIN_H+5);
        draw_sq(BOOK_SHOW_WIN_X-5, BOOK_SHOW_WIN_Y-5,
                BOOK_SHOW_WIN_X+BOOK_SHOW_WIN_W+5, BOOK_SHOW_WIN_Y+BOOK_SHOW_WIN_H+5, BLACK);
        set_LCD_Char_scale(text_scale);
        while(1){
#if 0
            mem_print(book_buf, 0, 512);
            if((flag & CHECK_CHINESE) && chs_need_adjust(next_show_char)){
                next_show_char++;
            }
#endif
            next_show_char=area_show_str(&bookw, &show_x, &show_y, next_show_char, is_dummy);
            showed_chars = next_show_char - book_buf;
            lprintf("%d xy %d %d\n", showed_chars, show_x, show_y);
            if(showed_chars <510) break;
            book_file_offset += showed_chars;
            memset(book_buf, 0, 512);
            ret = read_file(fd, book_buf, book_file_offset, 511);
            if(ret != FS_OK){
                lprintf("sd read fail\n");
                return -1;
            }
            next_show_char=book_buf;
        }
        set_LCD_Char_scale(1);
    }
    else{
        lprintf("sd read fail\n");
    }
    lprintf("----ret %d dummy %d bfo %d\n", ret, is_dummy, book_file_offset);
    return 0;
}

void font_size(){
    text_scale = 3 - text_scale;
    book_file_offset=page_start_offset;
    next_show_char=book_buf;
    show_book(0);
    page_end_offset = book_file_offset+next_show_char-book_buf-1;
}

void sd_detect(){
    last_page_start_offset = page_start_offset;
    page_start_offset = page_end_offset + 1;
    book_file_offset=page_start_offset;
    next_show_char=book_buf;
    lprintf("NEXT:start %d\n", page_start_offset);
    show_book(0);
    page_end_offset = book_file_offset+next_show_char-book_buf-1;
    lprintf("NEXT:end %d\n", page_end_offset);

#if 0
    book_file_offset=page_start_offset;
    next_show_char=book_buf;
    show_book(1);
    page_end_offset = book_file_offset+next_show_char-book_buf-1;
#endif
}

uint32_t get_full_disp_size()
{
    return (BOOK_SHOW_WIN_W*BOOK_SHOW_WIN_H/(16+BOOK_SHOW_WIN_DY)/(8+BOOK_SHOW_WIN_DY)/text_scale/text_scale);
}

void last_page(){
    int flag = 0;
#if 0
    uint32_t test_last_start = last_page_start_offset;
    next_show_char=book_buf;
    while(1){
        lprintf("LAST:try laststart %d\n", test_last_start);
        book_file_offset=test_last_start;
        show_book(1);
        uint32_t test_page_end_offset = book_file_offset+next_show_char-book_buf-1;
        uint32_t diff = page_start_offset - test_page_end_offset - 1;
        lprintf("LAST:test end %d, target %d diff %d\n",
                test_page_end_offset, page_start_offset, diff);
        if(page_start_offset<=test_page_end_offset+1){
            break;
        }
        test_last_start+=diff;
        next_show_char=book_buf;
    }
#endif
    if(last_page_start_offset<page_start_offset){
        page_start_offset = last_page_start_offset ;
    }
    else if(page_start_offset<get_full_disp_size()+1){
        page_start_offset = 0;
    }
    else{
        //try find last page start
        uint32_t test_last_start = page_start_offset-get_full_disp_size()-1;
        next_show_char=book_buf;
        while(1){
            lprintf("LAST:try laststart %d\n", test_last_start);
            book_file_offset=test_last_start;
            show_book(LCD_SHOW_DUMMY);
            uint32_t test_page_end_offset = book_file_offset+next_show_char-book_buf-1;
            uint32_t diff = page_start_offset - test_page_end_offset - 1;
            lprintf("LAST:test end %d, target %d diff %d\n",
                    test_page_end_offset, page_start_offset, diff);
            if(page_start_offset<=test_page_end_offset+1){
                break;
            }
            test_last_start+=diff;
            next_show_char=book_buf;
        }
        page_start_offset = test_last_start;
        flag = CHECK_CHINESE;
    }
    book_file_offset=page_start_offset;
    next_show_char=book_buf;
    lprintf("LAST:start %d\n", page_start_offset);
    show_book(flag);
    page_end_offset = book_file_offset+next_show_char-book_buf-1;
    lprintf("LAST:end %d\n", page_end_offset);
}

void percentage_page()
{
    int filesize, fd;
    if(0xffffffff==ui_buf[0]){
        fd = open_file(SD_ReadBlock, "BOOK", "TXT", &filesize);
        if(fd == FS_DISK_ERR){
            init_sd(0);
            fd = open_file(SD_ReadBlock, "BOOK", "TXT", &filesize);
        }
        if(fd < 0){
            lprintf("open book.txt failed\n");
            return;
        }
        ui_buf[0]=fd;
        ui_buf[1]=filesize;
    }
    filesize=ui_buf[1];
    uint32_t ptg = page_start_offset*100 / (int64_t)filesize;
    ptg = (ptg +5)/10*10;
    if(ptg == 100){
        ptg = 0;
    }
    else{
        ptg += 10;
    }
    last_page_start_offset = 0xffffffff;
    page_start_offset = filesize * ptg /100;
    book_file_offset=page_start_offset;
    next_show_char=book_buf;
    lprintf("PERC:start %d\n", page_start_offset);
    show_book(0);
    page_end_offset = book_file_offset+next_show_char-book_buf-1;
    lprintf("PERC:end %d\n", page_end_offset);
}

void show_correct()
{
    page_start_offset+=1;
    book_file_offset=page_start_offset;
    next_show_char=book_buf;
    show_book(0);
    page_end_offset = book_file_offset+next_show_char-book_buf-1;
}

button_t sd_button[]={
    //{235, 680, 100,  40, sd_detect, -1, 0, "", 0, sd_detect_cch_str},
    {BOOK_SHOW_WIN_X-5, BOOK_SHOW_WIN_Y-5, BOOK_SHOW_WIN_W,  BOOK_SHOW_WIN_H, sd_detect, -1, 0, NULL, 0, NULL},
#ifdef LARGE_SCREEN
    {125, 680, 100,  40, font_size, -1, 0, "Font Size", 0, font_size_cch_str},
    {345, 680, 100,  40, last_page, -1, 0, "Last", 0, last_page_cch_str},
    {15, 680, 100,  40, percentage_page, -1, 0, NULL, 0, NULL},
    {235, 680, 100,  40, show_correct, -1, 0, "fix show", 0, fix_show_cch_str},
#else
    {50, 270, 40,  20, font_size, -1, 0, "Font", 0, font_size_cch_str},
    {140, 270, 40,  20, last_page, -1, 0, "Last", 0, last_page_cch_str},
    {5, 270, 40,  20, percentage_page, -1, 0, NULL, 0, NULL},
    {95, 270, 40,  20, show_correct, -1, 0, "fix", 0, fix_show_cch_str},
#endif
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
};

int update_percentage()
{
    uint32_t pct;
    button_t*pbt = &sd_button[3];
    pct=get_percentage();
    lprintf("updateperc %d\n", pct);
    if(pct == 0xffffffff){
        lprintf("get_percentage fail\n");
        return -1;
    }
#ifdef LARGE_SCREEN
    set_LCD_Char_scale(2);
#endif
    lcd_lprintf(pbt->x+5, pbt->y+5, "%d%  ", pct);
    set_LCD_Char_scale(1);
    return 0;
}

/******end of sd_ui********/

/****start of random ui*****/
void random_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    common_ui_init(vp);
    ui_buf[0] = 0;
    ui_buf[1] = 0;
    ui_buf[2] = 0;
    ui_buf[3] = get_env_uint("random0", 100);
    ui_buf[4] = get_env_uint("random1", 100);
}
void random_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    ui_buf[0]++;
    ui_buf[1]++;
    if(ui_buf[0]>ui_buf[3]){
        ui_buf[0]=0;
    }
    if(ui_buf[1]>ui_buf[4]){
        ui_buf[1]=0;
    }
    common_process_event(vp);
}

void do_random()
{
#ifdef LARGE_SCREEN
    set_LCD_Char_scale(4);
    lcd_lprintf(200, 175, "%d  ", ui_buf[0]);
    lcd_lprintf(200, 350, "%d  ", ui_buf[1]);
    set_LCD_Char_scale(1);
#else
    set_LCD_Char_scale(2);
    lcd_lprintf(100, 100, "%d  ", ui_buf[0]);
    lcd_lprintf(100, 180, "%d  ", ui_buf[1]);
    set_LCD_Char_scale(1);
#endif
}

button_t random_button[]={
#ifdef LARGE_SCREEN
    {125, 550, 200,  80, do_random, -1, 0, "Push", 0, NULL},
#else
    {100, 240, 40,  20, do_random, -1, 0, "Push", 0, NULL},
#endif
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
};
/****end of random ui*****/

/****start of tipt ui*****/
void tipt_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    common_ui_init(vp);
    ui_buf[0] = 0;
    ui_buf[1] = 0;
    ui_buf[2] = 0;
    ui_buf[3] = 0;//choose index. 4 bytes
    ui_buf[4] = 0;//input buf pointer
    memset(book_buf, 0, 512);
}
void tipt_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    common_process_event(vp);
}

#define TIPT_TEXT_SHOW_WIN_X 25
#define TIPT_TEXT_SHOW_WIN_Y 40
#define TIPT_TEXT_SHOW_WIN_W 430
#define TIPT_TEXT_SHOW_WIN_H 150
#define TIPT_TEXT_SHOW_WIN_DX 2
#define TIPT_TEXT_SHOW_WIN_DY 2
#define TIPT_SHOW_WIN_X 25
#define TIPT_SHOW_WIN_Y 200
#define TIPT_SHOW_WIN_W 430
#define TIPT_SHOW_WIN_H 200
#define TIPT_SHOW_WIN_DX 2
#define TIPT_SHOW_WIN_DY 2
#define FONT_SIZE 16
#define N_EACH_LINE (TIPT_SHOW_WIN_W/(FONT_SIZE+TIPT_SHOW_WIN_DX*2))
#define MAX_PY_OPS 4
void do_tipt(void*cfp)
{
#ifdef LARGE_SCREEN
    int btidx=*(int*)cfp;
    char *inputs=(char*)ui_buf;
    char rs[3]={0};
    signed char *choose_idx=(signed char*)&ui_buf[3];//0-py index 1-char index 2-mode:input/py_choose/char_choose
    win tiptw={TIPT_SHOW_WIN_X, TIPT_SHOW_WIN_Y, TIPT_SHOW_WIN_W, TIPT_SHOW_WIN_H,
        TIPT_SHOW_WIN_DX, TIPT_SHOW_WIN_DY};
    win tiptw_text={TIPT_TEXT_SHOW_WIN_X, TIPT_TEXT_SHOW_WIN_Y, TIPT_TEXT_SHOW_WIN_W, TIPT_TEXT_SHOW_WIN_H,
        TIPT_TEXT_SHOW_WIN_DX, TIPT_TEXT_SHOW_WIN_DY};
    u32 t_show_x=TIPT_SHOW_WIN_X, t_show_y=TIPT_SHOW_WIN_Y-FONT_SIZE-TIPT_SHOW_WIN_DY;
	unsigned char t=0;
	unsigned char i=0;
    lcd_clr_window(WHITE, TIPT_SHOW_WIN_X-5, TIPT_SHOW_WIN_Y-5,
            TIPT_SHOW_WIN_X+TIPT_SHOW_WIN_W+5, TIPT_SHOW_WIN_Y+TIPT_SHOW_WIN_H+5);
    draw_sq(TIPT_SHOW_WIN_X-5, TIPT_SHOW_WIN_Y-5,
            TIPT_SHOW_WIN_X+TIPT_SHOW_WIN_W+5, TIPT_SHOW_WIN_Y+TIPT_SHOW_WIN_H+5, BLACK);
    lcd_clr_window(WHITE, TIPT_TEXT_SHOW_WIN_X-5, TIPT_TEXT_SHOW_WIN_Y-5,
            TIPT_TEXT_SHOW_WIN_X+TIPT_TEXT_SHOW_WIN_W+5, TIPT_TEXT_SHOW_WIN_Y+TIPT_TEXT_SHOW_WIN_H+5);
    draw_sq(TIPT_TEXT_SHOW_WIN_X-5, TIPT_TEXT_SHOW_WIN_Y-5,
            TIPT_TEXT_SHOW_WIN_X+TIPT_TEXT_SHOW_WIN_W+5, TIPT_TEXT_SHOW_WIN_Y+TIPT_TEXT_SHOW_WIN_H+5, BLACK);
    lprintf("\r\nbtidx=%d u4=%d each=%d\r\n",btidx, ui_buf[4], N_EACH_LINE);
    mem_print(inputs, 0, 12);
    if(btidx<0 || btidx>=12){
        lprintf("error btidx\r\n");
        return;
    }
    else if(btidx==0){
        if(ui_buf[4]>0){
            inputs[--ui_buf[4]]=0;
        }
		choose_idx[2]=0;
		choose_idx[1]=0;
    }
    else if(btidx<9){
        if(0==choose_idx[2]){
            inputs[ui_buf[4]++]=0x30+btidx+1;
            ui_buf[3]=0;
        }
        else{
            if(btidx==3){
                choose_idx[1]--;
            }
            if(btidx==5){
                choose_idx[1]++;
            }
        }
    }
    else if(btidx==9){
        if(0==choose_idx[2]){
            choose_idx[0]--;
        }
        else{
            choose_idx[1]-=N_EACH_LINE;
        }
    }
    else if(btidx==10){
        if(0==choose_idx[2]){
            choose_idx[0]++;
        }
        else{
            choose_idx[1]+=N_EACH_LINE;
        }
    }
    else if(btidx==11){//enter
        if(0==choose_idx[2]){
            choose_idx[2]=1;
        }
        else{
            lprintf("final enter, clear\r\n");
            ui_buf[0] = 0;
            ui_buf[1] = 0;
            ui_buf[2] = 0;
            ui_buf[3] = 0;//choose index. 4 bytes
            ui_buf[4] = 0;//input buf pointer
            rs[0]=t9.pymb[(int)choose_idx[0]]->pymb[choose_idx[1]*2];
            rs[1]=t9.pymb[(int)choose_idx[0]]->pymb[choose_idx[1]*2+1];
            rs[2]=0;
            strcpy(book_buf, rs);
        }
    }
    lprintf("\r\ninput is:%s\r\n",inputs);
    t=t9.getpymb((unsigned char*)inputs);
    if(t>MAX_PY_OPS)t=MAX_PY_OPS;
	if(t)
	{
		lprintf("total match:%d\r\n",t);
		//lprintf("total match result:\r\n");
		for(i=0;i<t;i++)
		{
			//lprintf("%s,%s\r\n",t9.pymb[i]->py,t9.pymb[i]->pymb);
            next_show_char=(const char*)t9.pymb[i]->py;
            t_show_x=TIPT_SHOW_WIN_X+TIPT_SHOW_WIN_DX;
            t_show_y+=FONT_SIZE+TIPT_SHOW_WIN_DY;
            next_show_char=area_show_str(&tiptw, &t_show_x, &t_show_y, next_show_char, 0);
		}
        if(t==1){
            next_show_char=(const char*)t9.pymb[0]->pymb;
            t_show_x=TIPT_SHOW_WIN_X+TIPT_SHOW_WIN_DX;
            t_show_y+=FONT_SIZE+TIPT_SHOW_WIN_DY;
            next_show_char=area_show_str(&tiptw, &t_show_x, &t_show_y, next_show_char, 0);
            choose_idx[2]=1;
            choose_idx[0]=0;
        }
        else{
            while(choose_idx[0]<0)choose_idx[0]+=t;
            while(choose_idx[0]>(t-1))choose_idx[0]-=t;
            next_show_char=(const char*)t9.pymb[(int)choose_idx[0]]->pymb;
            t_show_x=TIPT_SHOW_WIN_X+TIPT_SHOW_WIN_DX;
            t_show_y+=FONT_SIZE+TIPT_SHOW_WIN_DY;
            next_show_char=area_show_str(&tiptw, &t_show_x, &t_show_y, next_show_char, 0);
            draw_sq2(TIPT_SHOW_WIN_X+TIPT_SHOW_WIN_DX/2, TIPT_SHOW_WIN_Y+TIPT_SHOW_WIN_DY/2+(TIPT_SHOW_WIN_DY+FONT_SIZE)*choose_idx[0],
                    FONT_SIZE*10, FONT_SIZE+TIPT_SHOW_WIN_DY/2, BLACK);
        }
        if(choose_idx[2]==1){
            lprintf("num=%d\r\n", t9.pymb[(int)choose_idx[0]]->num);
            lprintf("choose_idx[1]=0x%b\r\n", choose_idx[1]);
            while(choose_idx[1]<0)choose_idx[1]+=t9.pymb[(int)choose_idx[0]]->num/2;
            lprintf("choose_idx[1]=0x%b\r\n", choose_idx[1]);
            while(choose_idx[1]>=(t9.pymb[(int)choose_idx[0]]->num/2))choose_idx[1]-=t9.pymb[(int)choose_idx[0]]->num/2;
            lprintf("choose_idx[1]=0x%b\r\n", choose_idx[1]);
            draw_sq2(TIPT_SHOW_WIN_X+TIPT_SHOW_WIN_DX/2+(TIPT_SHOW_WIN_DX*2+FONT_SIZE)*(choose_idx[1]%N_EACH_LINE), TIPT_SHOW_WIN_Y+TIPT_SHOW_WIN_DY/2+(TIPT_SHOW_WIN_DY+FONT_SIZE)*(t+choose_idx[1]/N_EACH_LINE),
                    FONT_SIZE+TIPT_SHOW_WIN_DX, FONT_SIZE+TIPT_SHOW_WIN_DY, BLACK);
        }
	}else lprintf("no matched results\r\n");
    next_show_char=book_buf;
    t_show_x=TIPT_TEXT_SHOW_WIN_X+TIPT_TEXT_SHOW_WIN_DX;
    t_show_y=TIPT_TEXT_SHOW_WIN_Y+TIPT_TEXT_SHOW_WIN_DY;
    next_show_char=area_show_str(&tiptw_text, &t_show_x, &t_show_y, next_show_char, 0);
#endif
}

#define TIPT_OX 25
#define TIPT_OY 435

button_t tipt_button[]={
#ifdef LARGE_SCREEN
    {TIPT_OX, TIPT_OY, 80,  80, do_tipt, -1, 0, "1 DEL", 0, NULL},
    {TIPT_OX+115, TIPT_OY, 80,  80, do_tipt, -1, 0, "2 abc", 0, NULL},
    {TIPT_OX+230, TIPT_OY, 80,  80, do_tipt, -1, 0, "3 def", 0, NULL},
    {TIPT_OX, TIPT_OY+100, 80,  80, do_tipt, -1, 0, "4 ghi <", 0, NULL},
    {TIPT_OX+115, TIPT_OY+100, 80,  80, do_tipt, -1, 0, "5 jkl", 0, NULL},
    {TIPT_OX+230, TIPT_OY+100, 80,  80, do_tipt, -1, 0, "6 mno >", 0, NULL},
    {TIPT_OX, TIPT_OY+200, 80,  80, do_tipt, -1, 0, "7 pqrs", 0, NULL},
    {TIPT_OX+115, TIPT_OY+200, 80,  80, do_tipt, -1, 0, "8 tuv", 0, NULL},
    {TIPT_OX+230, TIPT_OY+200, 80,  80, do_tipt, -1, 0, "9 wxyz", 0, NULL},
    {TIPT_OX+345, TIPT_OY, 80,  80, do_tipt, -1, 0, "up", 0, NULL},//9
    {TIPT_OX+345, TIPT_OY+100, 80,  80, do_tipt, -1, 0, "down", 0, NULL},//10
    {TIPT_OX+345, TIPT_OY+200, 80,  80, do_tipt, -1, 0, "enter", 0, NULL},//11
#endif
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
};
/****end of tipt ui*****/

/****start of set ui*****/
nedt_t ui_set_nedt[]={
#ifdef LARGE_SCREEN
    {80, 100, 350, 40, 0x7fffffff, 1, 10, 1, NULL, "random1"},
    {80, 160, 350, 40, 0x7fffffff, 1, 10, 1, NULL, "ramdom2"},
    {80, 220, 350, 40, 0x7fffffff, 1, 10, 1, NULL, "HsAdj1Min"},
    {80, 280, 350, 40, 1, 0, 1, 1, NULL, "+/-"},
    {80, 340, 350, 40, 99, 0, 10, 1, NULL, "Idle_BL"},
    {80, 400, 350, 40, 99, 0, 10, 1, NULL, "BL"},
    {80, 460, 350, 40, 99, 0, 10, 1, NULL, "time_ct"},
    {-1, -1,-1, -1, 0, 0, 0, 0, NULL, NULL},
#else
    {40, 20, 195, 20, 0x7fffffff, 1, 10, 1, NULL, "random1"},
    {40, 45, 195, 20, 0x7fffffff, 1, 10, 1, NULL, "ramdom2"},
    {40, 70, 195, 20, 0x7fffffff, 1, 10, 1, NULL, "HsAdj1Min"},
    {40, 95, 195, 20, 1, 0, 1, 1, NULL, "+/-"},
    {40, 120, 195,20, 99, 0, 10, 1, NULL, "Idle_BL"},
    {40, 145, 195,20, 99, 0, 10, 1, NULL, "BL"},
    {40, 170, 195,20, 99, 0, 10, 1, NULL, "time_ct"},
    {-1, -1,-1, -1, 0, 0, 0, 0, NULL, NULL},
#endif
};

void set_ui_init(void*vp)
{
    char chs[32]={0};
    ui_set_nedt[0].data=&ui_buf[0];
    ui_set_nedt[1].data=&ui_buf[1];
    ui_set_nedt[2].data=&ui_buf[2];
    ui_set_nedt[3].data=&ui_buf[3];
    ui_set_nedt[4].data=&ui_buf[4];
    ui_set_nedt[5].data=&ui_buf[5];
    ui_set_nedt[6].data=&ui_buf[6];
    ui_buf[0]=get_env_uint("random1", 100);
    ui_buf[1]=get_env_uint("random2", 100);
    if(ENV_OK != get_env("HsAdj1Min", chs) ||
            (chs[0]!='+' && chs[0]!='-') ){
        ui_buf[2]=0;
        ui_buf[3]=0;
    }
    else{
        if(chs[0]=='+'){
            ui_buf[3]=1;
        }
        else{
            ui_buf[3]=0;
        }
        str_to_hex(&chs[1], &ui_buf[2]);
        ui_buf[2]=bcd2hex_32(ui_buf[2]);
    }
    ui_buf[4]=get_env_uint("idle_bl", DEFAULT_BL);
    ui_buf[5]=get_env_uint("bl", DEFAULT_IDLE_BL);
    ui_buf[6]=get_env_uint("time_ct", 20);
#if 0
    for(int i=0;i<6;i++){
        lprintf("[%d]=%d\n", i, ui_buf[i]);
    }
#endif
    common_ui_init(vp);
}
void set_enable()
{
    char chs[32]={0};
#if 0
    for(int i=0;i<6;i++){
        lprintf("[%d]=%d\n", i, ui_buf[i]);
    }
#endif
    if(ui_buf[3]==1){
        chs[0]='+';
    }
    else{
        chs[0]='-';
    }
    slprintf(&chs[1], "%d", ui_buf[2]);
    lprintf("%s\n", chs);
    set_env_uint("random1", ui_buf[0]);
    set_env_uint("random2", ui_buf[1]);
    set_env_uint("idle_bl", ui_buf[4]);
    set_env_uint("bl", ui_buf[5]);
    set_env_uint("time_ct", ui_buf[6]);
    set_env("HsAdj1Min", chs);
    ui_transfer(UI_LAST);
}

button_t ui_set_button[]={
#ifdef LARGE_SCREEN
    {375, 520, 100,  40, set_enable, -1, 0, "OK", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#else
    {205, 220, 30,  20, set_enable, -1, 0, "OK", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#endif
};
/****end of set ui*****/

#ifdef DAC_SUPPORT
/****start of wave file ui*****/

void wav_ui_init(void*vp)
{
    int fd, file_len;
    const char* fname="MUSIC", *ename="WAV";
    SD_Error Status = SD_OK;
    SD_CardInfo mycard;

    ui_buf[0]=0xffffffff;

    {
		lprintf("sd_init\n");
		if((Status = SD_Init()) != SD_OK)
		{
			lprintf("Fail\n");
		}
		else{
			lprintf("OK\n");
			if((Status = SD_GetCardInfo(&mycard)) != SD_OK)
			{
				lprintf("get card info Fail\n");
			}
			else{
				lprintf("block size %d\n", mycard.CardBlockSize);
				lprintf("block capacity %d\n", mycard.CardCapacity);
			}
		}
    }

    lprintf("wav_ui:dac on\n");
    Dac1_Init();

    lprintf("wav_ui:open fname:%s.%s\n", fname, ename);
    memset(book_buf, 0xff, 512);
    fd = open_file(SD_ReadBlock, fname, ename, &file_len);
    if(fd < 0){
        lprintf("file open fail %d\n", fd);
        return;
    }
    ui_buf[0]=fd;
    ui_buf[1]=file_len;
    ui_buf[2]=0;//file offset
    ui_buf[3]=0;//dac data buf size
    ui_buf[4]=8;//BitsPerSample;
    ui_buf[5]=0;//dac data pointer
    ui_buf[6]=0;//pause
    ui_buf[7]=0;//file name index
    lprintf("file len %d\n", file_len);
    dac_set_freq(5500);//11k
    common_ui_init(vp);
    lcd_lprintf(20,100,"%s.%s", fname, ename);
}

void wav_next_file();
void wav_ui_process_event(void*vp)
{
    int rlen, t_sbl, ret;
    uint8_t *wd;
    uint32_t buf_limit;
    uint16_t *wd16b;

    if(0xffffffff!=ui_buf[0]&&0==ui_buf[6]){
        t_sbl=dac_get_sound_size();
        wd=(uint8_t*)ui_buf[5];
        wd16b=(uint16_t*)ui_buf[5];
        buf_limit=(uint32_t)book_buf+ui_buf[3];
        while(!dac_sound_pool_full()){
            if(0==ui_buf[5] ||
                    (uint32_t)wd16b>=buf_limit || (uint32_t)wd>=buf_limit){//read from file
                if(ui_buf[2]>=ui_buf[1]){//play end
                    //ui_buf[2]=0;//restart play
                    wav_next_file();
                    break;
                }
                rlen =ui_buf[1]-ui_buf[2];
                if(rlen>512){
                    rlen=512;
                }
                ret = read_file(ui_buf[0], book_buf, ui_buf[2], rlen);
                if(ret != FS_OK){
                    lprintf("sd wave file read fail\n");
                    close_file(ui_buf[0]);
                    ui_buf[0]=0xffffffff;
                    return;
                }
                if(0==ui_buf[2]){//file start
                    struct wave_header*whd=(struct wave_header*)book_buf;
                    lprintf("wave file info:\n");
                    lprintf("ChunkID:      0x%x\n", whd->ChunkID      );
                    lprintf("ChunkSize:      %d\n", whd->ChunkSize    );
                    lprintf("Format:       0x%x\n", whd->Format       );
                    lprintf("SubChunk1ID:  0x%x\n", whd->SubChunk1ID  );
                    lprintf("SubChunk1Size:  %d\n", whd->SubChunk1Size);
                    lprintf("AudioFormat:  0x%x\n", whd->AudioFormat  );
                    lprintf("NumChannels:  0x%x\n", whd->NumChannels  );
                    lprintf("SampleRate:     %d\n", whd->SampleRate   );
                    lprintf("ByteRate:       %d\n", whd->ByteRate     );
                    lprintf("BlockAlign:   0x%x\n", whd->BlockAlign   );
                    lprintf("BitsPerSample:0x%x\n", whd->BitsPerSample);
                    lprintf("SubChunk2ID:  0x%x\n", whd->SubChunk2ID  );
                    lprintf("SubChunk2Size:  %d\n", whd->SubChunk2Size);
                    lcd_lprintf(20,120,"%s", "wave file info:");
                    lcd_lprintf(20,140,"NumChannels:  0x%x     ", whd->NumChannels  );
                    lcd_lprintf(20,160,"SampleRate:     %d     ", whd->SampleRate   );
                    lcd_lprintf(20,180,"BitsPerSample:0x%x     ", whd->BitsPerSample);
                    if(0x46464952!=whd->ChunkID //'RIFF'
                            ||0x45564157!=whd->Format //'WAVE'
                            ||0x20746D66!=whd->SubChunk1ID  //'fmt '
                            ||0x1!=whd->AudioFormat  //PCM = 1
                      ){
                        lprintf("WARNING:not support wave file\n");
                    }
                    if(1!=whd->NumChannels){
                        lprintf("WARNING:not single channel wave file\n");
                    }
                    if(11025!=whd->SampleRate){
                        lprintf("WARNING:not 11025Hz\n");
                    }
                    dac_set_freq(whd->SampleRate/2);
                    ui_buf[4]=whd->BitsPerSample;
                    ui_buf[5]=(uint32_t)&whd->data;
                }
                else{
                    ui_buf[5]=(uint32_t)book_buf;
                }
                wd=(uint8_t*)ui_buf[5];
                wd16b=(uint16_t*)ui_buf[5];
                ui_buf[2]+=rlen;//file offset
                ui_buf[3]=rlen;//dac data buf size
                buf_limit=(uint32_t)book_buf+ui_buf[3];
            }
            if(16==ui_buf[4]){
                dac_put_sound((*wd16b+0x7fff)>>4);
                wd16b++;
            }
            else{
                dac_put_sound(*wd<<4);
                wd++;
            }
        }
        if(16==ui_buf[4]){
            ui_buf[5]=(uint32_t)wd16b;
        }
        else{
            ui_buf[5]=(uint32_t)wd;
        }
        if(0==t_sbl || g_flag_1s){
            lprintf("S+%d\n", t_sbl);
        }
    }
    common_process_event(vp);
}

void wav_ui_uninit(void*vp)
{
    if(0xffffffff!=ui_buf[0]){
        close_file(ui_buf[0]);
    }
    Dac1_DeInit();
    dac_set_freq(0);//11k
    common_ui_init(vp);
}
void wav_control()
{
    ui_buf[6]=1-ui_buf[6];
}

void wav_next_file()
{
    const char* fname="MUSIC", *ename="WAV";
    char on[8];
    int l=9, fd, file_len;
    ui_buf[6]=1;//pause play
    if(0xffffffff!=ui_buf[0]){
        close_file(ui_buf[0]);
        ui_buf[0]=0xffffffff;
    }
    while(l--){
        slprintf(on, "%s%d", fname, ui_buf[7]++);
        if(ui_buf[7]>9){
            ui_buf[7]=0;
        }
        lprintf("wav_ui:open fname:%s.%s\n", on, ename);
        fd = open_file(SD_ReadBlock, on, ename, &file_len);
        if(fd < 0){
            lprintf("file open fail %d\n", fd);
        }
        else{
            break;
        }
    }
    if(fd>0){
        ui_buf[0]=fd;
        ui_buf[1]=file_len;
        ui_buf[2]=0;//file offset
        ui_buf[3]=0;//dac data buf size
        ui_buf[4]=8;//BitsPerSample;
        ui_buf[5]=0;//dac data pointer
        ui_buf[6]=0;//pause
        lprintf("file len %d\n", file_len);
        lcd_lprintf(20,100,"%s.%s      ", on, ename);
    }
    else{
        lcd_lprintf(20,100,"No file playing   ");
    }
}

button_t wav_button[]={
#ifdef LARGE_SCREEN
    {175, 620, 180,  40, wav_next_file, -1, 0, "Next", 0, NULL},
    {175, 660, 180,  40, wav_control, -1, 0, "Pause/Play", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#else
    {205, 90, 30,  20, wav_next_file, -1, 0, "Next", 0, NULL},
    {205, 180, 30,  20, wav_control, -1, 0, "Pause/Play", 0, NULL},
    {-1,-1,-1, -1,NULL, -1, 0, NULL, 1, NULL},
#endif
};
/****end of wav file ui*****/
#endif

ui_t ui_list[]={
    {
        main_ui_init,
        main_ui_process_event,
        NULL,
        main_menu_button,
        UI_MAIN_MENU,
        40,
        0,// disp_mode
        NULL,
        NULL,
        NULL,//nedt_t*
    },
    {
        poff_ctd_ui_init,
        poff_ctd_ui_process_event,
        NULL,
        NULL,
        UI_POFF_CTD,
        10, //timeout
        TIME_OUT_EN,
        pwroff_music,//char*timeout_music;
        NULL,
        NULL,//nedt_t*
    },
    {
        timer_ui_init,
        timer_ui_process_event,
        NULL,
        NULL,
        UI_TIMER,
        10, //timeout
        0,
        xiyouji1,//char*timeout_music;
        timer_prgb,
        NULL,//nedt_t*
    },
    {
        timer_set_ui_init,
        timer_set_ui_process_event,
        NULL,
        timer_set_button,
        UI_TIMER_SET,
        20, //timeout
        TIME_OUT_EN,
        NULL,//char*timeout_music;
        NULL,
        NULL,//nedt_t*
    },
    {
        date_ui_init,
        date_ui_process_event,
        NULL,
        date_button,
        UI_DATE,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        NULL,//nedt_t*
    },
    {
        power_ui_init,
        power_ui_process_event,
        NULL,
        power_ui_button,
        UI_POWER,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        NULL,//nedt_t*
    },
    {
        sd_ui_init,
        sd_ui_process_event,
        sd_ui_uninit,
        sd_button,
        UI_SD,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        NULL,//nedt_t*
    },
    {
        date_set_ui_init,
        date_set_ui_process_event,
        NULL,
        date_set_button,
        UI_DATE_SET,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        date_set_nedt,
    },
    {
        random_ui_init,
        random_ui_process_event,
        NULL,
        random_button,
        UI_RANDOM,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        NULL,//nedt_t*
    },
    {
        set_ui_init,
        NULL,
        NULL,
        ui_set_button,
        UI_SET,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        ui_set_nedt,
    },
    {
        tipt_ui_init,
        tipt_ui_process_event,
        NULL,
        tipt_button,
        UI_TIPT,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        NULL,//nedt_t*
    },
#ifdef DAC_SUPPORT
    {
        wav_ui_init,
        wav_ui_process_event,
        wav_ui_uninit,
        wav_button,
        UI_WAV,
        220, //timeout
        0,
        NULL,//char*timeout_music;
        NULL,
        NULL,
    },
#endif
    {
        NULL,//uiinit
        NULL,//uiprocess
        NULL,//uiuninit
        NULL,//button
        UI_MAX,//ui index
        0, //timeout
        0, //time disp mode
        NULL,//char*timeout_music;
        NULL,//prgb_t*
        NULL,//nedt_t*
    },
};

void common_ui_uninit(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    (void)uif;
    cur_task_event_flag = 0;
    if(is_playing_music()){
        pause_music();
    }
}

void timer_ui_transfer_with_para(uint32_t timeout,
        uint32_t repeat, const int8_t* music)
{
    last_ui_index = cur_ui_index;
    cur_ui_index = UI_TIMER;
    if(current_ui->ui_uninit){
        current_ui->ui_uninit(current_ui);
    }
    else{
        common_ui_uninit(current_ui);
    }
    memcpy(&working_ui_t, &ui_list[UI_TIMER], sizeof(ui_t));
    current_ui = &working_ui_t;
    ui_buf[TMR_MAGIC_INDX] = TMR_MAGIC;
    ui_buf[TMR_TMOUT_INDX] = timeout;
    ui_buf[TMR_REPET_INDX] = repeat;
    ui_buf[TMR_REPETCT_INDX] = repeat;
    ui_buf[TMR_TMOUTCT_INDX] = ui_buf[TMR_TMOUT_INDX];
    current_ui->timeout_music = music;
    if(current_ui->ui_init){
        current_ui->ui_init(current_ui);
    }
    else{
        common_ui_init(current_ui);
    }
    lprintf_time("ui %u->TIMER OFTEN\r\n", last_ui_index);
}

void f3mins_timer()
{
    timer_ui_transfer_with_para(60, 3, shaolshi);
}

void often_used_timer()
{
    timer_ui_transfer_with_para(300, 3, NULL);
}

void PutPixel(int x, int y, int color)
{
    POINT_COLOR = color;
    LCD_DrawPoint(x, y);
}

#define diff(x,y) (x>y?x-y:y-x)

void draw_sq(int x1, int y1, int x2, int y2, int color)
{
    int d, x, y;
    //lprintf("%u %u %u %u %u\n", x1, y1, x2 ,y2, color);
    d = 1;
    if(x1>x2)d=-1;
    x=x1;
    do{
        x += d;
        PutPixel(x,y1,color);
        PutPixel(x,y2,color);
    }while(x!=x2);
    d = 1;
    if(y1>y2)d=-1;
    y=y1;
    do{
        y += d;
        PutPixel(x1,y,color);
        PutPixel(x2,y,color);
    }while(y!=y2);
}
void draw_sq2(int x1, int y1, int w, int h, int color)
{
    draw_sq(x1, y1, x1+w, y1+h, color);
}

void draw_prgb_raw(prgb_t*pip)
{
    if(!pip)return;
    lcd_clr_window(WHITE, pip->x, pip->y, pip->x+pip->w, pip->y+pip->h);
    draw_sq(pip->x, pip->y, pip->x+pip->w, pip->y+pip->h, prgb_color);
}

void draw_prgb(prgb_t*pip)
{
    if(!pip)return;
    while(pip->x >=0){
        draw_prgb_raw(pip);
        pip++;
    }
}

#ifdef LARGE_SCREEN
#define BUTTON_LINE_SPACE 5
#else
#define BUTTON_LINE_SPACE 2
#endif
void draw_button(button_t*pbt)
{
    uint16_t color, color_bak = LCD_PRINT_FRONT_COLOR;
    if(!pbt)return;
    int is_eng = is_english();

    while(pbt->x >=0){
        if(pbt->disable){
            color = WHITE;
            LCD_PRINT_FRONT_COLOR = WHITE;
        }
        else{
            color = BLACK;
            LCD_PRINT_FRONT_COLOR = BLACK;
        }
        int lx = MIN(pbt->x, pbt->x+pbt->w);
        int ly = MIN(pbt->y, pbt->y+pbt->h);
        draw_sq(pbt->x, pbt->y, pbt->x+pbt->w, pbt->y+pbt->h, color);
        if(pbt->ch_text && !is_eng){
            lcd_lprintf(lx+BUTTON_LINE_SPACE,ly+BUTTON_LINE_SPACE,pbt->ch_text);
        }
        else if(pbt->text){
            lcd_lprintf(lx+BUTTON_LINE_SPACE,ly+BUTTON_LINE_SPACE,pbt->text);
        }
        pbt++;
    }
    LCD_PRINT_FRONT_COLOR = color_bak;
}

void draw_nedt(nedt_t *ntt)
{
    int lx;
    uint16_t color, color_bak = LCD_PRINT_FRONT_COLOR;
    if(!ntt)return;

    lprintf("nedt_draw\n");
    while(ntt->x >=0){
        color = BLACK;
        LCD_PRINT_FRONT_COLOR = BLACK;
        lx = ntt->w/5;
        draw_sq(ntt->x, ntt->y, ntt->x+ntt->w, ntt->y+ntt->h, color);
        draw_sq(ntt->x+lx, ntt->y, ntt->x+lx*4, ntt->y+ntt->h, color);
        draw_sq(ntt->x+2*lx, ntt->y, ntt->x+lx*3, ntt->y+ntt->h, color);
        if(ntt->data){
            lcd_lprintf(ntt->x+lx*2+5, ntt->y+5, "%d", *ntt->data);
        }
        LCD_DrawLine(ntt->x+lx/2, ntt->y, ntt->x, ntt->y+ntt->h/2);
        LCD_DrawLine(ntt->x+lx/2, ntt->y+ntt->h, ntt->x, ntt->y+ntt->h/2);
        LCD_DrawLine(ntt->x+lx*4+lx/2, ntt->y, ntt->x+lx*5, ntt->y+ntt->h/2);
        LCD_DrawLine(ntt->x+lx*4+lx/2, ntt->y+ntt->h, ntt->x+lx*5, ntt->y+ntt->h/2);
        LCD_DrawLine(ntt->x+lx+lx/2, ntt->y, ntt->x+lx+lx/4, ntt->y+ntt->h/2);
        LCD_DrawLine(ntt->x+lx+lx/2, ntt->y+ntt->h, ntt->x+lx+lx/4, ntt->y+ntt->h/2);
        LCD_DrawLine(ntt->x+lx*3+lx/2, ntt->y, ntt->x+lx*3+lx*3/4, ntt->y+ntt->h/2);
        LCD_DrawLine(ntt->x+lx*3+lx/2, ntt->y+ntt->h, ntt->x+lx*3+lx*3/4, ntt->y+ntt->h/2);
        lcd_lprintf(ntt->x-lx, ntt->y+5, "%s", ntt->nedt_name);
        ntt++;
    }
    LCD_PRINT_FRONT_COLOR = color_bak;
}

void set_prgb_color(int color)
{
    prgb_color = color;
}

#ifdef LARGE_SCREEN
#define PRGB_TEXT_SPACE 30
#else
#define PRGB_TEXT_SPACE 20
#endif
void update_prgb(ui_t* uif, prgb_t*pip)
{
    (void)uif;
    int t;
    if(!pip)return;
    //lcd_clr_window(pip->b_color, pip->x, pip->y, pip->x+pip->w, pip->y+pip->h);
    while(pip->x >=0){
        if(pip->last_data != *pip->data){
            pip->last_data = *pip->data;
            t = pip->w*(*pip->data)/(*pip->max);
            if(t>=0 && t<pip->w){
                draw_prgb_raw(pip);
                lcd_clr_window(prgb_color, pip->x, pip->y, pip->x+pip->w-t, pip->y+pip->h);
            }
            if(pip->des){
                lcd_lprintf(pip->x, pip->y - PRGB_TEXT_SPACE,
                    "%s:%d/%d  ", pip->des, (*pip->data), (*pip->max));
            }
        }
        pip++;
    }
    prgb_color = BLACK;//restore color if changed
}

void process_nedt(ui_t* uif, nedt_t* ndt)
{
    (void)uif;
    if(!ndt){
        return;
    }
    lprintf("nedt_process\n");
    int x = cached_touch_x, y = cached_touch_y;
    int lx = ndt->w/5;
    while(ndt->x >=0 ){
        if(IN_RANGE(x, ndt->x, ndt->x+lx) &&
                IN_RANGE(y, ndt->y, ndt->y+ndt->h)){
            lprintf_time("in ndt l2\n");
            *ndt->data -= ndt->dl;
            if((*ndt->data<=ndt->min-1 && *ndt->data & 0x80000000)
                    || *ndt->data<ndt->min)
                *ndt->data=ndt->max;
        }
        if(IN_RANGE(x, ndt->x+lx, ndt->x+2*lx) &&
                IN_RANGE(y, ndt->y, ndt->y+ndt->h)){
            lprintf_time("in ndt l1\n");
            *ndt->data -= ndt->ds;
            if((*ndt->data<=ndt->min-1 && *ndt->data & 0x80000000)
                    || *ndt->data<ndt->min)
                *ndt->data=ndt->max;
        }
        if(IN_RANGE(x, ndt->x+3*lx, ndt->x+4*lx) &&
                IN_RANGE(y, ndt->y, ndt->y+ndt->h)){
            lprintf_time("in ndt r1\n");
            *ndt->data += ndt->ds;
            if(*ndt->data>=ndt->max+1)*ndt->data=ndt->min;
        }
        if(IN_RANGE(x, ndt->x+4*lx, ndt->x+5*lx) &&
                IN_RANGE(y, ndt->y, ndt->y+ndt->h)){
            lprintf_time("in ndt r2\n");
            *ndt->data += ndt->dl;
            if(*ndt->data>=ndt->max+1)*ndt->data=ndt->min;
        }
        if(ndt->data){
            lcd_lprintf(ndt->x+lx*2+5, ndt->y+5, "    ", *ndt->data);
            lcd_lprintf(ndt->x+lx*2+5, ndt->y+5, "%d", *ndt->data);
        }
        ndt++;
    }
}

void process_button(ui_t* uif, button_t*pbt)
{
    uint16_t x = cached_touch_x, y = cached_touch_y;
    int bt_idx=0;
    while(pbt->x >=0 ){
        if(pbt->disable){
            pbt++;
            continue;
        }
        if(IN_RANGE(x, pbt->x, pbt->x+pbt->w) &&
                IN_RANGE(y, pbt->y, pbt->y+pbt->h)){
            if(pbt->text){
                lprintf_time("in botton %s\n", pbt->text);
            }
            else{
                lprintf_time("in botton NULL text\n", pbt->text);
            }
#if 0
            draw_sq(pbt->x, pbt->y, pbt->x+pbt->w, pbt->y+pbt->h, 0x0f0f);
            lprintf("in button\n");
            udelay(100*1000);
            draw_sq(pbt->x, pbt->y, pbt->x+pbt->w, pbt->y+pbt->h, 0xffff);
#endif
            if(pbt->need_re_init_ui){
                uif->ui_init(uif);
            }
            if(pbt->click_func){
                pbt->click_func(&bt_idx);
            }
            lprintf_time("uigot %x\n", pbt->ui_goto);
            if(pbt->ui_goto != -1){
                ui_transfer(pbt->ui_goto);
            }
        }
        pbt++;
        bt_idx++;
    }
}

void common_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    button_t* p_bt = uif->button_info;
    if(uif->time_disp_mode & TIMER_TRIGGER_START){
        cur_task_timer_started = false;
    }
    else{
        cur_task_timer_started = true;
    }
    LCD_Clear(WHITE);	//fill all screen with some color
    draw_button(p_bt);
    draw_button(common_button);
    draw_prgb(uif->prgb_info);
    draw_nedt(uif->nedt_info);
    //power bar
    power_prgb[0].max = &power_max_display;
    power_prgb[0].data = &power_display;
    power_prgb[0].last_data = 0;
    draw_prgb(&power_prgb[0]);
    show_sound_status();

#if 0
    if(uif->time_disp_mode & TIMER_TRIGGER_START){
        cur_task_timer_started = false;
    }
    else{
        cur_task_timer_started = true;
    }
#endif
    cur_task_timeout_ct = uif->timeout;
    //if(uif->time_disp_mode & TIME_OUT_INPUT){
        //cur_task_timeout_ct = input_timeout;
    //}
    //led_flash_per_second = true;
    lprintf("cur task timect---init %x\r\n", cur_task_timeout_ct);
    cur_task_event_flag = 0;
    no_touch_down_ct = 0;
    power_indicator_counter = POWER_INDICATOR_CYCLE;
}

void ui_transfer(uint8 ui_id)
{
    if(ui_id == UI_LAST){
        if(last_ui_index == cur_ui_index){
            return;
        }
        else{
            //uint8 tmp = cur_ui_index;
            cur_ui_index = last_ui_index;
            last_ui_index = 0;
        }
    }
    else{
        last_ui_index = cur_ui_index;
        cur_ui_index = ui_id;
    }
    if(current_ui->ui_uninit){
        current_ui->ui_uninit(current_ui);
    }
    else{
        common_ui_uninit(current_ui);
    }
    memcpy(&working_ui_t, &ui_list[cur_ui_index], sizeof(ui_t));
    current_ui = &working_ui_t;
    if(current_ui->ui_init){
        current_ui->ui_init(current_ui);
    }
    else{
        common_ui_init(current_ui);
    }
    lprintf_time("%s\n", get_rtc_time(NULL));
    lprintf_time("ui %u->%u\n", last_ui_index, ui_id);
}

void common_process_event(void*vp)
{
    //bool dg = g_flag_1s;
    ui_t* uif =(ui_t*)vp;
    int32_t uii_bak=uif->ui_index;
#if 0
    if(cur_task_event_flag && !is_playing_music()){
        if(keyA2_up){
            play_music_note(50, 50);
        }
        if(keyA4_up){
            play_music_note(5, 50);
        }
        if(keyA1_up || keyA3_up){
            play_music_note(1, 50);
        }
        if(!(uif->time_disp_mode & NO_LED_FLASH_EVENT)){
            flash_led(2, 5);
        }
    }
#endif
    for(int8 i = 0; i < EVENT_MAX; i++){
        uint32_t evt_flag=1<<i;
        //if(dg) lprintf("ev flag %x %x i %x\r\n", cur_task_event_flag, evt_flag, i);
        if(cur_task_event_flag & evt_flag){
#if 0
            if(uif->ui_event_transfer[i]>=0){
                ui_transfer(uif->ui_event_transfer[i]);
                return;
            }
            if(uif->ui_event_transfer[i]==UI_TRANSFER_DEFAULT){
            }
            if(uif->ui_event_transfer[i]==UI_RESET_TIMEOUT){
                if(uif->time_disp_mode & TIME_OUT_INPUT){
                    cur_task_timeout_ct = input_timeout;
                }
                else{
                    cur_task_timeout_ct = uif->timeout;
                }
            }
#endif
            //lprintf("ev flag %x EVUTO %x\r\n", evt_flag, EVENT_UI_TIMEOUT);
            if(evt_flag == (1<<EVENT_UI_TIMEOUT)){
                lprintf_time("UI timeout\n");
                if(uif->timeout_music){
                    play_music(uif->timeout_music, 0);
                }
                else{
                    if(UI_POFF_CTD != cur_ui_index){
                        ui_transfer(UI_POFF_CTD);
                        return;
                    }
                }
            }
            if(evt_flag == (1<<EVENT_TOUCH_UP)){
                button_t* p_bt = current_ui->button_info;
                process_button(uif, p_bt);
                if(uii_bak!=current_ui->ui_index){
                    return;
                }
                process_button(uif, common_button);
                if(uii_bak!=current_ui->ui_index){
                    return;
                }
                process_nedt(uif, current_ui->nedt_info);
            }
            if(evt_flag == (1<<EVENT_BATT_LOW)){
                lprintf_time("Battary low\n");
                if(UI_POFF_CTD != cur_ui_index){
                    ui_transfer(UI_POFF_CTD);
                    return;
                }
            }
        }
    }
    //power_display = v_bat - BATT_LOW_LIMIT;
    if(g_flag_1s){
        power_indicator_counter++;
        if(power_indicator_counter > POWER_INDICATOR_CYCLE){
            power_indicator_counter = 0;
            power_display = BATT_MAX - v_bat;
            if(v_bat<BATT_LOW_ALERT){
                set_prgb_color(RED);
                play_music_note(1, 100);
            }
            update_prgb(uif, &power_prgb[0]);
        }
    }
    cur_task_event_flag = 0;
}

void ui_start()
{
#ifdef LARGE_SCREEN
    lprintf_time("ui large_screen start\n");
#else
    lprintf_time("ui small_screen start\n");
#endif
    memcpy(&working_ui_t, &ui_list[0], sizeof(ui_t));
    current_ui = & working_ui_t;
    if(current_ui->ui_init){
        current_ui->ui_init(current_ui);
    }
    else{
        common_ui_init(current_ui);
    }
}

void task_ui(struct task*vp)
{
    (void)vp;//fix unused variable warning
    if(current_ui->ui_process_event){
        current_ui->ui_process_event(current_ui);
    }
    else{
        common_process_event(current_ui);
    }
}

