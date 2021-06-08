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

void rtc_read(uint8_t*);
void rtc_write(uint8_t*);
char* get_rtc_time();
uint8_t rtc_read_reg(uint8_t addr);
void rtc_write_reg(uint8_t addr, uint8_t data);

#endif
