#include "common.h"
#include "ui.h"

button_t main_menu_button[]={
    //{130,70,200, 95, NULL, UI_CLOCK, 0, "CLOCK"},
    //{130,110,200, 135, NULL, UI_ADC, 0, "ADC"},
    {130,150,200, 60, soft_reset_system, -1, 0, "Reboot"},
    {130,220,200, 60, reboot_download, -1, 0, "RebootDownload"},
    {130,290,200, 60, power_off, -1, 0, "PowerOff"},
    //{130,210,200, 190, exit_ui, -1, 0, "Exit"},
    {-1,-1,-1, -1,NULL, -1, 0, NULL},
};

ui_t ui_list[]={
    {
        NULL,
        NULL,
        NULL,
        main_menu_button,
        UI_MAIN_MENU,
    },
#if 0
    {
        adc_ui_init,
        NULL,
        NULL,
        adc_ctr_button,
        UI_ADC,
    },

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

#define MIN(x,y) ((x)<(y)?(x):(y))
ui_t*current_ui;
void ui_start()
{
    button_t* p_bt = current_ui->button_info;
    if(current_ui->init){
        current_ui->init(NULL);
    }
    else{
        LCD_Clear(WHITE);	//fill all screen with some color
    }
    //lprintf("but x %x\n", p_bt->x);
    while(p_bt->x >=0){
        draw_sq(p_bt->x, p_bt->y, p_bt->x+p_bt->w, p_bt->y+p_bt->h, BLACK);
        if(p_bt->text){
            int lx = MIN(p_bt->x, p_bt->x+p_bt->w);
            int ly = MIN(p_bt->y, p_bt->y+p_bt->h);
            lcd_lprintf(lx+5,ly+5,p_bt->text);
        }
        p_bt++;
    }
}

void ui_init()
{
    current_ui = & ui_list[0];
    ui_start();
}

#define IN_RANGE(x, x1, x2) ((((x1)<(x2))&&((x1)<(x))&&((x)<(x2))) ||\
    (((x2)<(x1))&&((x2)<(x))&&((x)<(x1))))
void task_ui(struct task*vp)
{
    vp;//fix unused variable warning
    //current_ui->ui_process_event(current_ui);
    uint16_t x, y;
    button_t* p_bt = current_ui->button_info;
    if(get_TP_point(&x, &y)){
        while(p_bt->x >=0){
            if(IN_RANGE(x, p_bt->x, p_bt->x+p_bt->w) &&
                    IN_RANGE(y, p_bt->y, p_bt->y+p_bt->h)){
#if 0
                draw_sq(p_bt->x, p_bt->y, p_bt->x+p_bt->w, p_bt->y+p_bt->h, 0x0f0f);
                lprintf("in button\n");
                udelay(100*1000);
                draw_sq(p_bt->x, p_bt->y, p_bt->x+p_bt->w, p_bt->y+p_bt->h, 0xffff);
#endif
                if(p_bt->need_re_init_ui){
                    ui_start();
                }
                if(p_bt->click_func){
                    p_bt->click_func(NULL);
                }
#if 0
                lprintf("%x\n", p_bt->ui_goto);
                if(p_bt->ui_goto != -1){
                    ui_transfer(p_bt->ui_goto);
                }
#endif
            }
            p_bt++;
        }
    }
}

