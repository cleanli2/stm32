#include "common.h"
#include "adc.h"

#define ADC_INITED 0x11d0
#ifdef ADC_DEBUG
#define Debug_LOG_ADC(...) lprintf(__VA_ARGS__)
#else
#define Debug_LOG_ADC(...)
#endif
uint32_t v_bat = 0;

static int adc_inited = 0;
static int g_adc_log_en= 0;
void adc_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    lprintf("performing adc init\n");
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure analog inputs */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_ADC_V4_2_PIN | GPIO_ADC_IBAT_PIN;
    GPIO_Init(GPIO_GROUP_V4_2_IBAT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_ADC_VREF_PIN;
    GPIO_Init(GPIO_GROUP_VREF, &GPIO_InitStructure);
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    /* ADC1 regular channels configuration */
    lprintf("config adc1 using VREF\n");
    ADC_RegularChannelConfig(ADC1, ADC_Channel_VREF, 1, ADC_SampleTime_28Cycles5);

    lprintf("enable adc1\n");
    ADC_Cmd(ADC1, ENABLE);
    /* Enable ADC1 reset calibration register */
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    lprintf("wait adc1 calibration\n");
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));
    lprintf("adc1 calibration done\n");
    adc_inited = ADC_INITED;
}

uint32_t get_adc_value(int my_index)
{
    switch(my_index){
        case VREF:
            ADC_RegularChannelConfig(ADC1, ADC_Channel_VREF, 1, ADC_SampleTime_28Cycles5);
            break;
        case V4_2:
            ADC_RegularChannelConfig(ADC1, ADC_Channel_V4_2, 1, ADC_SampleTime_28Cycles5);
            break;
        case VIMEA:
            ADC_RegularChannelConfig(ADC1, ADC_Channel_IBAT, 1, ADC_SampleTime_28Cycles5);
            break;
        default:
            lprintf("not support adc type\n");
            return 0;
    }
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    do
    {
        delay_us(5);
    }while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET);
    return ADC_GetConversionValue(ADC1);
}

void get_myadc_value(uint32_t*v_core_mv_p, uint32_t*v_bat_mv_p, int32_t*i_mA_p)
{
    uint32_t v_currt_mv, v_core_mv, v_bat_mv, raw_v_ref, raw_v_bat, raw_v_currt;
    int32_t i_mA;

    if(g_adc_log_en)Debug_LOG_ADC("start adc1 VREF convertion\n");
    raw_v_ref=get_adc_value(VREF);
    if(g_adc_log_en)Debug_LOG_ADC("vref = %x\n", raw_v_ref);
    v_core_mv = V_REV_mv * 4096 / raw_v_ref;
    if(g_adc_log_en)Debug_LOG_ADC("real vcore = %dmv\n", v_core_mv);

    if(g_adc_log_en)Debug_LOG_ADC("start adc1 V4_2 convertion\n");
    raw_v_bat=get_adc_value(V4_2);
    v_bat_mv = V_REV_mv * raw_v_bat / raw_v_ref;
    v_bat_mv = v_bat_mv * V4_2_RATIO;
    if(g_adc_log_en)Debug_LOG_ADC("real vbat = %dmv\n", v_bat_mv);

    if(g_adc_log_en)Debug_LOG_ADC("start adc1 IBAT convertion\n");
    raw_v_currt=get_adc_value(VIMEA);
    if(g_adc_log_en)Debug_LOG_ADC("vcur = %x\n", raw_v_currt);
    //mA/mv
    v_currt_mv = V_REV_mv * raw_v_currt / raw_v_ref;
    if(g_adc_log_en)Debug_LOG_ADC("real v_currt = %dmv\n", v_currt_mv);
    i_mA = v_currt_mv;
    i_mA = (i_mA-V_REV_mv) * CURRENT_MEASUREMENT_CALIBRATION;
    *v_core_mv_p = v_core_mv;
    *v_bat_mv_p = v_bat_mv;
    *i_mA_p = i_mA;
}

int adc_test(int log_en)
{
    int ret = 0;
    g_adc_log_en = log_en;
#ifdef POWER_MONITOR
    uint32_t v_core, i_currt;
    int32_t i;
    char in_charge;

    if(ADC_INITED!=adc_inited){
        adc_init();
    }
    else{
        if(log_en)Debug_LOG_ADC("adc already inited\n");
    }

    get_myadc_value(&v_core, &v_bat, &i);
    if(i>=0){
        in_charge = '+';
        i_currt = i;
    }
    else{
        in_charge = '-';
        i_currt = -i;
    }
    if(log_en)Debug_LOG_ADC("real I = %dmA\n", i_currt);
    if(log_en)Debug_LOG_ADC("----%dmv %dmv %c%dmA\n", v_core, v_bat, in_charge, i_currt);
#ifdef LARGE_SCREEN
    lcd_lprintf(240, 0, "%dmv %dmv %c%dmA", v_core, v_bat, in_charge, i_currt);
#else
    lcd_lprintf(180, 30, "%dmv %dmv %c%dmA", v_core, v_bat, in_charge, i_currt);
#endif
    if(500000 > (get_system_us()%(1000*1000*200))){//200s
        lprintf_time("v_core %dmv v_bat %dmv in_charge %c I %dmA\n", v_core, v_bat, in_charge, i_currt);
    }
    if(v_bat < BATT_LOW_LIMIT){
        lprintf_time("battery is low\n");
        ret = 1;
    }
#endif
    return ret;
}
