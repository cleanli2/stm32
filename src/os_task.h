#ifndef __OS_TASK_H
#define __OS_TASK_H

#include "list.h"

#define INTERRUPT_REGS_BAK_NUM 10
#define STACK_SIZE_LOCAL 0x30
#define STACK_SIZE_LARGE 0xA0
#define MAX_OS_TASKS 4
#define MAX_OS_TIMERS 4

#define OS_FAIL -1
#define OS_OK 0 
#define BIT_ACCESS(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define MEM_PRINT_LOCK 0
#define LPRINTF_LOCK 1

#define TASK_STATUS_RUNNING 0
#define TASK_STATUS_SLEEPING 1

#define TIMER_AVALABLE 0

typedef struct _os_task_st
{
    struct list_head list;
    struct _os_task_st * next;
    u32*stack_p;
    const char* name;
    u32 start_run_time_count;
    u32 run_time_counts;
    u32 cpu_accp_perctg;
    u32 task_status;
} os_task_st;

typedef struct _task_timer
{
    os_task_st* task;
    u32 time;
} os_task_timer;

extern os_task_st * cur_os_task;
extern u32 os_is_running;
u32*sche_os_task(u32*stack_data);
void os_task_init();
int os_task_add(func_p fc, u32*, const char*, u32 stack_size);
void spin_lock_init(u32 lockno);
void spin_lock(u32 lockno);
void spin_unlock(u32 lockno);
u32 atomic_inc(u32*);
void os_10ms_delay(u32);
void check_os_timer();
void os_switch_trigger();
uint16_t os_con_recv();

#endif
