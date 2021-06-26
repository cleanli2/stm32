#ifndef _UI_H
#define _UI_H
#include "task.h"
#include "type.h"
#include "common.h"

#define TIME_DISP_EN (1<<0)
#define TIME_DISP_SECOND (1<<1)
#define TIME_DISP_LEFT (1<<2)
#define TIME_OUT_INPUT (1<<3)
#define TIME_OUT_EN (1<<4)
#define TIMER_TRIGGER_START (1<<5)
#define NO_LED_FLASH_EVENT (1<<6)

#define UI_TRANSFER_DEFAULT (-2)
#define UI_RESET_TIMEOUT (-3)

enum UI_NAME_INDEX {
    UI_MAIN_MENU,
    UI_ADC,
    UI_CLOCK,
    UI_MAX
};

extern int8 cur_ui_index ;
extern int8 last_ui_index ;

typedef struct button {
    int x;
    int y;
    int w;
    int h;
    func_p click_func;
    int ui_goto;
    int need_re_init_ui;
    const char* text;
} button_t;

typedef struct ui_info {
#if 0
    const char* ui_name;
    uint timeout;
    uint8 time_disp_mode;
    uint8 time_position_of_dispmem;
    uint8 power_position_of_dispmem;
    int8 ui_event_transfer[EVENT_MAX];
    const signed char*timeout_music;
#endif
    func_p ui_init;
    func_p ui_process_event;
    func_p ui_uninit;
    button_t* button_info;
    int ui_index;
    uint timeout;
} ui_t;

typedef struct change_level_info_{
    uint factor;
    uint min_adder;
    uint max_adder;
    uint8 cursor_jump;
    uint8 max_cursor_posi;
    uint8 min_cursor_posi;
    uint8 switch_cursor_type;
} change_level_info;

enum SWITCH_CURSOR_TYPE{
    SWITCH_CURSOR_BY_DOUBLE_KEY,
    SWITCH_CURSOR_BY_LONG_PRESS,
    SWITCH_CURSOR_BY_LEFT_KEY,
};

void ui_start();
#endif
