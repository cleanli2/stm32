#ifndef _ADC_H
#define _ADC_H

#define BATT_MAX 4200
#define BATT_LOW_ALERT 3550
#define BATT_LOW_LIMIT 3400
extern uint32_t v_bat;
/*return:
  0:normal
  1:battery voltage is low
  */
int adc_test();
#endif
