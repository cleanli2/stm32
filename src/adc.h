#ifndef _ADC_H
#define _ADC_H

enum MY_ADC_TYPE {
    VREF,
    V4_2,
    VIMEA,
    MY_ADC_END,
};

#define V_REV_mv 2500
#define BATT_MAX 4250
#define BATT_LOW_ALERT 3550
#define BATT_LOW_LIMIT 3400
extern uint32_t v_bat;
/*return:
  0:normal
  1:battery voltage is low
  */
int adc_test();
#endif
