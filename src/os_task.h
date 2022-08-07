#ifndef __OS_TASK_H
#define __OS_TASK_H

#include "list.h"

#define INTERRUPT_REGS_BAK_NUM 10
#define STACK_SIZE_LOCAL 0x80
#define STACK_SIZE_LARGE 0xA0
#define MAX_OS_TASKS 9
#define MAX_OS_TIMERS 12
#define OS_LOCK_TASKS_NUM 8

#define OS_FAIL -1
#define OS_OK 0 
#define BIT_ACCESS(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define MEM_PRINT_LOCK 0
#define LPRINTF_LOCK 1
#define SPI_BUS_LOCK 2
#define EVT_LOCK_NO 3
#define LOG_LOCK_NO 4
#define LCD_PRINT_LOCK_NO 5
#define DISP_RB_LOCK 6
#define OS_LCD_PRT_BUF_LOCK 7


#define TIMER_AVALABLE 0
#define TASK_PRIORITIES_NUM MAX_OS_TASKS

enum task_status{
    TASK_STATUS_RUNNING,
    TASK_STATUS_SLEEPING_IDLE,
    TASK_STATUS_SLEEPING_TIMER,
    TASK_STATUS_SLEEPING_WAITLOCK,
    TASK_STATUS_SLEEPING_WAITIO,
    TASK_STATUS_SLEEPING_RRB,
    TASK_STATUS_SLEEPING_WRB
};


typedef struct _os_task_st
{
    struct list_head list;
    struct _os_task_st * next;
    u32*stack_p;
    u32* stack_base;
    u32 stack_size;
    const char* name;
    u32 start_run_time_count;
    u32 run_time_counts;
    u32 cpu_accp_perctg;
    u32 task_status;
    u32 task_priority;
} os_task_st;

typedef struct _task_timer
{
    os_task_st* task;
    u32 time;
} os_task_timer;

typedef struct _oslock_o
{
    u32 lockno;
    os_task_st* wait_tasks[OS_LOCK_TASKS_NUM];
} oslock_o;

enum EVT_TYPE {
    EVT_SCRN_TOUCH_PRESSED,
    EVT_SCRN_TOUCH_UP,
    EVT_ONE_SECOND,
    EVT_END
};

typedef struct _evt
{
    u32 type;
    u8 pkg[8];
} evt;

extern os_task_st * cur_os_task;
extern u32 os_is_running;
u32*sche_os_task(u32*stack_data);
void os_task_init();
int os_task_add(func_p fc, u32*, const char*, u32 stack_size, u32 priority);
void showtasks();
void spin_lock_init(u32 lockno);
void spin_lock(u32 lockno);
void spin_unlock(u32 lockno);
u32 atomic_inc(u32*);
void os_10ms_delay(u32);
void check_os_timer();
void os_switch_trigger();
uint16_t os_con_recv();
void os_lock(oslock_o* lock);
void os_unlock(oslock_o* lock);
void system_halt();

void putchars(const char *pt);
#define sleep_wait(task_to_wait, sleep_type) {\
    __disable_irq(); \
    if(task_to_wait!=NULL){putchars("ERROR:sleep wait lost\n");system_halt();} \
    task_to_wait = cur_os_task; \
    cur_os_task->task_status = sleep_type; __enable_irq();\
    os_switch_trigger();}

#define wake_up(task_to_wake) \
    __disable_irq(); \
    if(NULL!=task_to_wake){task_to_wake->task_status = TASK_STATUS_RUNNING; \
    task_to_wake=NULL;} \
    __enable_irq();

#define DECLARE_OS_LOCK(lockname, lockno) \
    oslock_o lockname = {lockno, {0}}

extern os_task_st * touch_wait_task;

#endif
