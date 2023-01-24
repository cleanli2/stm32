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

extern uint32_t logv;

u32 get_con_char_10ms(u32 tout);
void delay_us(u32 nus);
void delay_ms(uint16_t nms);
void led_flash(u32 led_flag, u32 ms_ct);
void soft_reset_system();
void power_off();
void reboot_download();
uint64_t get_system_us();
void beep_by_timer_100(uint32_t hz_100);
char * str_to_hex(char *s, uint32_t * result);
void main_init(void);
void enable_sound(int en);
int get_sound_sta();
int get_sound_mode();
int get_sound_size();
uint add_with_limit(uint a, uint b, uint limit);
uint sub_with_limit(uint a, uint b, uint limit);
void Dac1_Init(void);
void Dac1_Set_Vol(u32 vol);
void Dac1_wave(u32 type, u32 para2);
void Dac1_DeInit(void);
void dac_set_freq(uint32_t freq);

#define LED_R_C 0x1
#define LED_Y_C 0x2
#define LED_R_1 0x4
#define LED_Y_1 0x8
#define LED_R_0 0
#define LED_Y_0 0
#define MEM_SD_SIZE 0x2000
#define usb_writable (0x80000000&logv)
void led_raw_set(u32 led_flag);
void usb_main_init(uint32_t flag);

typedef uint16_t u16;
typedef uint8_t u8;
#endif
