#include "display.h"
#include "common.h"
#include "ring_buf.h"

DECLARE_OS_LOCK(oslk_disp_para, DISP_RB_LOCK);
DECLARE_RB_DATA(disp_func_para, rb_disp_para, 2)

const char* Proxy_Show_Str_win_raw(u32 *xp, u32 *yp, u32 fc, u32 bc, const char *str, u32 size, u32 mode, win_pt wd, int is_dummy)
{
    u32 xbak, ybak;
    const char* ret;
    disp_func_para*rbdpp;

    xbak = *xp;
    ybak = *yp;
    ret = Show_Str_win_raw(xp, yp, fc, bc, str, size, mode, wd, 1);
    if(is_dummy){
        return ret;
    }
    //do real display
    os_lock(&oslk_disp_para);
    rbdpp=RB_W_GET_wait(disp_func_para, rb_disp_para);
    rbdpp->type = DISPFUNC_WIN_STR;
    rbdpp->win_str_para.p_x=xbak;
    rbdpp->win_str_para.p_y=ybak;
    rbdpp->win_str_para.fc=fc;
    rbdpp->win_str_para.bc=bc;
    rbdpp->win_str_para.str=str;
    rbdpp->win_str_para.size=size;
    rbdpp->win_str_para.mode=mode;
    rbdpp->win_str_para.df_win=*wd;
    /*
    rbdpp->win_str_para->df_win.x=wd->x;
    rbdpp->win_str_para->df_win.y=wd->y;
    rbdpp->win_str_para->df_win.w=wd->w;
    rbdpp->win_str_para->df_win.h=wd->h;
    rbdpp->win_str_para->df_win.dx=wd->dx;
    rbdpp->win_str_para->df_win.dy=wd->dy;
    */
    RB_W_SET(disp_func_para, rb_disp_para);
    os_unlock(&oslk_disp_para);
    return ret;
}

void os_task_display(void*p)
{
    (void)p;
    disp_func_para*rbdpp;
    u32 *xp, *yp, fc, bc, size, mode;
    const char *str;
    win_pt wd;
    while(1){
        os_lock(&oslk_disp_para);
        rbdpp=RB_R_GET_wait(disp_func_para, rb_disp_para);
        switch(rbdpp->type){
            case DISPFUNC_WIN_STR:
                xp=&rbdpp->win_str_para.p_x;
                yp=&rbdpp->win_str_para.p_y;
                fc=rbdpp->win_str_para.fc;
                bc=rbdpp->win_str_para.bc;
                str=rbdpp->win_str_para.str;
                size=rbdpp->win_str_para.size;
                mode=rbdpp->win_str_para.mode;
                wd=&rbdpp->win_str_para.df_win;
                Show_Str_win_raw(xp, yp, fc, bc, str, size, mode, wd, 0);
                break;
            default:
                lprintf("unknow dispfunc type\n");
        };
        RB_R_SET(disp_func_para, rb_disp_para);
        os_unlock(&oslk_disp_para);
    }
}
