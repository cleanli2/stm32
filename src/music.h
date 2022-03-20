#ifndef _MUSIC_H
#define _MUSIC_H

enum MUSIC_STATUS {
    MUSIC_IDLE,
    MUSIC_PLAYING,
    MUSIC_PAUSE,
    MUSIC_END,
};

#define SCORE_END 127
#define HALF_PERIOD 126
#define DOUBLE_PERIOD 125
#define FLAGSTART 124
#define GOSTART 123
#define DIVERT 122
#define HP HALF_PERIOD
#define DP DOUBLE_PERIOD

#define NO_DIVERT 0xffff
#define DEFAULT_MUSIC_NOTE_PERIOD 312
struct music_play_info{
    const signed char*pu;
    uint pu_index;
    uint8 music_status;
    uint divert_index;
    uint restart_index;
};

struct music_note_play_info{
    int8 music_note;
    uint period_ms_ct;
    ulong note_start_timerct;
};

extern struct music_note_play_info music_note_task_play_info;
extern struct music_play_info music_task_play_info;
extern const uint musical_scale_regv[];
extern uint32_t musical_scale_freq_100[];

//music score
extern const signed char fu[];
extern const signed char shaolshi[];
extern const signed char xianglian[];
extern const signed char notice_music[];
extern const signed char warning[];
extern const signed char xiyouji1[];
extern const signed char pwroff_music[];
extern const signed char count_down_music[];
extern const signed char YouJianChuiYan[];

uint8 get_note_index(signed char value);
#endif
