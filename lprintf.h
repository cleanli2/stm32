#ifndef __LPRINTF_H
#define __LPRINTF_H
#include<stdint.h>

int __io_putchar(int ch);
uint16_t __io_getchar();

void lprintf(const char *fmt, ...);

#endif
