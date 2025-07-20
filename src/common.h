#ifndef __COMMON_H
#define __COMMON_H
#include"board_config.h"
#include"stm32f10x.h"
#include"type.h"
#include"version.h"
#include<stdint.h>
#include<string.h>
#include "lprintf.h"
#include "lcd_sueb.h"
#include "cmd.h"
#include "adc.h"
#include "gui.h"
#include "rtc.h"
#include "touch.h"
#include "sd_lowlevel.h"
#include "stm32_eval_spi_sd.h"
#include "env.h"
#include "cch_str.h"
#include "w25flash.h"
#include "lcd800x480/test.h"
#include "date.h"
#include "soc_rtc.h"
#include "ui.h"
#include "music.h"
#include "os_task.h"

//for camera
#define RECORD_CAM_STUCK_SIZE 8

extern u32 interv_systick;
extern u32 g_cam_r70p_e;
extern u32 g_cam_r71p_e;
extern u32 g_tlcd;
#define TIM2_RELOAD 60000
enum intnum {
    INTSYSTICK,
    INTTIM2,
    INTTIM3,
    INTPENDSV,
    INTUART1,
    NUM_INTRPT
};
extern u32 intrpt_time[NUM_INTRPT];
#define tm_cpt_start() u32 t_start=TIM_GetCounter(TIM2)
#define tm_cpt_end() \
    ({u32 t_end=TIM_GetCounter(TIM2); \
    (t_end>t_start?t_end-t_start:t_end+TIM2_RELOAD-t_start);})


struct emulate_touch {
    u32 n_pt;
    u32 cur_n_pt;
    u32 cur_interval;
    u32 interval;
    u32 start;
    u32 last;
    struct point* pts;
};

extern struct emulate_touch *gftp;
extern uint32_t logv;

extern uint32_t g_ms_count;
void delay_us(u32 nus);
void delay_ms(uint16_t nms);
void w10ms_delay(u32 ct);
void led_flash(u32 led_flag, u32 ms_ct);
void soft_reset_system();
void power_off();
void reboot_download();
uint64_t get_system_us();
u32 dt_us_last();
void beep_by_timer_100(uint32_t hz_100);
char * str_to_hex(char *s, uint32_t * result);
void main_init(void);
void enable_sound(int en);
int get_sound_sta();
uint add_with_limit(uint a, uint b, uint limit);
uint sub_with_limit(uint a, uint b, uint limit);
uint8_t pcf8574t_writeData(uint8_t mdata);
uint8_t pcf8574t_readData1(uint8_t * buff);

#define LED_R_C 0x1
#define LED_Y_C 0x2
#define LED_R_1 0x4
#define LED_Y_1 0x8
#define LED_R_0 0
#define LED_Y_0 0
#define MEM_SD_SIZE 0x2000
#define usb_writable (0x80000000&logv)
void led_raw_set(u32 led_flag);
void toggle_led(int i);
void usb_main_init(uint32_t flag);

typedef uint16_t u16;
typedef uint8_t u8;


extern u32 debug_mode;
extern uint32_t g_fnn;
extern uint32_t g_fnn_not_save;
#if 0
static inline u32 en_irq_save()
{
    //u32 flag = __get_PRIMASK();
    u32 flags;
    asm volatile(
        "   mrs %0, primask @ local_save_flags"
        : "=r" (flags) : : "memory", "cc");
    __enable_irq();
    return flags;
}

static inline u32 dis_irq_save()
{
    //u32 flag = __get_PRIMASK();
    u32 flags;
    asm volatile(
        "   mrs %0, primask @ local_save_flags"
        : "=r" (flags) : : "memory", "cc");
    __disable_irq();
    return flags;
}

static inline void irq_restore(u32 flags)
{
    //__set_PRIMASK(flag);
    asm volatile(
        "   msr primask, %0 @ local_irq_restore"
        :
        : "r" (flags)
        : "memory", "cc");
}
#else
#define en_irq_save(flags) \
{ \
    asm volatile( \
        "   mrs %0, primask @ local_save_flags" \
        : "=r" (flags) : : "memory", "cc"); \
    asm volatile ("cpsie i"); \
}
#define dis_irq_save(flags) \
{ \
    asm volatile( \
        "   mrs %0, primask @ local_save_flags" \
        : "=r" (flags) : : "memory", "cc"); \
    asm volatile ("cpsid i"); \
}
#define irq_restore(flags) \
{ \
    asm volatile( \
        "   msr primask, %0 @ local_irq_restore" \
        : : "r" (flags) : "memory", "cc"); \
}
#endif

#endif
