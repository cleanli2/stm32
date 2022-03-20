#include "date.h"
#include "type.h"


uint32_t diff_with_inc_step(uint32_t f, uint32_t b, uint32_t inc_step)
{
    return (f>=b)?f-b:f+inc_step-b;
}
uint8_t add_with_back_limit(uint8_t * iptp, uint32_t diff, uint8_t limit)
{
    uint8_t ret = 0;
    //lprintf("&&in %d %d %d\n", *iptp, diff, limit);
    uint32_t t = *iptp + diff;
    while(t>=limit){
        ret++;
        t-=limit;
    }
    *iptp = t;
    //lprintf("&&out %d %d\n", *iptp, ret);
    return ret;
}
/*
uint32_t time_diff_seconds(date_info_t* dtp_f, date_info_t * dtp)
{
    uint32_t ret, h = dtp_f->hour, m  = dtp_f->minute;
    //lprintf("%d %d - %d %d\n", dtp_f->hour, dtp_f->minute,
    //        dtp->hour, dtp->minute);
    ret = diff_with_inc_step(dtp_f->second, dtp->second, 60);
    if(dtp_f->second < dtp->second){
        m = diff_with_inc_step(m, 1, 60);
    }
    ret += diff_with_inc_step(m, dtp->minute, 60)*60;
    if(dtp_f->minute < dtp->minute){
        h = diff_with_inc_step(h, 1, 24);
    }
    ret += diff_with_inc_step(h, dtp->hour, 24)*3600;
    //lprintf("ret %d\n", ret);
    return ret;
}
*/

uint32_t days_passed_in_months[12]={
    0, // Jan
    31, //Feb
    31+28, //Mar
    31+28+31, //Apr
    31+28+31+30, //May
    31+28+31+30+31, //Jun
    31+28+31+30+31+30, //Jul
    31+28+31+30+31+30+31, //Aug
    31+28+31+30+31+30+31+31, //Sep
    31+28+31+30+31+30+31+31+30, //Oct
    31+28+31+30+31+30+31+31+30+31, //Nov
    31+28+31+30+31+30+31+31+30+31+30, //Dec
};
uint32_t days_passed_year(date_info_t *dt)
{
    uint32_t ret;
    ret = dt->day + days_passed_in_months[dt->month-1];
    if((dt->year % 4)==0 && dt->month > 2){
        ret++;
    }
    return ret;
}
int days_diff(date_info_t* dtp_f, date_info_t * dtp)
{
    int ret = 0, dps_f, dps;
    dps_f= days_passed_year(dtp_f);
    dps = days_passed_year(dtp);
    ret = dps_f - dps +
        DAYS_IN_YEAR*(dtp_f->year-dtp->year);
    return ret;
}
uint32_t time_diff_hours(date_info_t* dtp_f, date_info_t * dtp)
{
    uint32_t ret, d_diff;

    d_diff = days_diff(dtp_f, dtp);
    ret = diff_with_inc_step(dtp_f->hour, dtp->hour, 24);
    if(dtp_f->hour < dtp->hour){
        d_diff--;
    }
    ret += d_diff*24;
    //lprintf("ret %d\n", ret);
    return ret;
}

uint32_t time_diff_minutes(date_info_t* dtp_f, date_info_t * dtp)
{
    uint32_t ret, h = dtp_f->hour;
    //lprintf("%d %d - %d %d\n", dtp_f->hour, dtp_f->minute,
    //        dtp->hour, dtp->minute);
    ret = diff_with_inc_step(dtp_f->minute, dtp->minute, 60);
    if(dtp_f->minute < dtp->minute){
        h = diff_with_inc_step(h, 1, 24);
    }
    ret += diff_with_inc_step(h, dtp->hour, 24)*60;
    //lprintf("ret %d\n", ret);
    return ret;
}

void add_time_diff_minutes(date_info_t*dtp, uint32_t tsms)
{
    add_with_back_limit(&dtp->hour, add_with_back_limit(&dtp->minute, tsms, 60), 24);
}

int is_later_than(date_info_t*dt_new, date_info_t*dt_old)
{
#if 0
    if(
            dt_new->year > dt_old->year ||
            (dt_new->year == dt_old->year && dt_new->month > dt_old->month) ||
            (dt_new->year == dt_old->year && dt_new->month == dt_old->month && dt_new->day > dt_old->day) ||
            (dt_new->year == dt_old->year &&
                 dt_new->month == dt_old->month &&
                 dt_new->day == dt_old->day && dt_new->hour > dt_old->hour) ||
            (dt_new->year == dt_old->year &&
                 dt_new->month == dt_old->month &&
                 dt_new->day == dt_old->day &&
                 dt_new->hour == dt_old->hour && dt_new->minute > dt_old->minute) ||
            (dt_new->year == dt_old->year &&
                 dt_new->month == dt_old->month &&
                 dt_new->day == dt_old->day &&
                 dt_new->hour == dt_old->hour &&
                 dt_new->minute == dt_old->minute && dt_new->second == dt_old->second)
      ){
        return 1;
    }
    else
        return 0;
#endif
    if(dt_new->year > dt_old->year){
        return 1;
    }
    else if(dt_new->year < dt_old->year){
        return 0;
    }
    else{//year ==
        if(dt_new->month > dt_old->month){
            return 1;
        }
        else if(dt_new->month< dt_old->month){
            return 0;
        }
        else{//year month==
            if(dt_new->day > dt_old->day){
                return 1;
            }
            else if(dt_new->day < dt_old->day){
                return 0;
            }
            else{//year month day==
                if(dt_new->hour > dt_old->hour ){
                    return 1;
                }
                else if(dt_new->hour < dt_old->hour){
                    return 0;
                }
                else{//year month day hour==
                    if(dt_new->minute > dt_old->minute ){
                        return 1;
                    }
                    else if(dt_new->minute < dt_old->minute){
                        return 0;
                    }
                    else{//year month day hour minute==
                        if(dt_new->second > dt_old->second ){
                            return 1;
                        }
                        else if(dt_new->second < dt_old->second ){
                            return 0;
                        }
                        else{//year month day hour minute second==
                            return 0;
                        }
                    }
                }
            }
        }
    }

}

