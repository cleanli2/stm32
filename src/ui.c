#include "common.h"
#include "ui.h"

ui_t working_ui_t;
int cur_ui_index = 0;
int last_ui_index = 0;
void main_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    common_process_event(vp);
}
void poff_ctd_ui_process_event(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    common_process_event(vp);
}
button_t common_button[]={
    {10,730,200, 60, NULL, UI_LAST, 0, "RETURN"},
    {270,730,200, 60, NULL, 0, 0, "HOME"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};
button_t main_menu_button[]={
    //{130,70,200, 95, NULL, UI_CLOCK, 0, "CLOCK"},
    {130,90,200, 60, NULL, UI_POFF_CTD, 0, "PowerOffCountDown"},
    {130,150,200, 60, soft_reset_system, -1, 0, "Reboot"},
    {130,220,200, 60, reboot_download, -1, 0, "RebootDownload"},
    {130,290,200, 60, power_off, -1, 0, "PowerOff"},
    //{130,210,200, 190, exit_ui, -1, 0, "Exit"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};

ui_t ui_list[]={
    {
        NULL,
        main_ui_process_event,
        NULL,
        main_menu_button,
        UI_MAIN_MENU,
        0,
    },
    {
        NULL,
        poff_ctd_ui_process_event,
        NULL,
        NULL,
        UI_POFF_CTD,
        10, //timeout
    },

#if 0
    {
        NULL,
        NULL,
        clock_doing_ui,
        clock_button,
        UI_CLOCK,
    },
#endif
};

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

ui_t*current_ui;
void common_ui_init(void*vp)
{
    ui_t* uif =(ui_t*)vp;
    button_t* p_bt = uif->button_info;
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
                if(last_ui_index != cur_ui_index){
                    ui_transfer(last_ui_index);
                    return;
                }
            }
            if(uif->ui_event_transfer[i]==UI_RESET_TIMEOUT){
                if(uif->time_disp_mode & TIME_OUT_INPUT){
                    cur_task_timeout_ct = input_timeout;
                }
                else{
                    cur_task_timeout_ct = uif->timeout;
                }
            }
            //lprintf("ev flag %x EVUTO %x\r\n", evt_flag, EVENT_UI_TIMEOUT);
            if(evt_flag == (1<<EVENT_UI_TIMEOUT) && uif->timeout_music){
                play_music(uif->timeout_music, 0);
            }
#endif
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

