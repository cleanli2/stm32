#ifndef _LCD_H
#define _LCD_H

#include"common.h"
extern unsigned char jiong1[];
extern unsigned char lei1[];
extern unsigned char ziku[];

void lcd_init(void);
void Display_Chinese(unsigned char y,unsigned char l,unsigned char *p);
void Display_Chinese2(unsigned char y,unsigned char l,unsigned char *p);
void lcd_write_byte(uint8_t page, uint8_t clm, uint8_t d);
#endif
