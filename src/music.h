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

#define SOUND_BEEP_MODE 1
#define SOUND_DAC_MODE 2
struct music_play_info{
    const signed char*pu;
    uint pu_index;
    uint8 music_status;
    uint divert_index;
    uint restart_index;
};

typedef struct sound_info_t {
    u32 mrv;
    u32 stc;
} sound_info;

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
extern const signed char * music_lst[];

uint8 get_note_index(signed char value);
int get_music_lst_size();
void* get_sound();
void* dac_get_sound();
int put_sound(u32, u32);
int dac_put_sound(uint16_t);
void sound_execute();
void sound_execute_dac();
int sound_pool_full();
int dac_sound_pool_full();
int dac_get_sound_size();
int get_sound_size();

struct wave_header{
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t Format;
    uint32_t SubChunk1ID;
    uint32_t SubChunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    uint32_t SubChunk2ID;
    uint32_t SubChunk2Size;
    uint8_t data;
};
#endif
