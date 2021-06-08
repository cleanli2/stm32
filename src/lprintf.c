#include "lprintf.h"
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "common.h"

char lprintf_buf[256];
char halfbyte2char(char c)
{
        return ((c & 0x0f) < 0x0a)?(0x30 + c):('A' + c - 0x0a);
}

void puthexch(char c)
{
    __io_putchar(halfbyte2char(c>>4));
    __io_putchar(halfbyte2char(c&0xf));
}

void put_hex_uint(uint32_t i)
{
    int c = 8;
    while(c--){
        __io_putchar(halfbyte2char((char)((i&0xf0000000)>>28)));
        i<<=4;
    }
}

int sput_hex_uint8(char*s, uint32_t i)
{
    int c = 2;
    char* p = s;
    while(c--){
        *p++ = (halfbyte2char((char)((i&0xf0)>>4)));
        i<<=4;
    }
    return 2;
}

int sput_hex_uint(char*s, uint32_t i)
{
    int c = 8;
    char* p = s;
    while(c--){
        *p++ = (halfbyte2char((char)((i&0xf0000000)>>28)));
        i<<=4;
    }
    return 8;
}

void puthexchars(char *pt)
{
    while(*pt){
        puthexch(*pt++);
        __io_putchar(' ');
    }
}

void putchars(const char *pt)
{
    while(*pt)
        __io_putchar(*pt++);
}

char * num2str(uint32_t jt, char * s, char n)
{
        char * st, k = 1, j;
        uint32_t tmp;

        st = s;
        if(n > 16 || n < 2){
                *st++ = 0x30;
                *st = 0;
                return s;
        }
        tmp = 1;
        while(jt/tmp >= n){
                k++;
                tmp *= n;
        }

        while(k--){
                j = jt/tmp;
                *st++ = halfbyte2char(j);
                jt -= tmp * j;
                tmp = tmp/n;
        }
        *st = 0;
        return s;
}

void print_uint(uint32_t num)
{
        char nc[11];
        num2str(num, nc, 10);
        putchars(nc);
}

int sprint_uint(char*s, uint32_t num)
{
    char nc[11];
    num2str(num, nc, 10);
    strcpy(s, nc);
    return strlen(nc);
}

void print_hex(uint32_t num)
{
        char nc[9];
        num2str(num, nc, 16);
        putchars(nc);
}

int sprint_hex(char*s, uint32_t num)
{
    char nc[9];
    num2str(num, nc, 16);
    strcpy(s, nc);
    return strlen(nc);
}

void print_binary(uint32_t num)
{
        char nc[33];
        num2str(num, nc, 2);
        putchars(nc);
}

void vslprintf(char*s_buf, const char *fmt, va_list args)
{
    const char *s;
    uint32_t d;
    va_list ap;
    char*sp = s_buf;

    va_copy(ap, args);
    while (*fmt) {
        if (*fmt != '%') {
            *sp++ = *fmt++;
            continue;
        }
        switch (*++fmt) {
	    case '%':
	        *sp++ = (*fmt);
		break;
            case 's':
                s = va_arg(ap, const char *);
                strcpy(sp, s);
                sp += strlen(s);
                break;
            case 'u':
            case 'd':
                d = va_arg(ap, uint32_t);
                sp += sprint_uint(sp, d);
                break;
	    case 'c':
                d = va_arg(ap, uint32_t);
                *sp++ = d&0xff;
                break;
	    case 'x':
                d = va_arg(ap, uint32_t);
                sp += sprint_hex(sp, d);
                break;
	    case 'X':
                d = va_arg(ap, uint32_t);
                sp += sput_hex_uint(sp, d);
                break;
	    case 'b':
                d = va_arg(ap, uint32_t);
                sp += sput_hex_uint8(sp, d);
                break;
            /* Add other specifiers here... */             
            default: 
                *sp++ = (*(fmt-1));
                *sp++ = (*fmt);
                break;
        }
        fmt++;
    }
    *sp = 0;
    va_end(ap);
}

void lprintf(const char *fmt, ...)
{
#if 1
    va_list ap;

    va_start(ap,fmt);
    vslprintf(lprintf_buf,fmt,ap);
    putchars(lprintf_buf);
    va_end(ap);
#else
    putchars(fmt);
#endif
}

void slprintf(char*buf, const char *fmt, ...)
{
    va_list ap;

    va_start(ap,fmt);
    vslprintf(buf,fmt,ap);
    va_end(ap);
}
void lcd_lprintf(uint32_t x, uint32_t y, const char *fmt, ...)
{
    va_list ap;
    uint8_t lpb[32];
    memset(lpb, 0, 32);
    va_start(ap,fmt);
    vslprintf(lpb,fmt,ap);
    va_end(ap);
    Show_Str(x, y,0,0xffff,lpb,24,0);
}
