#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "type.h"
#include "gui.h"

enum disp_func_type {
    DISPFUNC_WIN_STR,
    MAX_NUM_DISPFUNC
};

typedef union _disp_func_para {
    enum disp_func_type type;
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
} disp_func_para;

void os_task_display(void*p);

const char* Proxy_Show_Str_win_raw(u32 *xp, u32 *yp, u32 fc, u32 bc, const char *str, u32 size, u32 mode, win_pt wd, int is_dummy);
#endif
