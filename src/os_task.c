#include "common.h"
#include "os_task.h"
#include "task.h"

os_task_st * cur_os_task;

void os_task1(void*);
os_task_st os_tasks[MAX_OS_TASKS];
int tasks_for_use_index = 0;
u32* sche_os_task(u32*stack_data)
{
    if(cur_os_task != cur_os_task->next){
        cur_os_task->stack_p=stack_data;
        cur_os_task = cur_os_task->next;
        stack_data=cur_os_task->stack_p;
    }
    return stack_data;
}

void os_task_init()
{
    cur_os_task = &os_tasks[0];
    cur_os_task->next = cur_os_task;
    cur_os_task->name = "main";
    tasks_for_use_index = 1;
}

#define stbase 0x70
int os_task_add(func_p fc)
{
    if(tasks_for_use_index == MAX_OS_TASKS){
        return OS_FAIL;
    }
    os_task_st* new_tk = &os_tasks[tasks_for_use_index++];
    new_tk->stack_bak[stbase+1] = 0xfffffff9;
    new_tk->stack_bak[stbase+7] = (u32)fc;
    new_tk->stack_bak[stbase+8] = (u32)fc;
    new_tk->stack_bak[stbase+9] = 0x21000000;
    new_tk->next = cur_os_task->next;
    new_tk->stack_p=new_tk->stack_bak+stbase;
    cur_os_task->next = new_tk;
    new_tk->name = "t1";
    return OS_OK;
}

/************lock***********/
static u32 spin_lock_base = 0;
void spin_lock_init(u32 lockno)
{
    BIT_ACCESS(&spin_lock_base,lockno) = 0;
}
void spin_lock(u32 lockno)
{
    while(1 == BIT_ACCESS(&spin_lock_base, lockno));
    BIT_ACCESS(&spin_lock_base, 0)=1;
}
void spin_unlock(u32 lockno)
{
    BIT_ACCESS(&spin_lock_base, lockno) = 0;
}
