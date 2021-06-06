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

void rtc_test();

#endif
