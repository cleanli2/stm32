#ifndef __COMMON_H
#define __COMMON_H
#include"stm32f10x.h"
#include"version.h"
#include<stdint.h>
#include<string.h>
#include "lprintf.h"
#include "lcd_sueb.h"
#include "cmd.h"
#include "adc.h"
#include "gui.h"
void delay_us(u32 nus);
void delay_ms(uint16_t nms);

typedef uint16_t u16;
typedef uint8_t u8;
#endif
