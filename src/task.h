#ifndef _TASK_H
#define _TASK_H

#include "type.h"

extern uint16_t cached_touch_x;
extern uint16_t cached_touch_y;
extern uint no_touch_down_ct;
extern uint g_flag_1s ;
extern uint g_flag_10ms ;
#define NO_KEY_DOWN_CT_MAX 100
#define NO_KEY_DOWN_PWSAVE_MAX 100
#define LCD_POWER_SAVE_CYCLE 60
#define LCD_POWER_SAVE_RATIO 15
enum EVENT_TYPE{
    EVENT_KEYA1_UP,
    EVENT_KEYA2_UP,
    EVENT_KEYA3_UP,
    EVENT_KEYA4_UP,
    EVENT_TOUCH_DOWN,
    EVENT_TOUCH_UP,
    EVENT_UI_TIMEOUT,
    EVENT_MUSIC_PLAY_END,
    EVENT_NOKEYCT_MAXREACHED,
    EVENT_BATT_LOW,
    EVENT_MAX
};
typedef void (*func_p)(void*);
struct task;
typedef void (*task_func)(struct task*);
struct task {
    task_func t_func;
    //char flag_1s;
};
struct delay_work_info {
    func_p function;
    uint ct_10ms;
    void * para;
};
extern struct task all_tasks[];
extern uint8 cur_task_timer_started;
extern uint cur_task_timeout_ct;
extern uint32_t cur_task_event_flag;
extern uint8 cursor_cmd ;
extern stop_feed_wtd;
extern disp_mem_update ;

void task_ui(struct task*v);
void task_key_status(struct task*v);
void task_timer(struct task*v);
void task_disp(struct task*v);
void task_music(struct task*v);
void task_power(struct task*v);
void task_lcd_bklight(struct task*vp);
void task_misc(struct task*v);

void set_delayed_work(uint tct, func_p f, void*pa);
void set_music_note_period(uint p);
void reset_music_note();
uint8 play_music_note(int8 note, uint period);
void pause_music();
void continue_music();
uint8 is_playing_music();
void play_music(const signed char* pu, uint note_period);
void time_hms(char*buf, uint t);
void local_float_sprintf(struct s_lfs_data* lfsd);
#endif
