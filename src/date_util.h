#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <stdint.h>

// Simple date structure
typedef struct {
    uint16_t y;
    uint8_t  m;
    uint8_t  d;
    uint8_t  h;
    uint8_t  min;
    uint8_t  s;
    uint8_t  w;
} date_time;
extern date_time *dt;

// === Date conversion utilities ===

// Convert a calendar date (Y, M, D)
// to the number of days since 0001-01-01 (Gregorian calendar)
uint32_t days_since_ad1(uint32_t y, uint32_t m, uint32_t d);

// Convert a day count since 0001-01-01
// back to the corresponding calendar date
void date_from_days(uint32_t days, uint16_t *y, uint8_t *m, uint8_t *d);

int get_weekday(int y, int m, int d);

void secs2date_197011(uint32_t secs);

uint32_t date2secs_197011();

void RTC_Set();

char*RTC_Get(void);

#endif
