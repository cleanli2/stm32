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

int os_task_add(func_p fc, u32*stack_base, const char* name)
{
    u32 set_base_offset = STACK_SIZE_LOCAL - INTERRUPT_REGS_BAK_NUM - 1;
    if(tasks_for_use_index == MAX_OS_TASKS){
        lprintf("max os tasks\n");
        return OS_FAIL;
    }
    os_task_st* new_tk = &os_tasks[tasks_for_use_index++];
    memset((void*)stack_base, 0x55, STACK_SIZE_LOCAL*sizeof(u32));
    stack_base[set_base_offset+1] = 0xfffffff9;
    stack_base[set_base_offset+7] = (u32)fc;
    stack_base[set_base_offset+8] = (u32)fc;
    stack_base[set_base_offset+9] = 0x21000000;
    new_tk->next = cur_os_task->next;
    new_tk->stack_p=stack_base+set_base_offset;
    cur_os_task->next = new_tk;
    new_tk->name = name;
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
