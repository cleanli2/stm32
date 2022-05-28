#ifndef __OS_TASK_H
#define __OS_TASK_H

#define INTERRUPT_REGS_BAK_NUM 10
#define STACK_SIZE 0x80
#define MAX_OS_TASKS 2

#define OS_FAIL -1
#define OS_OK 0 
typedef struct _os_task_st
{
    struct _os_task_st * next;
    u32 stack_bak[STACK_SIZE];
    u32*stack_p;
    const char* name;
} os_task_st;

extern os_task_st * cur_os_task;
u32*sche_os_task(u32*stack_data);
void os_task_init();
int os_task_add(func_p fc);
#endif
