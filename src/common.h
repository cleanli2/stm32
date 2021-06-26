#ifndef __COMMON_H
#define __COMMON_H
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
void delay_us(u32 nus);
void delay_ms(uint16_t nms);
void led_flash(u32 led_flag, u32 ms_ct);
void soft_reset_system();
void power_off();
void reboot_download();
uint64_t get_system_us();

#define LED_R_C 0x1
#define LED_Y_C 0x2
#define LED_R_1 0x4
#define LED_Y_1 0x8
#define LED_R_0 0
#define LED_Y_0 0
void led_raw_set(u32 led_flag);

typedef uint16_t u16;
typedef uint8_t u8;
#endif
