#ifndef _RING_BUF_H
#define _RING_BUF_H

#include "os_task.h"

typedef struct _rb_ctl{
    unsigned int ri;
    unsigned int wi;
    os_task_st * r_notify;
    os_task_st * w_notify;
} rb_ctl;

#define INC_ROUND(a, size) ((a+1)>=size?(a+1-size):(a+1))

#define DECLARE_RB_DATA(TYPE, name, SIZE) \
    TYPE RB_DATA_##name[SIZE]; \
    rb_ctl rb_ctl##name = {0, 0, 0, 0};

#define EXTERN_DECLARE_RB_DATA(TYPE, name, SIZE) \
    extern TYPE RB_DATA_##name[SIZE]; \
    extern rb_ctl rb_ctl##name;

#define RB_DATA_SIZE(TYPE, name) (sizeof(RB_DATA_##name)/sizeof(TYPE))

#define RB_IS_EMPTY(TYPE, name) (rb_ctl##name.ri == rb_ctl##name.wi)
#define RB_IS_FULL(TYPE, name) \
    (rb_ctl##name.ri == INC_ROUND(rb_ctl##name.wi, RB_DATA_SIZE(TYPE, name)))

/*
#define RB_GET(TYPE, name) ({ \
        TYPE* ret; \
        if(rb_ctl##name.ri == rb_ctl##name.wi){ret = NULL;} \
        else{ret = &RB_DATA_##name[rb_ctl##name.ri]; \
        rb_ctl##name.ri = INC_ROUND(rb_ctl##name.ri, RB_DATA_SIZE(TYPE, name))}; \
        ret;})
        */

#define RB_R_GET(TYPE, name) (&RB_DATA_##name[rb_ctl##name.ri])
#define RB_W_GET(TYPE, name) (&RB_DATA_##name[rb_ctl##name.wi])

#define RB_R_SET(TYPE, name) {\
    rb_ctl##name.ri = INC_ROUND(rb_ctl##name.ri, RB_DATA_SIZE(TYPE, name)); \
    wake_up(rb_ctl##name.r_notify)}
#define RB_W_SET(TYPE, name) {\
    rb_ctl##name.wi = INC_ROUND(rb_ctl##name.wi, RB_DATA_SIZE(TYPE, name)); \
    wake_up(rb_ctl##name.w_notify)}

#define RB_OK 0
#define RB_FAIL 0

#define RB_W_GET_wait(TYPE, name) ({ \
        while(RB_IS_FULL(TYPE, name))sleep_wait(rb_ctl##name.r_notify, TASK_STATUS_SLEEPING_WRB); \
        RB_W_GET(TYPE, name);})

#define RB_R_GET_wait(TYPE, name) ({ \
        while(RB_IS_EMPTY(TYPE, name))sleep_wait(rb_ctl##name.w_notify, TASK_STATUS_SLEEPING_RRB); \
        RB_R_GET(TYPE, name);})

#endif
