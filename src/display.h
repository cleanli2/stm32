#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "type.h"
#include "gui.h"

extern u16 PROXY_POINT_COLOR;
enum disp_func_type {
    DISPFUNC_WIN_STR,
    DISPFUNC_WIN_CLR,
    DISPFUNC_DRAW_POINT,
    DISPFUNC_DRAW_SQ,
    DISPFUNC_DRAW_LINE,
    MAX_NUM_DISPFUNC
};

typedef struct _disp_func_para {
    u32 type;
    union _data {
        struct _win_str_para {
            u32 p_x;
            u32 p_y;
            u16 fc;
            u16 bc;
            const char*str;
            u16 size;
            u16 mode;
            win df_win;
        } win_str_para;
        struct _win_clr_para {
            u16 color;
            u16 xs;
            u16 ys;
            u16 xe;
            u16 ye;
        } win_clr_para;
        struct _draw_point_para {
            u16 color;
            u16 x;
            u16 y;
        } draw_point_para;
        struct _draw_sq_para {
            u16 color;
            u16 x1;
            u16 y1;
            u16 x2;
            u16 y2;
        } draw_sq_para;
        struct _draw_line_para {
            u16 x1;
            u16 y1;
            u16 x2;
            u16 y2;
        } draw_line_para;
    }data;
} disp_func_para;

void os_task_display(void*p);

const char* Proxy_Show_Str_win_raw(u32 *xp, u32 *yp, u32 fc, u32 bc, const char *str, u32 size, u32 mode, win_pt wd, int is_dummy);
void Proxy_lcd_clr_window(u16 color, u16 xs, u16 ys, u16 xe, u16 ye);
void Proxy_LCD_DrawPoint(u16 x,u16 y);
void Proxy_draw_sq(int x1, int y1, int x2, int y2, int color);
void Proxy_LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
#endif
