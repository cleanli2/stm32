#ifndef __OS_TASK_H
#define __OS_TASK_H

#define INTERRUPT_REGS_BAK_NUM 10
#define STACK_SIZE_LOCAL 0x30
#define MAX_OS_TASKS 3

#define OS_FAIL -1
#define OS_OK 0 
#define BIT_ACCESS(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define MEM_PRINT_LOCK 0
#define LPRINTF_LOCK 1
typedef struct _os_task_st
{
    struct _os_task_st * next;
    u32*stack_p;
    const char* name;
} os_task_st;

extern os_task_st * cur_os_task;
u32*sche_os_task(u32*stack_data);
void os_task_init();
int os_task_add(func_p fc, u32*, const char*);
void spin_lock_init(u32 lockno);
void spin_lock(u32 lockno);
void spin_unlock(u32 lockno);
u32 atomic_inc(u32*);

#endif
