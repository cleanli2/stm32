#include "common.h"
#include "os_task.h"
#include "task.h"

char* os_task_status_str[]={
    "taskrunning",
    "sleep_idle",
    "sleep_timer",
    "sleep_lock",
    "sleep_io",
    "sleep_rrb",
    "sleep_wrb",
};

struct list_head priority_tasks_head[TASK_PRIORITIES_NUM];
u32 os_is_running = 0;
os_task_st * cur_os_task;
os_task_st * usart1_wait_task = NULL;
//os_task_st * touch_wait_task = NULL;
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
}

void system_halt()
{
    u32 irqsv;
    dis_irq_save(irqsv);
    while(1);
}

os_task_timer* get_os_timer()
{
    u32 irqsv;
    int index = 0;
    //check if current task already in os timer wait
    for(index = 0; index < MAX_OS_TIMERS; index++){
        if(g_task_timer[index].time != TIMER_AVALABLE &&
                g_task_timer[index].task == cur_os_task){
            lprintf("system:%d ms\n", g_ms_count);
            lprintf("[%d] %d %s\n", index, g_task_timer[index].time,
                    g_task_timer[index].task->name);
            lprintf("Fatal:cur task %s already in wait, status %d\n",
                    cur_os_task->name, cur_os_task->task_status);
            goto error_handle;
        }
    }
    index=0;
    while(1){
        if(g_task_timer[index].time == TIMER_AVALABLE){
            return &g_task_timer[index];
        }
        if(index == MAX_OS_TIMERS){
            lprintf("Fatal:os timer not available\n");
            goto error_handle;
        }
        index++;
    }
error_handle:
    dis_irq_save(irqsv);
    for(index = 0; index < MAX_OS_TIMERS; index++){
        lprintf("[%d]:%d->", index, g_task_timer[index].time);
        if(g_task_timer[index].task){
            lprintf("%s", g_task_timer[index].task->name);
        }
        lprintf("\n");
    }
    system_halt();
    return 0;
}

void os_10ms_delay(u32 timeout)
{
    u32 irqsv;
    os_task_timer* g_tt;
    if(timeout == 0)return;
    dis_irq_save(irqsv);
    g_tt = get_os_timer();
    if(g_tt != 0){
        g_tt->time = g_ms_count + timeout;
        g_tt->task = cur_os_task;
        cur_os_task->task_status = TASK_STATUS_SLEEPING_TIMER;
        irq_restore(irqsv);
        os_switch_trigger();
    }
    else{
        irq_restore(irqsv);
        lprintf("err!Out of os timer\n");
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

    lprintf("name\tcpu\tpri\tstatus\t\tstack\t\tbase\t\tsize\t\ttaskstruct\tdebugdata\n");
    for(int task_pri_index=0;task_pri_index<TASK_PRIORITIES_NUM;task_pri_index++){
        list_for_each(t, &priority_tasks_head[task_pri_index]){
            tmp_task = list_entry(t, os_task_st, list);
            lprintf("%s\t%d%\t%d\t%s\t%X\t%X\t%X\t%X\t%X\n", tmp_task->name,
                    tmp_task->cpu_accp_perctg,
                    task_pri_index,
                    os_task_status_str[tmp_task->task_status],
                    tmp_task->stack_p,
                    tmp_task->stack_base,
                    tmp_task->stack_size, tmp_task, tmp_task->debug_data);
        }
    }
    lprintf("\n");
    lprintf("intrpt\t\tsystk\ttm2\ttm3\tpendsv\turt1\ttkdist\ntime(us)\t");
    for(int i=0;i<NUM_INTRPT;i++){
        lprintf("%d\t", intrpt_time[i]/6);
    }
    lprintf("%d\n", interv_systick/6);
    return;
}
u32* sche_os_task(u32*stack_data)
{
    struct list_head * t;
    os_task_st* t_task;
    u32 task_pri_index = 0;

#if 0
    lprintf_time_buf(1, "oss+%s_%X:%X_%X_%X_%X\n", cur_os_task->name, stack_data,
            stack_data[1],
            stack_data[7],
            stack_data[8],
            stack_data[9]);
#endif
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
#if 0
    lprintf_time_buf(1, "oss-%s_%X:%X_%X_%X_%X\n", cur_os_task->name, stack_data,
            stack_data[1],
            stack_data[7],
            stack_data[8],
            stack_data[9]);
#endif
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
    cur_os_task->wait_next = NULL;
    cur_os_task->name = "main";
    cur_os_task->stack_base=0;
    cur_os_task->stack_size=0;
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
    u32 irqsv;
    lprintf("add task %s\n", name);
    u32 set_base_offset = stack_size - INTERRUPT_REGS_BAK_NUM - 1;
    if(tasks_for_use_index == MAX_OS_TASKS){
        lprintf("max os tasks\n");
        return OS_FAIL;
    }
    os_task_st* new_tk = &os_tasks[tasks_for_use_index++];
    memset((void*)stack_base, 0xAA, stack_size*sizeof(u32));
    memset((void*)stack_base, 0x55, stack_size*sizeof(u32)/2);
    stack_base[set_base_offset+OFFINDEX_LR_CODE] = 0xfffffff9;
    stack_base[set_base_offset+OFFINDEX_LR] = (u32)fc;
    stack_base[set_base_offset+OFFINDEX_PC] = (u32)fc;
    stack_base[set_base_offset+OFFINDEX_XPSR] = 0x21000000;
    new_tk->stack_base=stack_base;
    new_tk->stack_size=stack_size;
    new_tk->stack_p=stack_base+set_base_offset;
    new_tk->name = name;
    new_tk->start_run_time_count = 0;
    new_tk->run_time_counts = 0;
    new_tk->task_status = TASK_STATUS_RUNNING;
    new_tk->task_priority = task_pri;
    new_tk->wait_next = NULL;
    if(task_pri>TASK_PRIORITIES_NUM-1){
        task_pri = TASK_PRIORITIES_NUM-1;
    }

    dis_irq_save(irqsv);
    new_tk->next = cur_os_task->next;
    cur_os_task->next = new_tk;
    list_add_tail(&new_tk->list, &priority_tasks_head[task_pri]);
    irq_restore(irqsv);
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
    (void)lock;
#if 0
    u32 irqsv;
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
            dis_irq_save(irqsv);
            for(i=0;i<OS_LOCK_TASKS_NUM;i++){
                if(NULL==lock->wait_tasks[i]){
                    lock->wait_tasks[i]=cur_os_task;
                    break;
                }
            }
            if(OS_LOCK_TASKS_NUM==i){
                lprintf("os lock tasks full\n");
            }
            cur_os_task->task_status=TASK_STATUS_SLEEPING_WAITLOCK;
            cur_os_task->debug_data=lock->lockno;
            irq_restore(irqsv);
            os_switch_trigger();
        }
    }
#endif
}
void os_unlock(oslock_o* lock)
{
    (void)lock;
#if 0
    u32 irqsv;
    int i;
    BIT_ACCESS(&spin_lock_base, lock->lockno) = 0;
    if(!os_is_running){
        return;
    }
    dis_irq_save(irqsv);
    for(i=0;i<OS_LOCK_TASKS_NUM;i++){
        if(NULL!=lock->wait_tasks[i]){
            lock->wait_tasks[i]->task_status=TASK_STATUS_RUNNING;
            lock->wait_tasks[i]=NULL;
        }
    }
    irq_restore(irqsv);
#endif
}

u32*PendSV_Handler_local(u32*stack_data)
{
    tm_cpt_start();
    //putchars("pendsv\n");
    stack_data=sche_os_task(stack_data);
    intrpt_time[INTPENDSV]=tm_cpt_end();
    return stack_data;
}

u32*USART1_IRQHandler_local(u32*stack_data)
{
    tm_cpt_start();
    //lprintf_time_buf(1, "urt+\n");
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
        if(usart1_wait_task != NULL){
            usart1_wait_task->task_status = TASK_STATUS_RUNNING;
            usart1_wait_task = NULL;
            os_switch_trigger();
        }
    }
    //lprintf_time_buf(1, "urt-\n");
    intrpt_time[INTUART1]=tm_cpt_end();
    return stack_data;
}
uint16_t os_con_recv()
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET){
        usart1_wait_task = cur_os_task;
        cur_os_task->task_status = TASK_STATUS_SLEEPING_WAITIO;
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        os_switch_trigger();
    }
    return 0xff&USART_ReceiveData(USART1);
}
