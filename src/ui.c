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
void main_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    if(check_rtc_alert_and_clear()){
        often_used_timer();
    }
    common_ui_init(vp);
}
void main_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    common_process_event(vp);
}
void poff_ctd_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    common_ui_init(vp);
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS);
    lcd_lprintf(20, 100, "Version:%s", VERSION);
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
#define TMR_TMOUT_INDX 0
#define TMR_REPET_INDX 1
#define TMR_TMOUTCT_INDX 2
#define TMR_MAGIC_INDX 3
void timer_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    play_music(notice_music, 0);
    if(ui_buf[TMR_MAGIC_INDX] != 0xF1EE4){
        ui_buf[TMR_TMOUT_INDX] = uif->timeout;
        ui_buf[TMR_REPET_INDX] = 2;
        ui_buf[TMR_TMOUTCT_INDX] = ui_buf[TMR_TMOUT_INDX];
    }
    else{
        ui_buf[TMR_MAGIC_INDX] = 0;
    }
    common_ui_init(vp);
}
void timer_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    if(cur_task_event_flag & (1<<EVENT_MUSIC_PLAY_END)
            && ui_buf[TMR_REPET_INDX]==0){
        ui_transfer(UI_MAIN_MENU);
    }
    if(g_flag_1s){
        if(ui_buf[TMR_REPET_INDX]==0){
            if(uif->timeout_music){
                play_music(uif->timeout_music, 0);
            }
        }
        else{
            ui_buf[TMR_TMOUTCT_INDX]--;
            if(ui_buf[TMR_TMOUTCT_INDX] == 0){
                ui_buf[TMR_REPET_INDX]--;
                ui_buf[TMR_TMOUTCT_INDX] = ui_buf[TMR_TMOUT_INDX];
                play_music(notice_music, 0);
            }
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
    //{130,210,200, 190, exit_ui, -1, 0, "Exit"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};

ui_t ui_list[]={
    {
        main_ui_init,
        main_ui_process_event,
        NULL,
        main_menu_button,
        UI_MAIN_MENU,
        20,
        TIME_OUT_EN,// disp_mode
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
    },
    {
        NULL,
        NULL,
        NULL,
        NULL,
        UI_MAX,
        0, //timeout
        0,
        NULL,//char*timeout_music;
    },
};

void common_ui_uninit(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    cur_task_event_flag = 0;
}

void timer_ui_transfer_with_para(uint32_t timeout,
        uint32_t repeat, int8_t* music)
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
    ui_buf[TMR_MAGIC_INDX] = 0xF1EE4;
    ui_buf[TMR_TMOUT_INDX] = timeout;
    ui_buf[TMR_REPET_INDX] = repeat;
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

void draw_button(button_t*pbt)
{
    if(!pbt)return;
    while(pbt->x >=0){
        draw_sq(pbt->x, pbt->y, pbt->x+pbt->w, pbt->y+pbt->h, BLACK);
        if(pbt->text){
            int lx = MIN(pbt->x, pbt->x+pbt->w);
            int ly = MIN(pbt->y, pbt->y+pbt->h);
            lcd_lprintf(lx+5,ly+5,pbt->text);
        }
        pbt++;
    }
}

void process_button(ui_t* uif, button_t*pbt)
{
    uint16_t x = cached_touch_x, y = cached_touch_y;
    while(pbt->x >=0){
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
            uint8 tmp = cur_ui_index;
            cur_ui_index = last_ui_index;
            last_ui_index = tmp;
        }
    }
    else{
        last_ui_index = cur_ui_index;
        cur_ui_index = ui_id;
    }
    if(current_ui->ui_uninit){
        current_ui->ui_uninit(current_ui);
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

