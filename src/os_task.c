#include "common.h"
#include "os_task.h"
#include "task.h"

u32 os_is_running = 0;
os_task_st * cur_os_task;
os_task_st * usart1_wait_task = NULL;
os_task_timer g_task_timer[MAX_OS_TIMERS];

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
u32* sche_os_task(u32*stack_data)
{
    if(cur_os_task != cur_os_task->next){
        os_task_st* t = cur_os_task;
        while(1){
            t = t->next;
            if(t->task_status == TASK_STATUS_RUNNING){
                break;
            }
        }
        cur_os_task->stack_p=stack_data;
        cur_os_task->run_time_counts +=
            g_ms_count - cur_os_task->start_run_time_count;
        cur_os_task = t;
        stack_data=cur_os_task->stack_p;
        cur_os_task->start_run_time_count =
            g_ms_count;
    }
    compute_cpu_occp();
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
}

int os_task_add(func_p fc, u32*stack_base, const char* name, u32 stack_size)
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
    cur_os_task->run_time_counts = 0;
    cur_os_task->task_status = TASK_STATUS_RUNNING;
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
        __io_putchar('8');
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
