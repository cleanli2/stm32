#ifndef _SOC_RTC_H
#define _SOC_RTC_H
#include"common.h"
typedef struct
{
	u8 hour;
	u8 min;
	u8 sec;
	u16 w_year;
	u8  w_month;
	u8  w_date;
	u8  week;
}date_time;
extern date_time *dt;

extern u8 const mon_table[12];
void Disp_Time(u8 x,u8 y,u8 size);
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);
u8 RTC_Init(void);
u8 Is_Leap_Year(u16 year);
char*RTC_Get(void);
void RTC_Set();
void adjust_second(int sec);
#endif
