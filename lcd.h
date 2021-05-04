#ifndef _LCD_H
#define _LCD_H

extern unsigned char jiong1[];
extern unsigned char lei1[];

int lcd_init(void);
void Display_Chinese(unsigned char y,unsigned char l,unsigned char *p);
#endif
