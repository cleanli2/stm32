#ifndef _ADC_H
#define _ADC_H

enum MY_ADC_TYPE {
    VREF,
    V5,
    MY_ADC_END,
};

#define V_REV_mv 2500
#define BATT_MAX 4250
#define BATT_LOW_ALERT 3550
#define BATT_LOW_LIMIT 3400
extern uint32_t v_bat;
extern int g_v5_vltg;
/*return:
  0:normal
  1:battery voltage is low
  */
char adc_random();
uint32_t get_adc_value(int my_index);
int get_bat_voltage();
int get_bat_voltage_tlv431();
#endif
