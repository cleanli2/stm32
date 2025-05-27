#include <stdio.h>
#include <string.h>
#include "type.h"
#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"

#define KEYBUF_SIZE 32
unsigned int keypressed=0;
uint8_t keyvalue[KEYBUF_SIZE]={0};
void keyboard_main();

uint last_count_1s = 0;
uint last_count_10ms = 0;
uint count_1s=0;
uint count_10ms=0;
uint g_flag_1s ;
uint g_flag_10ms ;
bool music_flash = false;
bool cur_task_timer_started = false;
date_info_t g_cur_date = {0};
uint16_t touch_x;
uint16_t touch_y;
uint16_t cached_touch_x = 0;
uint16_t cached_touch_y = 0;
uint16_t draw_x = 0;
uint16_t draw_y = 0;
//uint16_t touch_status = 0;
//uint16_t last_touch_status = 0;
uint no_touch_down_ct = 0;
uint32_t cur_task_event_flag;
uint cur_task_timeout_ct;
uint default_music_note_period = DEFAULT_MUSIC_NOTE_PERIOD;
uint no_key_down_ct_lcd = 0;
uint no_key_down_ct = 0;
uint32_t idle_bl = 0xffffffff;
uint32_t bl = 0xffffffff;
bool save_power_mode = false;
int sound_pool_full();
static u32 sound_time_ct = 0;
struct delay_work_info delayed_works[]={
    {
        NULL,
        0,
        NULL
    },
    {
        NULL,
        0,
        NULL
    }
};
#define NUMBER_OF_DELAYED_WORKS \
    (sizeof(delayed_works)/sizeof(struct delay_work_info))

#if 0
void local_float_sprintf(struct s_lfs_data* lfsd)
{
    if(lfsd->fv > 65535){
        strcpy(lfsd->buf, "oo");
        return;
    }
    uint tmp_int = lfsd->fv;
    sprintf(lfsd->buf, "%u", tmp_int);
    uint8 tmp=strlen(lfsd->buf);
    if(tmp<lfsd->number_int){
        memset(lfsd->buf, '0', lfsd->number_int);
        sprintf(lfsd->buf+lfsd->number_int-tmp, "%u", tmp_int);
    }
    float decimal = lfsd->fv - tmp_int + 1;
    uint8 n_dec=lfsd->number_decimal;
    while(n_dec--)decimal*=10;
    tmp_int=decimal;
    tmp=strlen(lfsd->buf);
    sprintf(lfsd->buf+tmp, "%u", tmp_int);
    lfsd->buf[tmp]='.';
    if(lfsd->follows){
        strcat(lfsd->buf, lfsd->follows);
    }
}

void time_hms(char*buf, uint t)
{
    uint h, m, tm, s;

    h = t / 3600;
    tm = t - h * 3600;
    m = tm / 60;
    s = tm - m * 60;
    sprintf(buf, "%02u:%02u:%02u", h, m, s);
}
#endif

void task_key_status(struct task*vp)
{
    (void)vp;//fix unused variable warning
}

void task_lcd_bklight(struct task*vp)
{
    (void)vp;//fix unused variable warning
    if(idle_bl == 0xffffffff){
        idle_bl = get_env_uint("idle_bl", DEFAULT_IDLE_BL);
        lprintf("get idle_bl=%d\r\n", idle_bl);
    }
    if(bl == 0xffffffff){
        bl = get_env_uint("bl", DEFAULT_BL);
        lprintf("get bl=%d\r\n", bl);
    }
    if(save_power_mode){
        //lprintf("lcdps:%d %d\n", no_key_down_ct_lcd, get_BL_value());
        if(get_BL_value() > idle_bl){
            if(no_key_down_ct_lcd > (LCD_POWER_SAVE_CYCLE/LCD_POWER_SAVE_RATIO)){
                lprintf_time("lcd_=%d\r\n", idle_bl);
                no_key_down_ct_lcd = 0;
                set_BL_value(idle_bl);
            }
        }
        else{
            if(no_key_down_ct_lcd > (LCD_POWER_SAVE_CYCLE)){
                lprintf_time("lcd^=%d\r\n", bl);
                no_key_down_ct_lcd = 0;
                set_BL_value(bl);
            }
        }
    }
}

void delayed_close_led(void*p)
{
    uint led_cmd = (uint)p;
    led_raw_set(led_cmd);
}

void flash_led(uint8 led_cmd, uint mss)
{
    led_raw_set(led_cmd);
    set_delayed_work(mss, delayed_close_led, (void*)(led_cmd&(LED_Y_C|LED_R_C)));
}

void task_power(struct task*vp)
{
    (void)vp;
    if(g_flag_1s){
        if(adc_test(0)){
            cur_task_event_flag |= 1<<EVENT_BATT_LOW;
        }
        flash_led(LED_Y_C|LED_Y_1, 20);
    }
}

void task_timer(struct task*vp)
{
    (void)vp;//fix unused variable warning
    g_flag_1s = false;
    g_flag_10ms = false;
    uint64_t systime = get_system_us();
    count_1s = systime/1000000;
    count_10ms = (systime - 1000000*count_1s)/10000;
    if(count_10ms != last_count_10ms){
        g_flag_10ms = true;
    }
    if(count_1s != last_count_1s){
        do_time_correct();
        char*date = get_rtc_time(&g_cur_date);
        g_flag_1s = true;
        lcd_lprintf(0,0,date);
        //lprintf("task timect %u\r\n", cur_task_timeout_ct);
        if(cur_task_timeout_ct > 0 && (current_ui->time_disp_mode & TIME_OUT_EN)){
            if(cur_task_timer_started){
                cur_task_timeout_ct--;
                if(cur_task_timeout_ct == 0){
                    cur_task_event_flag |= 1<<EVENT_UI_TIMEOUT;
                }
            }
#if 0
            if(current_ui->time_disp_mode & TIME_DISP_EN){
                uint tmp_ct;
                if(current_ui->time_disp_mode & TIME_DISP_LEFT){
                    tmp_ct = cur_task_timeout_ct;
                }
                else{
                    tmp_ct = current_ui->timeout - cur_task_timeout_ct;
                }
                if(current_ui->time_disp_mode & TIME_DISP_SECOND){
                    sprintf(disp_mem+current_ui->time_position_of_dispmem,
                            "%u", tmp_ct);
                }
                else{
                    time_hms(disp_mem+current_ui->time_position_of_dispmem, tmp_ct);
                }
                disp_mem_update = true;
            }
#endif
        }
    }
    last_count_1s = count_1s;
    last_count_10ms = count_10ms;
}

void task_disp(struct task*vp)
{
    (void)vp;//fix unused variable warning
}
void reset_music_note()
{
    music_note_task_play_info.music_note = 0;
    music_note_task_play_info.note_start_timerct = 0;
    music_note_task_play_info.period_ms_ct = 0;
}

bool is_music_idle()
{
    return !sound_pool_full();
}

bool play_music_note(int8 note, uint period)
{
    /*
    if(music_note_task_play_info.period_ms_ct == 0){
        music_note_task_play_info.music_note = note;
        music_note_task_play_info.note_start_timerct = 0;
        music_note_task_play_info.period_ms_ct = period;
        return true;
    }
    return false;
    */
    uint music_register_value;
    if(note == 0){
        put_sound(0, period/10);
    }
    else{
        music_register_value =
            musical_scale_freq_100[get_note_index(note)];
        put_sound(music_register_value, period/10);
    }
    return true;
}

void set_music_note_period(uint p)
{
    default_music_note_period = p;
}

bool is_playing_music()
{
    //return (music_task_play_info.music_status == MUSIC_PLAYING);
    return sound_time_ct > 0;
}

void pause_music()
{
    if(music_task_play_info.music_status == MUSIC_PLAYING){
        //beep_by_timer_100(0);
        music_task_play_info.music_status = MUSIC_PAUSE;
    }
}

void continue_music()
{
    if(music_task_play_info.music_status == MUSIC_PAUSE){
        music_task_play_info.music_status = MUSIC_PLAYING;
    }
}

void play_music( const signed char* pu, uint note_period)
{
    if(music_task_play_info.pu == pu){
        if(music_task_play_info.music_status == MUSIC_PLAYING){
            music_task_play_info.music_status = MUSIC_PAUSE;
            //beep_by_timer_100(0);
        }
        if(music_task_play_info.music_status == MUSIC_PAUSE){
            music_task_play_info.music_status = MUSIC_PLAYING;
        }
        return;
    }
    music_task_play_info.pu = (const signed char*)pu;
    music_task_play_info.pu_index = 0;
    music_task_play_info.music_status = MUSIC_PLAYING;
    music_task_play_info.divert_index = NO_DIVERT;
    music_task_play_info.restart_index = 0;
    if(note_period == 0){
        note_period = DEFAULT_MUSIC_NOTE_PERIOD;
    }
    set_music_note_period(note_period);
}

void set_delayed_work(uint tct, func_p f, void*pa)
{
    for(uint i = 0; i<NUMBER_OF_DELAYED_WORKS; i++){
        if(delayed_works[i].function == NULL){
            delayed_works[i].function = f;
            delayed_works[i].ct_10ms = tct;
            delayed_works[i].para = pa;
            break;
        }
    }
}


void task_music(struct task*vp)
{
    (void)vp;//fix unused variable warning
    int8 music_note;
    //uint music_register_value;
    //uint32_t timer_ct;
    if(music_task_play_info.pu != NULL &&
            music_task_play_info.music_status == MUSIC_PLAYING &&
            is_music_idle()){
        music_flash = !music_flash;
        //set_led1(music_flash);
        //set_led2(!music_flash);
        //lprintf("pu_index %u status %x\r\n", music_task_play_info.pu_index, music_task_play_info.music_status);
        music_note = music_task_play_info.pu[music_task_play_info.pu_index++];
        if(music_note==SCORE_END){
            if(is_playing_music()){
                music_task_play_info.pu_index--;
                return;
            }
            music_task_play_info.music_status = MUSIC_END;
            cur_task_event_flag |= 1<<EVENT_MUSIC_PLAY_END;
            //set_led1(false);
            //set_led2(false);
            lprintf_time("play end\n");
            //beep_by_timer_100(0);
            set_music_note_period(DEFAULT_MUSIC_NOTE_PERIOD);//recover default note period
            music_task_play_info.pu = NULL;
        }
        else if(music_note==HALF_PERIOD){
            default_music_note_period /= 2;
        }
        else if(music_note==DOUBLE_PERIOD){
            default_music_note_period *= 2;
        }
        else if(music_note==DIVERT){
            if(music_task_play_info.divert_index != NO_DIVERT){
                music_task_play_info.pu_index = music_task_play_info.divert_index;
                music_task_play_info.divert_index = NO_DIVERT;
            }
        }
        else if(music_note==GOSTART){
                music_task_play_info.divert_index = music_task_play_info.pu_index;
                music_task_play_info.pu_index = music_task_play_info.restart_index;
        }
        else if(music_note==FLAGSTART){
            music_task_play_info.restart_index = music_task_play_info.pu_index;
        }
        else{
            play_music_note(music_note, default_music_note_period);
        }

    }

    /*
    if(music_note_task_play_info.period_ms_ct){
        timer_ct = get_system_us()/1000;//ms
        if(music_note_task_play_info.note_start_timerct == 0){
            music_note_task_play_info.note_start_timerct = timer_ct;
            if(music_note_task_play_info.music_note == 0){
                beep_by_timer_100(0);
            }
            else{
                music_register_value =
                    musical_scale_freq_100[get_note_index(music_note_task_play_info.music_note)];
                beep_by_timer_100(music_register_value);
            }
        }
        else if((timer_ct - music_note_task_play_info.note_start_timerct) >=
                (ulong)music_note_task_play_info.period_ms_ct){
            music_note_task_play_info.period_ms_ct = 0;
            if(!is_playing_music()){
                beep_by_timer_100(0);
            }
        }
    }
    else{
        beep_by_timer_100(0);
    }
    */
}

#define SOUND_POOL_SIZE 4
static sound_info si_pool[SOUND_POOL_SIZE];
static u32 si_wi = 0;
static u32 si_ri = 0;

/*
u32 filled_sound_size()
{
    u32 ret;
    if(si_ri<=si_wi){
        ret = si_wi - si_ri;
    }
    else{
        ret = SOUND_POOL_SIZE - si_ri + si_wi;
    }
    return ret;
}
*/

int get_sound_size()
{
    return sub_with_limit(si_wi, si_ri, SOUND_POOL_SIZE);
}

void* get_sound()
{
    void*ret;
    if(si_wi == si_ri){
        return NULL;
    }
    ret = &si_pool[si_ri];
    si_ri = add_with_limit(si_ri, 1, SOUND_POOL_SIZE);
    return ret;
}

int sound_pool_full()
{
    return add_with_limit(si_wi, 1, SOUND_POOL_SIZE) == si_ri;
}

int put_sound(u32 mrv, u32 stc)
{
    si_pool[si_wi].mrv = mrv;
    si_pool[si_wi].stc = stc;
    si_wi = add_with_limit(si_wi, 1, SOUND_POOL_SIZE);
    return 0;
}

void sound_execute()
{
    sound_info* si;
    if(0 != sound_time_ct){
        sound_time_ct--;
        return;
    }
    si =(sound_info*) get_sound();
    if(NULL == si){
        beep_by_timer_100(0);
        return;
    }
    beep_by_timer_100(si->mrv);
    sound_time_ct = si->stc;
}

#ifdef DAC_SUPPORT
#define DAC_POOL_SIZE (512*2+256)
static uint16_t dac_data_pool[DAC_POOL_SIZE];
static u32 dac_si_wi = 0;
static u32 dac_si_ri = 0;
int dac_get_sound_size()
{
    return sub_with_limit(dac_si_wi, dac_si_ri, DAC_POOL_SIZE);
}

void* dac_get_sound()
{
    void*ret;
    if(dac_si_wi == dac_si_ri){
        return NULL;
    }
    ret = &dac_data_pool[dac_si_ri];
    dac_si_ri = add_with_limit(dac_si_ri, 1, DAC_POOL_SIZE);
    return ret;
}

int dac_put_sound(u16 data)
{
    dac_data_pool[dac_si_wi] = data;
    dac_si_wi = add_with_limit(dac_si_wi, 1, DAC_POOL_SIZE);
    return 0;
}

int dac_sound_pool_full()
{
    return add_with_limit(dac_si_wi, 1, DAC_POOL_SIZE) == dac_si_ri;
}

void sound_execute_dac()
{
    uint16_t* si;
    si = (uint16_t*)dac_get_sound();
    if(NULL == si){
        return;
    }
    Dac1_Set_Vol(*si);
}
#endif

void enable_power_save(bool en)
{
    save_power_mode = en;
    if(idle_bl == 0xffffffff){
        idle_bl = get_env_uint("idle_bl", DEFAULT_IDLE_BL);
        lprintf("get idle_bl=%d\r\n", idle_bl);
    }
    if(bl == 0xffffffff){
        bl = get_env_uint("bl", DEFAULT_BL);
        lprintf("get bl=%d\r\n", bl);
    }
    if(en){
        set_BL_value(idle_bl);
    }
    else{
        set_BL_value(bl);
    }
}

void task_misc(struct task*vp)
{
    (void)vp;//fix unused variable warning
    if(con_is_recved() && (con_recv() == 'c') &&
            'm' == get_con_char_10ms(50) &&
            'd' == get_con_char_10ms(50)){
        LCD_Clear(BLACK);	//fill all screen with some color
        lcd_lprintf(0,0,"CMD mode");
        run_cmd_interface();
        ui_start();
    }
#if 0
    if(!stop_feed_wtd){
        feed_watch_dog();
    }
#endif
    if(g_flag_10ms){
        for(uint8 i = 0; i<NUMBER_OF_DELAYED_WORKS; i++){
            if(delayed_works[i].ct_10ms > 0){
                delayed_works[i].ct_10ms--;
                if(delayed_works[i].ct_10ms==0 && delayed_works[i].function != NULL){
                    delayed_works[i].function(delayed_works[i].para);
                    delayed_works[i].function = NULL;
                }
            }
        }
    }
    //getkey
    keyboard_main();
}

//keyboard of pcf
uint8_t gs_eg_data[2]={0xff, 0xff};
void pcf8574t_init()
{
    gs_eg_data[0] = 0xff;
    gs_eg_data[1] = 0x0f;
    pcf8574t_writeData(0, gs_eg_data[0]);
    pcf8574t_writeData(1, gs_eg_data[1]);
}
void  pcf8574t_set(int bit, int v)
{
    int index=bit/8;
    if(index==1)bit-=8;
    if(v){//set 1
        gs_eg_data[index]|=1<<bit;
    }
    else{
        gs_eg_data[index]&=~(1<<bit);
    }
    pcf8574t_writeData(index, gs_eg_data[index]);
}
void  pcf8574t_revert(int bit)
{
    int v;
    int index=bit/8;
    if(index==1)bit-=8;
    v=!(gs_eg_data[index]&(1<<bit));
    if(v){//set 1
        gs_eg_data[index]|=1<<bit;
    }
    else{
        gs_eg_data[index]&=~(1<<bit);
    }
    pcf8574t_writeData(index, gs_eg_data[index]);
}
int pcf8574t_get(int bit)
{
    u8 tv=1;
    int index=bit/8;
    if(index==1)bit-=8;
    pcf8574t_readData1(index, &tv);
    //lprintf("read i=%d = %x\r\n", index, tv);
    tv>>=bit;
    return tv&0x1;
}
uint8_t g_key_p=0;
void keyboard_main()
{
    int i, j;
    u8 tv=1;
    for(i=0;i<5;i++){
        pcf8574t_set(i, 0);
        for(j=8;j<12;j++){
            if(!pcf8574t_get(j)){
                //lprintf("key down %d\r\n", i*4+j-8);
                keyvalue[g_key_p++]=i*4+j-8;
                if(g_key_p>=KEYBUF_SIZE){
                    g_key_p=0;
                }
                return;
            }
        }
        pcf8574t_set(i, 1);
    }
    keyvalue[g_key_p++]=0xff;
    if(g_key_p>=KEYBUF_SIZE){
        g_key_p=0;
    }
}
int get_keypressed()
{
    static int lastkeyvalue=0xff;
    for(int i=0;i<KEYBUF_SIZE-1;i++){
        if(keyvalue[i]!=keyvalue[i+1]){
            lastkeyvalue=0xff;
            return 0xff;
        }
    }
    if(lastkeyvalue!=keyvalue[0]){
        lastkeyvalue=keyvalue[0];
        return keyvalue[0];
    }
    else{
        return 0xff;
    }
}
