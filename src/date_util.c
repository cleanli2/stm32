#include "sys.h"
#include "date_util.h"
#include "common.h"

char datestr[20]={0};

#define JDN_111 1721426u
#define DAY_SECS (24*60*60)

//days_since_ad1(1970,1,1)
#define DAYS_197011 719162u

date_time g_dt={2025,10,17,5,0,0,5};
date_time*dt=&g_dt;

// === Convert date → day count ===
uint32_t days_since_ad1(uint32_t y, uint32_t m, uint32_t d)
{
    uint32_t a = (14 - m) / 12;
    y = y + 4800 - a;
    m = m + 12 * a - 3;

    // Compute Julian Day Number (JDN)
    uint32_t jdn = d + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;

    // Convert JDN to days since 0001-01-01 (JDN of 0001-01-01 = 1721426)
    return jdn - JDN_111;
}

// === Convert day count → date ===
void date_from_days(uint32_t days, uint16_t*y, uint8_t*m, uint8_t*d)
{
    // Convert back to Julian Day Number
    uint32_t j = days + JDN_111;

    // Fliegel–Van Flandern algorithm (integer arithmetic)
    uint32_t f = j + 1401 + (((4 * j + 274277) / 146097) * 3) / 4 - 38;
    uint32_t e = 4 * f + 3;
    uint32_t g = (e % 1461) / 4;
    uint32_t h = 5 * g + 2;

    *d = (uint8_t)((h % 153) / 5 + 1);
    *m = (uint8_t)(((h / 153 + 2) % 12) + 1);
    *y = (uint16_t)(e / 1461 - 4716 + (12 + 2 - *m) / 12);
}

int get_weekday(int y, int m, int d)
{
    int h=(days_since_ad1(y, m, d)+1)%7;
    return h;
}

void secs2date_197011(uint32_t secs)
{
    uint32_t days=secs/DAY_SECS+DAYS_197011;
    date_from_days(days, &dt->y, &dt->m, &dt->d);
    secs = secs%DAY_SECS;
	dt->h=secs/3600;
	dt->min=(secs%3600)/60;
	dt->s=(secs%3600)%60;
	dt->w=get_weekday(dt->y, dt->m, dt->d);
}
uint32_t date2secs_197011()
{
    prt_dec(dt->y);
    prt_dec(dt->m);
    prt_dec(dt->d);
    uint32_t days=days_since_ad1(dt->y, dt->m, dt->d)-DAYS_197011;
    uint32_t secs=days*DAY_SECS+dt->s+dt->h*3600+dt->min*60;
    return secs;
}

void RTC_Set()
{
	u32 seccount=date2secs_197011();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	RTC_SetCounter(seccount);

	RTC_WaitForLastTask();
}

char*RTC_Get(void)
{
	u32 timecount=0; 
    timecount=RTC_GetCounter();	 
    secs2date_197011(timecount);
    slprintf(datestr, "%d.%d.%d-%d:%d:%d\n", dt->y, dt->m, dt->d,
            dt->h, dt->min, dt->s);
    return datestr;
}
