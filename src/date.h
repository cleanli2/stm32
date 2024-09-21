#ifndef __DATE_H
#define __DATE_H
#include<stdint.h>

#define DAYS_IN_YEAR 365
typedef struct date_info
{
    char* date_str;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t weekday;
} date_info_t;

int is_later_than(date_info_t*dt_new, date_info_t*dt_old);
uint32_t time_diff_minutes(date_info_t* dtp_f, date_info_t * dtp);
uint32_t time_diff_seconds(date_info_t* dtp_f, date_info_t * dtp);
uint32_t time_diff_hours(date_info_t* dtp_f, date_info_t * dtp);
int days_diff(date_info_t* dtp_f, date_info_t * dtp);
void add_time_diff_minutes(date_info_t*dtp, uint32_t tsms);
uint8_t add_with_back_limit(uint8_t * iptp, uint32_t diff, uint8_t limit);
uint8_t sub_with_back_limit(uint8_t * iptp, uint32_t diff, uint8_t limit);
#endif
