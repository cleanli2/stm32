#include "common.h"
#include "music.h"
#include "ui.h"

ui_t working_ui_t;
int cur_ui_index = 0;
int last_ui_index = 0;
ui_t*current_ui;
uint32_t ui_buf[8];

void often_used_timer();
void f3mins_timer();
void common_process_event(void*vp);
void ui_transfer(uint8 ui_id);
void common_ui_init(void*vp);
void draw_prgb(prgb_t*pip);

void timer_set_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    common_ui_init(vp);
    ui_buf[8] = 1;
    lcd_lprintf(60, 280, "Repeat %d times   ", ui_buf[8]);
}
void timer_set_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    common_process_event(vp);
}
void main_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    lprintf("mainmenu\n");
    if(check_rtc_alert_and_clear()){
        often_used_timer();
    }
    common_ui_init(vp);
}
void main_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    if(cur_task_event_flag & (1<<EVENT_NOKEYCT_MAXREACHED)){
        ui_transfer(UI_POFF_CTD);
    }
    common_process_event(vp);
}
void poff_ctd_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    common_ui_init(vp);
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, 20, 140);
    lcd_lprintf(20, 100, "Version:%s%s", VERSION, GIT_SHA1);
}
void poff_ctd_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    if(cur_task_event_flag & (1<<EVENT_MUSIC_PLAY_END)){
        power_off();
    }
    if(g_flag_1s){
        play_music_note(8, 100);
    }
    common_process_event(vp);
}

prgb_t timer_prgb[]={
    {20, 200, 440, 30, NULL, NULL, "Counter", 0},
    {20, 280, 440, 30, NULL, NULL, "Repeat", 0},
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
    draw_prgb(timer_prgb);
    update_prgb(uif, uif->prgb_info);
}
void timer_ui_process_event(void*vp)
{
    uint32_t mins, secs;
    ui_t* uif =(ui_t*)vp;
    if(cur_task_event_flag & (1<<EVENT_MUSIC_PLAY_END)
            && ui_buf[TMR_REPETCT_INDX]==0){
        ui_transfer(UI_MAIN_MENU);
    }
    if(g_flag_1s){
        set_LCD_Char_scale(6);
        mins = ui_buf[TMR_TMOUTCT_INDX]/60;
        secs = ui_buf[TMR_TMOUTCT_INDX] -  mins*60;
        lcd_lprintf(100, 400, "%d:%d   ", mins, secs);
        lcd_lprintf(100, 520, "RP:%d ", ui_buf[TMR_REPETCT_INDX]);
        set_LCD_Char_scale(1);
        if(ui_buf[TMR_REPETCT_INDX]==0){
            update_prgb(uif, uif->prgb_info);
            if(uif->timeout_music){
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
            }
            ui_buf[TMR_TMOUTCT_INDX]--;
        }
    }
    common_process_event(vp);
}
void timer_ui_uninit(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    ui_buf[TMR_MAGIC_INDX] = 0;
    lprintf("clr TMR_MAGIC_INDX\n");
}
button_t common_button[]={
    {10,730,200, 60, NULL, UI_LAST, 0, "RETURN"},
    {270,730,200, 60, NULL, 0, 0, "HOME"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};

void music_test()
{
    play_music(YouJianChuiYan, 0);
}

button_t main_menu_button[]={
    {130,40,200, 60, often_used_timer, -1, 0, "OftenTimer"},
    {130,110,200, 60, NULL, UI_POFF_CTD, 0, "PowerOffCountDown"},
    {130,180,200, 60, soft_reset_system, -1, 0, "Reboot"},
    {130,250,200, 60, reboot_download, -1, 0, "RebootDownload"},
    {130,320,200, 60, power_off, -1, 0, "PowerOff"},
    {130,390,200, 60, music_test, -1, 0, "MusicTest"},
    {130,460,200, 60, f3mins_timer, -1, 0, "3x1mins TIMER"},
    {130,530,200, 60, NULL, UI_TIMER_SET, 0, "More Timer"},
    {130,600,200, 60, NULL, UI_DATE, 0, "Date&Time"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
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
    { 60, 60, 80,  40, mins_2, -1, 0, "2 mins"},
    {160, 60, 80,  40, mins_3, -1, 0, "3 mins"},
    {260, 60, 80,  40, mins_5, -1, 0, "5 mins"},
    {360, 60, 80,  40, mins_10, -1, 0, "10 mins"},
    { 60,120, 80,  40, mins_20, -1, 0, "20 mins"},
    {160,120, 80,  40, mins_30, -1, 0, "30 mins"},
    {260,120, 80,  40, mins_45, -1, 0, "45 mins"},
    {360,120, 80,  40, mins_1h, -1, 0, "1h+3 mins"},
    { 60,200, 80,  40, rp_2, -1, 0, "repeat 2"},
    {160,200, 80,  40, rp_3, -1, 0, "repeat 3"},
    {260,200, 80,  40, rp_4, -1, 0, "repeat 4"},
    {360,200, 80,  40, rp_1, -1, 0, "repeat 1"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};

/*UI DATE*/
button_t date_button[];
struct point
{
    uint16_t px;
    uint16_t py;
};

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
}
void date_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    lprintf("data ui\n");
    common_ui_init(vp);
    lcd_lprintf(0, 20, "Version:%s%s", VERSION, GIT_SHA1);
    if(check_rtc_alert_and_clear()){
        often_used_timer();
    }
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, 260, 45);
    draw_clock_face(240, 400);
}
void date_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;

    if(g_flag_1s){
        set_LCD_Char_scale(2);
        lcd_lprintf(10, 100, "%s  ", get_rtc_time(NULL));
        set_LCD_Char_scale(1);
        lprintf("task timect %x\r\n", cur_task_timeout_ct);
    }
    common_process_event(vp);
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

button_t date_button[]={
    {15, 660, 100,  40, adjust_enable, -1, 0, "time adjust", 0},
    {135, 660, 100,  40, fast_1, -1, 0, "faster 1min", 1},
    {255, 660, 100,  40, slow_1, -1, 0, "slower 1min", 1},
    {375, 660, 100,  40, clr_s, -1, 0, "clear second", 1},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};
/*UI DATE END*/

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
    },
    {
        date_ui_init,
        date_ui_process_event,
        NULL,
        date_button,
        UI_DATE,
        220, //timeout
        TIME_OUT_EN,
        NULL,//char*timeout_music;
        NULL,
    },
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
    },
};

void common_ui_uninit(void*vp)
{
    ui_t* uif =(ui_t*)vp;
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
    lprintf("ui %u->TIMER OFTEN\r\n", last_ui_index);
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

void draw_prgb_raw(prgb_t*pip)
{
    if(!pip)return;
    lcd_clr_window(WHITE, pip->x, pip->y, pip->x+pip->w, pip->y+pip->h);
    draw_sq(pip->x, pip->y, pip->x+pip->w, pip->y+pip->h, BLACK);
}

void draw_prgb(prgb_t*pip)
{
    if(!pip)return;
    while(pip->x >=0){
        draw_prgb_raw(pip);
        pip++;
    }
}

void draw_button(button_t*pbt)
{
    uint16_t color, color_bak = LCD_PRINT_FRONT_COLOR;
    if(!pbt)return;
    while(pbt->x >=0){
        if(pbt->disable){
            color = WHITE;
            LCD_PRINT_FRONT_COLOR = WHITE;
        }
        else{
            color = BLACK;
            LCD_PRINT_FRONT_COLOR = BLACK;
        }
        draw_sq(pbt->x, pbt->y, pbt->x+pbt->w, pbt->y+pbt->h, color);
        if(pbt->text){
            int lx = MIN(pbt->x, pbt->x+pbt->w);
            int ly = MIN(pbt->y, pbt->y+pbt->h);
            lcd_lprintf(lx+5,ly+5,pbt->text);
        }
        pbt++;
    }
    LCD_PRINT_FRONT_COLOR = color_bak;
}

void update_prgb(ui_t* uif, prgb_t*pip)
{
    uint32_t t;
    if(!pip)return;
    //lcd_clr_window(pip->b_color, pip->x, pip->y, pip->x+pip->w, pip->y+pip->h);
    while(pip->x >=0){
        if(pip->last_data != *pip->data){
            pip->last_data = *pip->data;
            t = pip->w*(*pip->data)/(*pip->max);
            if(t>=0 && t<pip->w){
                lcd_clr_window(BLACK, pip->x, pip->y, pip->x+pip->w-t, pip->y+pip->h);
            }
            lcd_lprintf(pip->x, pip->y - 30,
                    "%s:%d/%d  ", pip->des, (*pip->data), (*pip->max));
        }
        pip++;
    }
}

void process_button(ui_t* uif, button_t*pbt)
{
    uint16_t x = cached_touch_x, y = cached_touch_y;
    while(pbt->x >=0 ){
        if(pbt->disable){
            pbt++;
            continue;
        }
        if(IN_RANGE(x, pbt->x, pbt->x+pbt->w) &&
                IN_RANGE(y, pbt->y, pbt->y+pbt->h)){
            lprintf("in botton %s\n", pbt->text);
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
                pbt->click_func(NULL);
            }
            lprintf("uigot %x\n", pbt->ui_goto);
            if(pbt->ui_goto != -1){
                ui_transfer(pbt->ui_goto);
            }
        }
        pbt++;
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
    lprintf("ui %u->%u\r\n", last_ui_index, ui_id);
}

void common_process_event(void*vp)
{
    //bool dg = g_flag_1s;
    ui_t* uif =(ui_t*)vp;
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
        uint8 evt_flag=1<<i;
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
                process_button(uif, common_button);
            }
        }
    }
    cur_task_event_flag = 0;
}

void ui_start()
{
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
    vp;//fix unused variable warning
    if(current_ui->ui_process_event){
        current_ui->ui_process_event(current_ui);
    }
    else{
        common_process_event(current_ui);
    }
}

