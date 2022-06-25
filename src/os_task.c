#include "common.h"
#include "os_task.h"
#include "task.h"

struct list_head priority_tasks_head[TASK_PRIORITIES_NUM];
u32 os_is_running = 0;
os_task_st * cur_os_task;
os_task_st * usart1_wait_task = NULL;
os_task_st * touch_wait_task = NULL;
os_task_timer g_task_timer[MAX_OS_TIMERS];
u32 total_tasks_num = 0;

void os_task1(void*);
os_task_st os_tasks[MAX_OS_TASKS];
int tasks_for_use_index = 0;

void os_switch_trigger()
{
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void check_os_timer()
{
    os_task_timer *g_tt = g_task_timer;
    int i = MAX_OS_TIMERS;
    while(i--){
        if(g_tt->time != TIMER_AVALABLE){
            if(g_tt->time < g_ms_count){
                g_tt->time = TIMER_AVALABLE;
                g_tt->task->task_status = TASK_STATUS_RUNNING;
                os_switch_trigger();
                return;
            }
        }
        g_tt++;
    }
    if(touch_down()){
        if(touch_wait_task != NULL){
            touch_wait_task->task_status = TASK_STATUS_RUNNING;
            touch_wait_task = NULL;
            os_switch_trigger();
        }
    }
}

os_task_timer* get_os_timer()
{
    int index = 0;
    while(1){
        if(g_task_timer[index].time == TIMER_AVALABLE){
            return &g_task_timer[index];
        }
        if(index == MAX_OS_TIMERS){
            lprintf("os timer not available\n");
            return 0;
        }
        index++;
    }
}

void os_10ms_delay(u32 timeout)
{
    os_task_timer* g_tt;
    if(timeout == 0)return;
    g_tt = get_os_timer();
    if(g_tt != 0){
        g_tt->time = g_ms_count + timeout;
        g_tt->task = cur_os_task;
        cur_os_task->task_status = TASK_STATUS_SLEEPING;
        os_switch_trigger();
    }
}

void compute_cpu_occp()
{
    static u32 last_cpu_compute = 0;
    u32 time_long = g_ms_count - last_cpu_compute;
    if(time_long > 1000){
        last_cpu_compute += time_long;
        os_task_st * tmp_task=cur_os_task;
        while(1){
            tmp_task->cpu_accp_perctg =
                (tmp_task->run_time_counts+5) * 100 / time_long;
            tmp_task->run_time_counts = 0;
            if(cur_os_task != tmp_task->next){
                tmp_task = tmp_task->next;
            }
            else{
                break;
            }
        }
    }
}
void showtasks()
{
    os_task_st * tmp_task;
    struct list_head * t;

    lprintf("name\tcpu\tpri\tstatus\n");
    for(int task_pri_index=0;task_pri_index<TASK_PRIORITIES_NUM;task_pri_index++){
        list_for_each(t, &priority_tasks_head[task_pri_index]){
            tmp_task = list_entry(t, os_task_st, list);
            lprintf("%s\t%d%\t%d\t%s\n", tmp_task->name,
                    tmp_task->cpu_accp_perctg,
                    task_pri_index,
                    tmp_task->task_status==TASK_STATUS_RUNNING?"running":"sleep");
        }
    }
    lprintf("\n");
    return;
}
u32* sche_os_task(u32*stack_data)
{
    struct list_head * t;
    os_task_st* t_task;
    u32 task_pri_index = 0;

    if(total_tasks_num > 1){
        while(task_pri_index < TASK_PRIORITIES_NUM){
            list_for_each(t, &priority_tasks_head[task_pri_index]){
                if(&cur_os_task->list == t){
                    continue;
                }
                t_task = list_entry(t, os_task_st, list);
                if(t_task->task_status == TASK_STATUS_RUNNING){
                    break;
                }
            }
            if(&priority_tasks_head[task_pri_index] == t){
                //if higher or same priority with current has no other running task,
                //keep running current task
                if(cur_os_task->task_priority == task_pri_index &&
                        cur_os_task->task_status == TASK_STATUS_RUNNING){
                    t_task = cur_os_task;
                    break;
                }
                else{
                    task_pri_index++;
                }
            }
            else{
                break;
            }
        }
        if(&priority_tasks_head[TASK_PRIORITIES_NUM - 1] == t){
            t_task = cur_os_task;
        }
        cur_os_task->stack_p=stack_data;
        cur_os_task->run_time_counts +=
            g_ms_count - cur_os_task->start_run_time_count;
        cur_os_task = t_task;
        stack_data=cur_os_task->stack_p;
        cur_os_task->start_run_time_count =
            g_ms_count;
        compute_cpu_occp();
    }
    return stack_data;
}

void enable_uart1_int()
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void os_task_init()
{
    cur_os_task = &os_tasks[0];
    cur_os_task->next = cur_os_task;
    cur_os_task->name = "main";
    tasks_for_use_index = 1;
    enable_uart1_int();
    os_is_running = 1;
    for(int i = 0; i < TASK_PRIORITIES_NUM; i++){
        INIT_LIST_HEAD(&priority_tasks_head[i]);
    }
    list_add_tail(&cur_os_task->list, &priority_tasks_head[TASK_PRIORITIES_NUM-1]);
    cur_os_task->task_priority = TASK_PRIORITIES_NUM-1;
    total_tasks_num = 1;
}

int os_task_add(func_p fc, u32*stack_base, const char* name, u32 stack_size, u32 task_pri)
{
    lprintf("add task %s\n", name);
    u32 set_base_offset = stack_size - INTERRUPT_REGS_BAK_NUM - 1;
    if(tasks_for_use_index == MAX_OS_TASKS){
        lprintf("max os tasks\n");
        return OS_FAIL;
    }
    os_task_st* new_tk = &os_tasks[tasks_for_use_index++];
    memset((void*)stack_base, 0xAA, stack_size*sizeof(u32));
    memset((void*)stack_base, 0x55, stack_size*sizeof(u32)/2);
    stack_base[set_base_offset+1] = 0xfffffff9;
    stack_base[set_base_offset+7] = (u32)fc;
    stack_base[set_base_offset+8] = (u32)fc;
    stack_base[set_base_offset+9] = 0x21000000;
    new_tk->next = cur_os_task->next;
    new_tk->stack_p=stack_base+set_base_offset;
    cur_os_task->next = new_tk;
    new_tk->name = name;
    new_tk->start_run_time_count = 0;
    new_tk->run_time_counts = 0;
    new_tk->task_status = TASK_STATUS_RUNNING;
    new_tk->task_priority = task_pri;
    if(task_pri>TASK_PRIORITIES_NUM-1){
        task_pri = TASK_PRIORITIES_NUM-1;
    }
    list_add_tail(&new_tk->list, &priority_tasks_head[task_pri]);
    total_tasks_num++;
    return OS_OK;
}

/************lock***********/
static u32 spin_lock_base = 0;
void spin_lock_init(u32 lockno)
{
    BIT_ACCESS(&spin_lock_base,lockno) = 0;
}
//#define BIT_ACCESS(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
void spin_lock(u32 lockno)
{
    u32 atomic_ret = 1;
    while(atomic_ret){
        if(0 == BIT_ACCESS(&spin_lock_base, lockno)){
            atomic_ret = atomic_inc((u32*)BITBAND(&spin_lock_base, lockno));
        }
    }
}
void spin_unlock(u32 lockno)
{
    BIT_ACCESS(&spin_lock_base, lockno) = 0;
}

void os_lock(oslock_o* lock)
{
    int i;
    u32 atomic_ret = 1;
    if(!os_is_running){
        return spin_lock(lock->lockno);
    }
    while(atomic_ret){
        if(0 == BIT_ACCESS(&spin_lock_base, lock->lockno)){
            atomic_ret = atomic_inc((u32*)BITBAND(&spin_lock_base, lock->lockno));
        }
        else{
            for(i=0;i<OS_LOCK_TASKS_NUM;i++){
                if(NULL==lock->wait_tasks[i]){
                    lock->wait_tasks[i]=cur_os_task;
                    break;
                }
            }
            if(OS_LOCK_TASKS_NUM==i){
                lprintf("os lock tasks full\n");
            }
            cur_os_task->task_status=TASK_STATUS_SLEEPING;
            os_switch_trigger();
        }
    }
}
void os_unlock(oslock_o* lock)
{
    int i;
    BIT_ACCESS(&spin_lock_base, lock->lockno) = 0;
    if(!os_is_running){
        return;
    }
    for(i=0;i<OS_LOCK_TASKS_NUM;i++){
        if(NULL!=lock->wait_tasks[i]){
            lock->wait_tasks[i]->task_status=TASK_STATUS_RUNNING;
            lock->wait_tasks[i]=NULL;
        }
    }
}

u32 sche_time;
u32*PendSV_Handler_local(u32*stack_data)
{
    u32 t = TIM_GetCounter(TIM2);
    //putchars("pendsv\n");
    stack_data=sche_os_task(stack_data);
    sche_time = TIM_GetCounter(TIM2) - t;
    return stack_data;
}

void USART1_IRQHandler()
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
        if(usart1_wait_task != NULL){
            usart1_wait_task->task_status = TASK_STATUS_RUNNING;
            usart1_wait_task = NULL;
            os_switch_trigger();
        }
    }
}
uint16_t os_con_recv()
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET){
        usart1_wait_task = cur_os_task;
        cur_os_task->task_status = TASK_STATUS_SLEEPING;
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        os_switch_trigger();
    }
    return 0xff&USART_ReceiveData(USART1);
}
