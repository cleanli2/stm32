#ifndef __LPRINTF_H
#define __LPRINTF_H
#include<stdint.h>

int __io_putchar(int ch);
uint16_t __io_getchar();
int __io_char_received();

extern uint16_t LCD_PRINT_BACK_COLOR;
extern uint16_t LCD_PRINT_FRONT_COLOR;
void lprintf(const char *fmt, ...);
void slprintf(char*buf, const char *fmt, ...);
void lcd_lprintf(uint32_t x, uint32_t y, const char *fmt, ...);

#endif
