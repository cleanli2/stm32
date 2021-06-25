#include <stdio.h>
#include <string.h>
#include "type.h"
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"

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

void task_ui(struct task*vp)
{
    vp;//fix unused variable warning
    //current_ui->ui_process_event(current_ui);
}

void task_key_status(struct task*vp)
{
    vp;//fix unused variable warning
}

void task_lcd_bklight(struct task*vp)
{
    vp;//fix unused variable warning
}

void task_power(struct task*vp)
{
    vp;
}

void task_timer(struct task*vp)
{
    vp;//fix unused variable warning
#if 0
    g_flag_1s = false;
    count_1s = timer_ct/tcops;
    count_10ms = (timer_ct - tcops*count_1s)/COUNT10MS;
    g_flag_10ms = false;
    if(count_10ms != last_count_10ms){
        g_flag_10ms = true;
    }
    if(count_1s != last_count_1s){
        g_flag_1s = true;
        //printf("task timect %u\r\n", cur_task_timeout_ct);
        if(cur_task_timeout_ct > 0 && (current_ui->time_disp_mode & TIME_OUT_EN)){
            if(cur_task_timer_started){
                cur_task_timeout_ct--;
                if(cur_task_timeout_ct == 0){
                    cur_task_event_flag |= 1<<EVENT_UI_TIMEOUT;
                }
            }
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
        }
    }
    last_count_1s = count_1s;
    last_count_10ms = count_10ms;
#endif
}

void task_disp(struct task*vp)
{
    vp;//fix unused variable warning
}
#if 0
void reset_music_note()
{
    music_note_task_play_info.music_note = 0;
    music_note_task_play_info.note_start_timerct = 0;
    music_note_task_play_info.period_ms_ct = 0;
}

bool is_music_idle()
{
    return (music_note_task_play_info.period_ms_ct == 0);
}

bool play_music_note(int8 note, uint period)
{
    if(music_note_task_play_info.period_ms_ct == 0){
        music_note_task_play_info.music_note = note;
        music_note_task_play_info.note_start_timerct = 0;
        music_note_task_play_info.period_ms_ct = period;
        return true;
    }
    return false;
}

void set_music_note_period(uint p)
{
    default_music_note_period = p;
}

bool is_playing_music()
{
    return (music_task_play_info.music_status == MUSIC_PLAYING);
}

void pause_music()
{
    if(music_task_play_info.music_status == MUSIC_PLAYING){
        sound_en(0);
        music_task_play_info.music_status = MUSIC_PAUSE;
    }
}

void continue_music()
{
    if(music_task_play_info.music_status == MUSIC_PAUSE){
        music_task_play_info.music_status = MUSIC_PLAYING;
    }
}

void play_music(__code const signed char* pu, uint note_period)
{
    music_task_play_info.pu = pu;
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
    for(int8 i = 0; i<NUMBER_OF_DELAYED_WORKS; i++){
        if(delayed_works[i].function == NULL){
            delayed_works[i].function = f;
            delayed_works[i].ct_10ms = tct;
            delayed_works[i].para = pa;
            break;
        }
    }
}

#endif

void task_music(struct task*vp)
{
    vp;//fix unused variable warning
#if 0
    int8 music_note;
    uint music_register_value;
    if(music_task_play_info.pu != NULL &&
            music_task_play_info.music_status == MUSIC_PLAYING &&
            is_music_idle()){
        music_flash = !music_flash;
        set_led1(music_flash);
        set_led2(!music_flash);
        //printf("pu_index %u status %x\r\n", music_task_play_info.pu_index, music_task_play_info.music_status);
        music_note = music_task_play_info.pu[music_task_play_info.pu_index++];
        if(music_note==SCORE_END){
            music_task_play_info.music_status = MUSIC_END;
            cur_task_event_flag |= 1<<EVENT_MUSIC_PLAY_END;
            set_led1(false);
            set_led2(false);
            printf("play end\r\n");
            sound_en(0);
            set_music_note_period(DEFAULT_MUSIC_NOTE_PERIOD);//recover default note period
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

    if(music_note_task_play_info.period_ms_ct){
        if(music_note_task_play_info.note_start_timerct == 0){
            music_note_task_play_info.note_start_timerct = timer_ct;
            if(music_note_task_play_info.music_note == 0){
                sound_en(0);
            }
            else{
                music_register_value =
                    musical_scale_regv[get_note_index(music_note_task_play_info.music_note)];
                sound_en(1);
                update_music_note_register(music_register_value);
            }
        }
        else if((timer_ct - music_note_task_play_info.note_start_timerct) >=
                (ulong)music_note_task_play_info.period_ms_ct*COUNT10MS/10){
            music_note_task_play_info.period_ms_ct = 0;
            if(!is_playing_music()){
                sound_en(0);
            }
        }
    }
    else{
        sound_en(0);
    }
#endif
}
void task_misc(struct task*vp)
{
    vp;//fix unused variable warning
#if 0
    if(!stop_feed_wtd){
        feed_watch_dog();
    }
    if(g_flag_10ms){
        for(int8 i = 0; i<NUMBER_OF_DELAYED_WORKS; i++){
            if(delayed_works[i].ct_10ms > 0){
                delayed_works[i].ct_10ms--;
                if(delayed_works[i].ct_10ms==0 && delayed_works[i].function != NULL){
                    delayed_works[i].function(delayed_works[i].para);
                    delayed_works[i].function = NULL;
                }
            }
        }
    }
#endif
}
