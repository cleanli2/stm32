#include "stm32f10x.h"
#include "board.h"
#include "common.h"
#include "cmd.h"
#include "lprintf.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sd/stm32_eval_spi_sd.h"
#include "os_task.h"
#include "ring_buf.h"
#include "display.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */
extern unsigned long debug_enable;

u32 intrpt_time[NUM_INTRPT]={0};
u32 debug_mode = 0;
DECLARE_OS_LOCK(oslk_evt, EVT_LOCK_NO);
DECLARE_RB_DATA(evt, rb_evt, 3)
#define COUNTS_PER_US 6

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 last_systick;
u32 interv_systick;
static int sound_enable=1;
static uint32_t g_10ms_count = 0;
uint32_t g_ms_count = 0;
void os_task1(void*);
void os_task2(void*);
void os_task3(void*);
void compute_cpu_occp();
void os_ui(void*p)
{
    (void)p;
    struct point* ppt;
    struct point last_pt = {0xffff, 0xffff};;
    evt *dtw;

    ui_start();
    while(1){
        g_flag_1s = false;
        dtw=RB_R_GET_wait(evt, rb_evt);
        switch(dtw->type){
            case EVT_SCRN_TOUCH_UP:
                ppt = (struct point*)dtw->pkg;
                if(last_pt.px != 0xffff){
                    Proxy_TP_Draw_Big_Point(last_pt.px, last_pt.py, WHITE);
                }
                Proxy_TP_Draw_Big_Point(ppt->px, ppt->py, BLACK);
                last_pt = *ppt;
                cur_task_event_flag |= 1<<EVENT_TOUCH_UP;
                cached_touch_x = ppt->px;
                cached_touch_y = ppt->py;
                break;
            case EVT_ONE_SECOND:
                g_flag_1s = true;
                break;
            default:
                lprintf("unknow evt type\n");
        };
        RB_R_SET(evt, rb_evt);
        task_ui(NULL);
    }
}
struct emulate_touch g_fake_touch = {0};
struct emulate_touch *gftp=&g_fake_touch;
void os_touch(void*p)
{
    struct point pt;
    struct point pt_cache;
    int touch_pressed = 0;
    (void)p;
    while(1){
        if(gftp->n_pt && gftp->pts){
            u32 test_time;
            gftp->cur_interval++;
            if(gftp->cur_interval >= gftp->interval){
                gftp->cur_interval = 0;
                if(gftp->cur_n_pt >= gftp->n_pt){
                    gftp->cur_n_pt = 0;
                }
                pt_cache = gftp->pts[gftp->cur_n_pt++];
                touch_pressed = 1;
                test_time = g_ms_count-gftp->start;
                if(gftp->last*60*1000u < test_time){//test done
                    gftp->pts =NULL;
                    lprintf("##########stress test end %s\n", get_rtc_time(0));
                    lprintf("##########stress test time long %d mins %d s %d ms\n",
                            test_time/1000/60,
                            (test_time/1000)%60, test_time%1000);
                }
            }
        }
        if(touch_down()){
            touch_pressed = 1;
            if(get_TP_point(&pt.px, &pt.py)){
                //lprintf("touch: %d %d\n", pt.px, pt.py);
                pt_cache = pt;
            }
        }
        else{
            if(touch_pressed == 1){
                os_lock(&oslk_evt);
                evt *dtw=RB_W_GET_wait(evt, rb_evt);
                //do work
                dtw->type = EVT_SCRN_TOUCH_UP;
                memcpy(dtw->pkg, &pt_cache, sizeof(struct point));
                RB_W_SET(evt, rb_evt);
                os_unlock(&oslk_evt);
            }
            touch_pressed = 0;
        }
        os_10ms_delay(20);
    }
}
static inline u32 get_sp()
{
    register u32 __reg_sp __asm("sp");
    return (__reg_sp);
}

int get_sound_sta()
{
    return sound_enable;
}
void enable_sound(int en)
{
    sound_enable = en;
}
void timer_init(uint16_t arr, uint16_t psr)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    lprintf("arr %x psr %x\n", arr, psr);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_DeInit(TIM2);
    TIM_InternalClockConfig(TIM2);
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psr;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

#if 0
    /* Output Compare Toggle Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = arr/2;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

    /* Output Compare Toggle Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = arr/4;

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

#endif

    TIM_ARRPreloadConfig(TIM2, DISABLE);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM2, ENABLE);
}

extern os_task_timer *g_tt;
u32*SysTick_Handler_local(u32*stack_data)
{
    (void)stack_data;
    tm_cpt_start();
    u32 t = TIM_GetCounter(TIM2);
    interv_systick = (t>last_systick)?t-last_systick:t+TIM2_RELOAD-last_systick;
    last_systick = t;
#if 0
    lprintf_time_buf(1, "stk+%s_%X:%X_%X_%X_%X\n", cur_os_task->name, stack_data,
            stack_data[1],
            stack_data[7],
            stack_data[8],
            stack_data[9]);
#endif
    g_ms_count++;
    check_os_timer();
#if 0
    lprintf_time_buf(1, "stk-%s_%X:%X_%X_%X_%X\n", cur_os_task->name, stack_data,
            stack_data[1],
            stack_data[7],
            stack_data[8],
            stack_data[9]);
#endif
    intrpt_time[INTSYSTICK]=tm_cpt_end();
    return stack_data;
}

void systick_init()
{
    lprintf("SystemCoreClock=%d\n", SystemCoreClock);

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_Config(72000-1);
    NVIC_SetPriority (PendSV_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    NVIC_SetPriorityGrouping(0x7);//no interrupt preempt
}

uint64_t get_system_us()
{
    uint64_t system_us_count;

    system_us_count = (uint64_t)g_ms_count * 1000 + (72000-1-SysTick->VAL)/72;
    return system_us_count;
}

u32*TIM2_IRQHandler_local(u32*stack_data)
{
    (void)stack_data;
    tm_cpt_start();
#if 0
    lprintf_time_buf(1, "tm2+%s_%X:%X_%X_%X_%X\n", cur_os_task->name, stack_data,
            stack_data[1],
            stack_data[7],
            stack_data[8],
            stack_data[9]);
#endif
	//if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    g_10ms_count++;
    sound_execute();
    //*(u32*)0xe000ed04=0x10000000;
    os_switch_trigger();
#if 0
    lprintf_time_buf(1, "tm2-%s_%X:%X_%X_%X_%X\n", cur_os_task->name, stack_data,
            stack_data[1],
            stack_data[7],
            stack_data[8],
            stack_data[9]);
#endif
    intrpt_time[INTTIM2]=tm_cpt_end();
    return stack_data;
}

/*low 4 bit: Pin14Value | Pin13Value | ToCtlPin14 | ToCtlPin13*/
void led_raw_set(u32 led_flag)
{
    if(led_flag & 0x2){
        if(led_flag & 0x8){
            GPIO_ResetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);
        }
        else{
            GPIO_SetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);
        }
    }
    if(led_flag & 0x1){
        if(led_flag & 0x4){
            GPIO_ResetBits(LED0_GPIO_GROUP,LED0_GPIO_PIN);
        }
        else{
            GPIO_SetBits(LED0_GPIO_GROUP,LED0_GPIO_PIN);
        }
    }
}

void led_flash(u32 led_flag, u32 ms_ct)
{
    led_raw_set(led_flag|0xc);
    delay_ms(ms_ct);
    led_raw_set(led_flag|0x0);
    delay_ms(ms_ct);
}

void delay_us(u32 nus)
{
    u32 ts;
    ts = TIM_GetCounter(TIM2);
    while(sub_with_limit(TIM_GetCounter(TIM2), ts, TIM2_RELOAD) < (nus*COUNTS_PER_US));
}

void delay_ms(u16 nms)
{
    while(nms--){
        delay_us(1000);
    }
}

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  *     @arg COM2  
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void STM_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);


  /* Enable UART clock */
  if (COM == COM1)
  {
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
  }
  else
  {
    /* Enable the USART2 Pins Software Remapping */
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}

int putchar_raw(int ch)
{
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(BOARD_COM1, USART_FLAG_TC) == RESET);
  USART_SendData(BOARD_COM1, (uint8_t) ch);
  return ch;
}

int __io_putchar(int ch)
{
    putchar_raw(ch);
    if(ch=='\n')putchar_raw('\r');
    return ch;
}

uint16_t __io_getchar()
{
  /* Check the parameters */
  while (USART_GetFlagStatus(BOARD_COM1, USART_FLAG_RXNE) == RESET);
  return 0xff&USART_ReceiveData(BOARD_COM1);
}

int __io_char_received()
{
  return (USART_GetFlagStatus(BOARD_COM1, USART_FLAG_RXNE) == SET);
}

static uint32_t beep_st = 0;
void beep_by_timer_100(uint32_t hz_100)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //RCC_APB2PeriphClockCmd(BEEP_GPIO_PERIPH, ENABLE);
    //GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if(beep_st == hz_100){
        return;
    }

    if(hz_100>0){
        /*
        GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN; //TIM_CH1
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
        GPIO_ResetBits(BEEP_GPIO_GROUP,BEEP_GPIO_PIN);
        */

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        TIM_DeInit(TIM3);
        TIM_InternalClockConfig(TIM3);
        /* Time base configuration */
        TIM_TimeBaseStructure.TIM_Period = 4000000/hz_100/2;
        TIM_TimeBaseStructure.TIM_Prescaler = 1800;
        TIM_TimeBaseStructure.TIM_ClockDivision = 0;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

        TIM_ARRPreloadConfig(TIM3, DISABLE);

        TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        /* TIM enable counter */
        TIM_Cmd(TIM3, ENABLE);
        beep_st = hz_100;
    }
    else{
        if(beep_st == 0)return;
        beep_st = 0;
        TIM_Cmd(TIM3, DISABLE);
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_InitStructure);
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
        TIM_DeInit(TIM3);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
        /*
        GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
        GPIO_SetBits(BEEP_GPIO_GROUP,BEEP_GPIO_PIN);
        */
    }
}

void beep_by_timer(uint32_t hz)
{
    beep_by_timer_100(hz*100);
}

u32*TIM3_IRQHandler_local(u32*stack_data)
{
    static int tog=0;
    tm_cpt_start();
#if 0
    lprintf_time_buf(1, "tm3-\n");
#endif
    //if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if(sound_enable){
        if(tog++&0x1){
            GPIO_SetBits(BEEP_GPIO_GROUP,BEEP_GPIO_PIN);
        }
        else{
            GPIO_ResetBits(BEEP_GPIO_GROUP,BEEP_GPIO_PIN);
        }
    }
#if 0
    lprintf_time_buf(1, "tm3-\n");
#endif
    intrpt_time[INTTIM3]=tm_cpt_end();
    return stack_data;
}

void beep(uint32_t hz, uint32_t t_ms)
{
    uint32_t pd, ct;
    if(hz > 1000000)return;
    if(!sound_enable)return;

    pd = 1000000/hz/2;
    ct = hz*t_ms/1000;
    RCC_APB2PeriphClockCmd(BEEP_GPIO_PERIPH, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
    GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
    while(ct--){
	    GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
	    delay_us(pd);
	    GPIO_SetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
	    delay_us(pd);
    }
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
}

void power_off()
{
    Show_Str(20, 630,RED,0xffff,"Power off in 3 seconds",24,0,1);
    lprintf_time("power off in 3 secs\n");
    foce_save_log_func();
    beep(600, 100);
#ifdef RTC_8563
    check_rtc_alert_and_clear();
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, -1, -1);
    auto_time_correct();
#endif
#ifndef ALIENTEK_MINI
    lprintf_time("gpio setb\n");
    foce_save_log_func();
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
    lprintf_time("gpio setb done\n");
#endif
    //power off pin set high to power down
    RCC_APB2PeriphClockCmd(POWEROFF_GPIO_PERIPH, ENABLE);

    GPIO_InitStructure.GPIO_Pin = POWEROFF_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(POWEROFF_GPIO_GROUP, &GPIO_InitStructure);
    GPIO_SetBits(POWEROFF_GPIO_GROUP, POWEROFF_GPIO_PIN);
    //

    delay_ms(200);
    lprintf_time("power off done\n");
    while(1);
}
typedef struct timer_struct
{
    date_info_t start;
    uint32_t seconds_len;
    uint32_t repeat:15;
    uint32_t to_repeat:15;
    uint8_t running:1;
    uint8_t timeout_poff:1;
} timer_struct_t;
timer_struct_t g_timer = {0};

typedef struct progress_indicator
{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint16_t f_color;
    uint16_t b_color;
} progress_indicator_t;
progress_indicator_t g_timer_repeat_pi={
    120,
    50,
    260,
    20,
    YELLOW,
    BLACK
};
progress_indicator_t g_timer_pi={
    120,
    10,
    260,
    20,
    YELLOW,
    BLACK
};
void clear_progress_indicator(progress_indicator_t*pip)
{
    lcd_clr_window(WHITE, pip->x, pip->y, pip->x+pip->w, pip->y+pip->h);
}

void w10ms_delay(u32 ct)
{
    u32 x;
    x = g_10ms_count + ct;
    while(x > g_10ms_count)
    {
        //putchars("\nx=");
        //print_hex(x);
    }
}

void update_progress_indicator(progress_indicator_t*pip, uint32_t progressed, uint32_t total)
{
    uint32_t t;
    lcd_clr_window(pip->b_color, pip->x, pip->y, pip->x+pip->w, pip->y+pip->h);
    t = pip->w*progressed/total;
    if(t>0)lcd_clr_window(pip->f_color, pip->x, pip->y, pip->x+t, pip->y+pip->h);
    lcd_lprintf(1, pip->x+pip->w+5, pip->y, "%d/%d", progressed, total);
}
#define AUTO_POWER_OFF_COUNT 100000
//static uint32_t single_timer_len = 16;
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
u32 task1_stack[STACK_SIZE_LOCAL];
u32 task2_stack[STACK_SIZE_LARGE*2];
u32 task_log_stack[STACK_SIZE_LOCAL];
u32 touch_stack[STACK_SIZE_LARGE];
u32 display_stack[STACK_SIZE_LARGE*2];
u32 cmd_stack[STACK_SIZE_LARGE];
u32 ui_stack[STACK_SIZE_LARGE*2];
u32 music_stack[STACK_SIZE_LARGE];
void main_init(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
  int looptimes = 3;
  //uint32_t ict;
  RCC_ClocksTypeDef RCC_ClocksStatus;

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_COMInit(COM1, &USART_InitStructure);
  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory. 
     You can monitor PD0 or PD2 on the scope to measure the output signal. 
     If you need to fine tune this frequency, you can add more GPIO set/reset 
     cycles to minimize more the infinite loop timing.
     This code needs to be compiled with high speed optimization option.  */

  //lprintf("debug_enable %X@%X vs %X\n", debug_enable, &debug_enable, 0xdeb49eab);
  if(0xdeb49eab==debug_enable){
      debug_mode = 1;
      debug_enable=0;
      os_is_running=0;
      lprintf("Go to debug mode\n");
      run_cmd_interface();
  }
  else{
      debug_mode = 0;
      lprintf("prepare debug\n");
      debug_enable = 0xdeb49eab;
  }
  systick_init();

  //PB3 PB4 PA15 PA13 PA14 set to gpio instead of SWJ
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
  //Touch_Test();

  os_task_init();

  //72M/72=1M, 1us/count
  //72M/12=6M, 1/6us / count
  timer_init(TIM2_RELOAD, 12-1);

  GPIO_InitTypeDef GPIO_InitStructure;
  //led
#ifndef RTC_SOC
  RTC_Init();
#endif
#ifdef ALIENTEK_MINI
  lprintf_time("\n\n================ALIENTEK_MINI board start================\n");
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,GPIO_Pin_8);
  GPIO_ResetBits(GPIOA,GPIO_Pin_13);

  RCC_APB2PeriphClockCmd(BEEP_GPIO_PERIPH, ENABLE);

  GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
  GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);

  RCC_APB2PeriphClockCmd(LED0_GPIO_PERIPH, ENABLE);

  GPIO_InitStructure.GPIO_Pin = LED0_GPIO_PIN;
  GPIO_Init(LED0_GPIO_GROUP, &GPIO_InitStructure);
  GPIO_ResetBits(LED0_GPIO_GROUP, LED0_GPIO_PIN);

  RCC_APB2PeriphClockCmd(LED1_GPIO_PERIPH, ENABLE);

  GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
  GPIO_Init(LED1_GPIO_GROUP, &GPIO_InitStructure);
  GPIO_ResetBits(LED1_GPIO_GROUP, LED1_GPIO_PIN);
#elif defined SUNRISE
  lprintf_time("\n\n================SUNRISE board start================\n");
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_0;//PB4 spi flash cs
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOC,GPIO_Pin_0);	
  GPIO_SetBits(GPIOC,GPIO_Pin_4);//spi flash cs =1

  RCC_APB2PeriphClockCmd(BEEP_GPIO_PERIPH, ENABLE);

  GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
  GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
#else
  lprintf_time("\n\n================Hamer board start================\n");
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_0;//PB4 spi flash cs
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
  GPIO_SetBits(GPIOB,GPIO_Pin_4);//spi flash cs =1

  RCC_APB2PeriphClockCmd(BEEP_GPIO_PERIPH, ENABLE);

  GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
  GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
#endif
  //led end

  //power off pin set low
  RCC_APB2PeriphClockCmd(POWEROFF_GPIO_PERIPH, ENABLE);

  GPIO_InitStructure.GPIO_Pin = POWEROFF_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(POWEROFF_GPIO_GROUP, &GPIO_InitStructure);
  GPIO_ResetBits(POWEROFF_GPIO_GROUP, POWEROFF_GPIO_PIN);
  //power off pin end

  RCC_GetClocksFreq(&RCC_ClocksStatus);
  lprintf_time("Version %s%s\n", VERSION, GIT_SHA1);
  get_rtc_time(0);
  lprintf_time("%s\n", get_rtc_time(0));
  lprintf("clk %d %d %d %d %d Hz\n\r",
		  RCC_ClocksStatus.SYSCLK_Frequency,
		  RCC_ClocksStatus.HCLK_Frequency,
		  RCC_ClocksStatus.PCLK1_Frequency,
		  RCC_ClocksStatus.PCLK2_Frequency,
		  RCC_ClocksStatus.ADCCLK_Frequency);
  lprintf_time("lcd init\n");
  lcd_sueb_init(0);
  lprintf_time("lcd init done.\n");
  lprintf_time("SD init\n");
  SD_Init();
  lprintf_time("SD init done\n");
  //SD_LowLevel_Init();

  /*1us/timer_count, 10ms/timer_intrpt*/
  while (looptimes--)
  {
      led_flash(0x3, 100);
  }
  beep_by_timer_100(0);
  os_task_add(os_task_log, task_log_stack, "log", STACK_SIZE_LOCAL, 5);
  os_task_add(os_task1, task1_stack, "t1", STACK_SIZE_LOCAL, 0);
  os_task_add(os_task2, task2_stack, "t2", STACK_SIZE_LARGE*2, 1);
  os_task_add(os_task3, cmd_stack, "cmd", STACK_SIZE_LARGE, 4);
  os_task_add(os_touch, touch_stack, "touch", STACK_SIZE_LARGE, 2);
  os_task_add(os_task_display, display_stack, "display", STACK_SIZE_LARGE*2, 7);
  os_task_add(task_music, music_stack, "music", STACK_SIZE_LARGE, 6);
  os_task_add(os_ui, ui_stack, "ui", STACK_SIZE_LOCAL*2, 3);
  while(1){
  }
}

#if 0
DECLARE_OS_LOCK(oslktest, 3);
void os_lock_test()
{
    lprintf("task %s wait lock\n", cur_os_task->name);
    os_lock(&oslktest);
    lprintf("task %s in\n", cur_os_task->name);
    os_10ms_delay(200);
    lprintf("task %s out\n", cur_os_task->name);
    os_unlock(&oslktest);
    lprintf("task %s release lock\n", cur_os_task->name);
}
#endif

void os_task1(void*p)
{
    int td = 0;
    u32 direct = 1;
    (void)p;
    while(1){
        //mem_print(cur_os_task, cur_os_task, sizeof(os_task_st));
        os_10ms_delay(td/2);
        //putchars("1 1\n");
        GPIO_SetBits(LED0_GPIO_GROUP,LED0_GPIO_PIN);
        os_10ms_delay(20-td/2+1);
        //putchars("1 0\n");
        GPIO_ResetBits(LED0_GPIO_GROUP,LED0_GPIO_PIN);
#if 1
        if(direct){
            td++;
            if(td>=40){
                direct = 0;
            }
        }
        if(!direct){
            td--;
            if(td<=0){
                direct = 1;
            }
        }
#endif
    }
}
void os_task3(void*p)
{
    (void)p;
    while(1){
        run_cmd_interface();
    }
}
u32 teset_td = 495;
void os_task2(void*p)
{
    (void)p;
    while(1){
        //mem_print(cur_os_task, cur_os_task, sizeof(os_task_st));
        //putchars("--0 0\n");
        GPIO_ResetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);
        os_10ms_delay(teset_td);
        //putchars("--0 1\n");
        GPIO_SetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);
        os_10ms_delay(teset_td);
        //lprintf("other task %d\n", *rtet);
#if 0
            {
                struct point pt_cache;
                os_lock(&oslk_evt);
                evt *dtw=RB_W_GET_wait(evt, rb_evt);
                //do work
                dtw->type = EVT_SCRN_TOUCH_UP;
                pt_cache.px=220;
                pt_cache.py=310;
                memcpy(dtw->pkg, &pt_cache, sizeof(struct point));
                RB_W_SET(evt, rb_evt);
                os_unlock(&oslk_evt);
            }
#endif
        task_timer(NULL);
    }
}
void soft_reset_system()
{
    lprintf_time("system reset\n");
    foce_save_log_func();
    set_BL_value(0);
    LCD_RESET();
    __disable_fault_irq();
    NVIC_SystemReset();
}

void reboot_download()
{
    lprintf_time("reboot download\n");
    GPIO_SetBits(GPIOA,GPIO_Pin_13);
    beep(100, 500);
    lprintf_time("reboot download\n");
    foce_save_log_func();
    soft_reset_system();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  lprintf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/************************util***********************/
uint add_with_limit(uint a, uint b, uint limit)
{
    uint ret = a + b;
    if(ret >= limit){
        ret -= limit;
    }
    return ret;
}
uint sub_with_limit(uint a, uint b, uint limit)
{
    uint ret;
    if(a>=b){
        ret = a-b;
    }
    else{
        ret = a+limit-b;
    }
    return ret;
}
