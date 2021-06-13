#ifndef _RTC_H
#define _RTC_H

#define SDA_GG GPIOB
#define SCL_GG GPIOB

#define SDA_PIN GPIO_Pin_7
#define SCL_PIN GPIO_Pin_6

#define SDA_HIGH GPIO_WriteBit(SDA_GG, SDA_PIN, 1)
#define SCL_HIGH GPIO_WriteBit(SCL_GG, SCL_PIN, 1)
#define SDA_LOW GPIO_WriteBit(SDA_GG, SDA_PIN, 0)
#define SCL_LOW GPIO_WriteBit(SCL_GG, SCL_PIN, 0)
#define GET_SDA GPIO_ReadInputDataBit(SDA_GG, SDA_PIN)

#define AUTO_TIME_ALERT_INC_MINS (60*6)
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

void rtc_read(uint8_t*);
void rtc_write(uint8_t*);
char* get_rtc_time(date_info_t*);
uint8_t rtc_read_reg(uint8_t addr);
uint8_t rtc_write_reg(uint8_t addr, uint8_t data);
void auto_time_alert_set(uint32_t time_step_minutes);
uint32_t time_diff_minutes(date_info_t* dtp_f, date_info_t * dtp);
uint8_t check_rtc_alert_and_clear();

#endif
