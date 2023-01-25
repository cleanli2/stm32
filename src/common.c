#include "stm32f10x.h"
#include "board.h"
#include "common.h"
#include "cmd.h"
#include "lprintf.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sd/stm32_eval_spi_sd.h"
#include "wave_data.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static int sound_enable=1;
static u8  fac_us=0;
static u16 fac_ms=0;
static uint32_t g_10ms_count = 0;
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

void delay_init()
{

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us=SystemCoreClock/8000000;

    fac_ms=(u16)fac_us*1000;
}

uint64_t get_system_us()
{
    uint64_t system_us_count;

    system_us_count = g_10ms_count * 10000 + TIM_GetCounter(TIM2);
    return system_us_count;
}

void TIM2_IRQHandler(void)
{
	//if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        g_10ms_count++;
	}
    if(SOUND_BEEP_MODE==get_sound_mode()){
        sound_execute();
    }
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
    u32 temp;
    SysTick->LOAD=nus*fac_us;
    SysTick->VAL=0x00;
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL =0X00;
}

void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD=(u32)nms*fac_ms;
    SysTick->VAL =0x00;
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL =0X00;
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

int __io_putchar(int ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(BOARD_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(BOARD_COM1, USART_FLAG_TC) == RESET)
  {}
  if(ch == '\n')
      USART_SendData(BOARD_COM1, '\r');
  while (USART_GetFlagStatus(BOARD_COM1, USART_FLAG_TC) == RESET);

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

u32 get_con_char_10ms(u32 tout)
{
    tout += g_10ms_count;
    while(!con_is_recved() && g_10ms_count < tout);
    if(con_is_recved()){
        return con_recv();
    }
    else{
        return 0x100;
    }
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
        TIM_TimeBaseStructure.TIM_Period = 200000000/hz_100-1;
        TIM_TimeBaseStructure.TIM_Prescaler = 18-1;
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

void TIM3_IRQHandler(void)
{
    static int tog=0;
    //if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if(sound_enable){
        if(SOUND_BEEP_MODE==get_sound_mode()){
            if(tog++&0x1){
                GPIO_SetBits(BEEP_GPIO_GROUP,BEEP_GPIO_PIN);
            }
            else{
                GPIO_ResetBits(BEEP_GPIO_GROUP,BEEP_GPIO_PIN);
            }
        }
#ifdef DAC_SUPPORT
        else if(SOUND_DAC_MODE==get_sound_mode()){
            sound_execute_dac();
        }
#endif
    }
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

void mytimer(uint32_t w_seconds)
{
    uint32_t seconds = w_seconds;
    uint32_t t;
    lcd_clr_window(0, 120, 10, 400, 30);
    while(seconds){
        delay_ms(1000);
        seconds--;
        t = (400-120)*(w_seconds-seconds)/w_seconds;
        if(t>0) lcd_clr_window(0xff, 120, 10, t+120, 30);
        lcd_lprintf(430, 10, "%d", seconds);
    }
    lprintf("beep\n");
    beep(1000, 3000);
}

void power_off()
{
    Show_Str(20, 630,RED,0xffff,"Power off in 3 seconds",24,0);
    lprintf_time("power off in 3 secs\n");
    foce_save_log_func();
    beep(600, 100);
#ifdef RTC_8563
    check_rtc_alert_and_clear();
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS, -1, -1);
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
void update_progress_indicator(progress_indicator_t*pip, uint32_t progressed, uint32_t total)
{
    uint32_t t;
    lcd_clr_window(pip->b_color, pip->x, pip->y, pip->x+pip->w, pip->y+pip->h);
    t = pip->w*progressed/total;
    if(t>0)lcd_clr_window(pip->f_color, pip->x, pip->y, pip->x+t, pip->y+pip->h);
    lcd_lprintf(pip->x+pip->w+5, pip->y, "%d/%d", progressed, total);
}
void my_repeat_timer(uint32_t w_repts, uint32_t seconds)
{
#if 0
    uint32_t repts = w_repts;
    uint32_t t;
    lcd_clr_window(0, 120, 50, 400, 70);
    beep(1000, 3000);
    while(repts){
        mytimer(seconds);
        repts--;
        t = (400-120)*(w_repts-repts)/w_repts;
        if(t>0)lcd_clr_window(0xff, 120, 50, t+120, 70);
        lcd_lprintf(430, 50, "%d", repts);
    }
    power_off();
#endif
    if(g_timer.running){
        g_timer.running = 0;
        clear_progress_indicator(&g_timer_repeat_pi);
        clear_progress_indicator(&g_timer_pi);
        beep(400, 300);
        return;
    }
    g_timer.timeout_poff = 1;
    get_rtc_time(&g_timer.start);
    g_timer.seconds_len = seconds;
    g_timer.running = 1;
    g_timer.repeat = w_repts;
    g_timer.to_repeat = w_repts;
    if(w_repts>1){
        update_progress_indicator(&g_timer_repeat_pi, 0, g_timer.repeat);
    }
    beep(800, 3000);
}
#define AUTO_POWER_OFF_COUNT 100000
//static uint32_t single_timer_len = 16;
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
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

  delay_init();

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

  //PB3 PB4 PA15 PA13 PA14 set to gpio instead of SWJ
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
  //Touch_Test();

  //72M/72=1M, 1us/count
  timer_init(10000-1, 72-1);

  GPIO_InitTypeDef GPIO_InitStructure;
  //led
#ifdef RTC_SOC
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
  //while(1) run_cmd_interface();
#if 0
  ict=0;
  lcd_clr_window(0xf00f, 0, 0, 100, 100);
  lcd_clr_window(BLUE, 0, 100, 100, 200);
  lcd_clr_window(0xff0f, 0, 200, 100, 300);
  lcd_lprintf(5, 250, "PowerOff");
  lcd_clr_window(0xff, 0, 300, 100, 400);
  lcd_lprintf(5, 350, "Timer");

  //reboot to download mode
  lcd_clr_window(RED, 0, 400, 100, 480);
  lcd_lprintf(5, 450, "Download");

  lcd_clr_window(0, 120, 760, 400, 780);
  lcd_clr_window(0xf0f, 120, 760, 163, 780);

  lcd_clr_window(GREEN, 40, 500, 440, 600);
  lcd_clr_window(0xf0f, 56, 500, 440, 600);
  LCD_DrawLine(71, 500, 71, 600);
  LCD_DrawLine(109, 500, 109, 600);//5min
  LCD_DrawLine(138, 500, 138, 600);//10min
  LCD_DrawLine(210, 500, 210, 600);//30min
  LCD_DrawLine(280, 500, 280, 600);//60min
  lcd_lprintf(60, 580, "0 hours 0 minutes 16 seconds");
  lcd_lprintf(68, 620, "1    5   10       30        60  mins");
  auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS);
  {
      char*date = get_rtc_time(0);
      Show_Str(190, 700,0,0xffff,(uint8_t*)date,24,0);
      if(check_rtc_alert_and_clear()){
          my_repeat_timer(3, 300);
      }
  }
  while(1){
      static uint32_t no_touch_count = 0;
      uint16_t tx = 0, ty=0;
      if(ict==0){
          char*date = get_rtc_time(&g_cur_date);
          if(g_timer.running){//timer
              uint32_t timer_ran = time_diff_seconds(&g_cur_date, &g_timer.start);
              if(timer_ran>g_timer.seconds_len){
                  beep(800, 3000);
                  if(g_timer.to_repeat>1){
                      g_timer.to_repeat--;
                      g_timer.start = g_cur_date;
                      update_progress_indicator(&g_timer_repeat_pi,
                              g_timer.repeat - g_timer.to_repeat,
                              g_timer.repeat);
                  }
                  else{
                      g_timer.running = 0;
                      if(g_timer.timeout_poff){
                          power_off();
                      }
                  }
              }
              else{
                  update_progress_indicator(&g_timer_pi, timer_ran, g_timer.seconds_len);
              }
          }
          Show_Str(190, 700,0,0xffff,(uint8_t*)date,24,0);
          adc_test();
          lprintf("sys_us %U\n", get_system_us());;
      }
      if(ict++>600){
          ict = 0;
      }
      delay_ms(1);
      if(get_TP_point(&tx, &ty)){
          no_touch_count = 0;
          if(get_BL_value()<=DEFAULT_IDLE_BL){
              set_BL_value(DEFAULT_BL);
          }
          TP_Draw_Big_Point(tx,ty,BLACK);		//画图
          if(tx < 100 && ty < 480 && ty > 400){//single timer
              reboot_download();
          }
          if(tx < 100 && ty < 400 && ty > 300){//single timer
              my_repeat_timer(1, single_timer_len);
          }
          if(tx < 100 && ty < 300 && ty > 200){//poweroff
              power_off();
          }
          if(tx < 100 && ty < 200 && ty > 100){
              my_repeat_timer(3, 10);
              //beep(1000, 2000);
          }
          if(tx < 100 && ty < 100){
              my_repeat_timer(3, 300);
              //beep(1000, 2000);
          }
          if(tx < 400 && tx > 120 && ty < 780 && ty > 760){
              uint16_t lcd_brt;
              lcd_clr_window(0, 120, 760, 400, 780);
              lcd_clr_window(0xf0f, 120, 760, tx, 780);
              lcd_brt = (tx - 120)*100/280;
              set_BL_value(lcd_brt);
          }
          if(tx < 440 && tx > 40 && ty < 600 && ty > 500){
              if(!g_timer.running){
                  uint32_t h, m, s;
                  lcd_clr_window(GREEN, 40, 500, 440, 600);
                  lcd_clr_window(0xf0f, 40, 500, tx, 600);
                  LCD_DrawLine(71, 500, 71, 600);//11min
                  LCD_DrawLine(109, 500, 109, 600);//5min
                  LCD_DrawLine(138, 500, 138, 600);//10min
                  LCD_DrawLine(210, 500, 210, 600);//30min
                  LCD_DrawLine(280, 500, 280, 600);//60min
                  single_timer_len = (tx - 40)*(tx - 40)/16;
                  h = single_timer_len/3600;
                  m = (single_timer_len%3600)/60;
                  s = single_timer_len%60;
                  lcd_lprintf(60, 580, "%d hours %d minutes %d seconds", h, m, s);
              }
          }
      }
      if(con_is_recved() && (con_recv() == 'c')){
          run_cmd_interface();
      }
      if(!g_timer.running){
          if(no_touch_count>AUTO_POWER_OFF_COUNT){
              if((no_touch_count-AUTO_POWER_OFF_COUNT)%1000 == 0){
                  beep(1000-(no_touch_count-AUTO_POWER_OFF_COUNT)/100, 200);
              }
              if(no_touch_count-AUTO_POWER_OFF_COUNT>9000)power_off();
          }
      }
      if(no_touch_count++>AUTO_POWER_OFF_COUNT/5){
          if(((no_touch_count/2000)%10) == 0){
              if(get_BL_value() != DEFAULT_BL) set_BL_value(DEFAULT_BL);
          }
          else{
              if(get_BL_value() != DEFAULT_IDLE_BL) set_BL_value(DEFAULT_IDLE_BL);
          }
      }
  }
#endif
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
uint sub_with_limit(uint a, uint b, uint limit)
{
    if(a >= b){
        return a - b;
    }
    else{
        return a + limit - b;
    }
}

uint add_with_limit(uint a, uint b, uint limit)
{
    uint ret = a + b;
    if(ret >= limit){
        ret -= limit;
    }
    return ret;
}

/*DAC code*/
static int global_sound_mode = SOUND_BEEP_MODE;
int get_sound_mode()
{
    return global_sound_mode;
}

#ifdef DAC_SUPPORT
static uint32_t dac_freq = 5500;//11k
static uint32_t pack_index = 0;
struct wave_header g_whd;
void dac_set_freq(uint32_t freq)
{
    lprintf("set dac freq=%u cur=%u\n", freq, dac_freq);
    dac_freq = freq;
    beep_by_timer(dac_freq);//11k
}
void Dac1_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //enable port A clk
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //enable DAC clk

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //config port
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //analog input
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4)	;//PA.4 output high

	DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//no trigger
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//no wave
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//mask ampitude
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Enable ;	//DAC1 output buf open, BOFF1=0
	DAC_Init(DAC_Channel_1,&DAC_InitType);	 //init DAC1

	DAC_Cmd(DAC_Channel_1, ENABLE);  //enable DAC1

	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12bit right align

    dac_set_freq(5500);//11k
    global_sound_mode = SOUND_DAC_MODE;
    pack_index = 0;
    lprintf("dac1 init done\n");
}

void Dac1_Set_Vol(u32 vol)
{
    //lprintf("set vol %X\n", vol&0xfff);
	DAC_SetChannel1Data(DAC_Align_12b_R,vol&0xFFF);
}

u16 sin_value[10]={
    0,
    711,
    1401,
    2048,
    2632,//40
    3137,
    3546,
    3849,//70
    4033,
    4095
};
#define SIN_SIZE (sizeof(sin_value)/sizeof(sin_value[0]))

u16 get_sin_value(u32 x, u32 n)
{
    u16 ret, total, index;
    u32 dx;
    u32 sin_size=sizeof(sin_value)/sizeof(sin_value[0]);
    total = n*(sin_size-1);
    while(x>total)x-=total;
    index = x / n;
    dx = x - index*n;
    ret = sin_value[index] + dx * (sin_value[index+1] - sin_value[index]) / n;
    return ret;
}

void Dac1_wave(u32 type, u32 para2)
{
    u32 va=0,i, n=para2, t_sbl;
    static u32 dac_data_ct=0;
    uint8_t *wd;
    uint16_t *wd16b;
    //lprintf("dac1 wave type %d\n", type);
    switch(type){
        case 0:
            for(va=0;va<0x1000;va+=40){
                while(dac_sound_pool_full());
                dac_put_sound(va);
            }
            break;
        case 1:
            for(i=0;i<4*n*(SIN_SIZE-1);i++){
                va=i;
                if(va<=(SIN_SIZE-1)*n){
                    va=get_sin_value(va, n)/2+2048;
                }
                else if(va<(SIN_SIZE-1)*n*2){
                    va=(get_sin_value(2*(SIN_SIZE-1)*n-va, n)/2+2048);
                }
                else if(va<(SIN_SIZE-1)*n*3){
                    va=(2048-get_sin_value(va-2*(SIN_SIZE-1)*n, n)/2);
                }
                else{
                    va=(2048-get_sin_value(4*(SIN_SIZE-1)*n-va, n)/2);
                }
                while(dac_sound_pool_full());
                dac_put_sound(va);
            }
            break;
        case 2:
            for(va=0;va<pcm_wav_len;va++){
                while(dac_sound_pool_full());
                dac_put_sound(pcm_wav[va]<<4);
            }
            break;
        case 3:
            t_sbl=dac_get_sound_size();
            wd=(uint8_t*)para2;
            wd16b=(uint16_t*)para2;
            if(0==pack_index){
                struct wave_header*whd=(struct wave_header*)wd;
                g_whd=*whd;
                lprintf("wave file info:\n");
                lprintf("ChunkID:      0x%x\n", whd->ChunkID      );
                lprintf("ChunkSize:      %d\n", whd->ChunkSize    );
                lprintf("Format:       0x%x\n", whd->Format       );
                lprintf("SubChunk1ID:  0x%x\n", whd->SubChunk1ID  );
                lprintf("SubChunk1Size:  %d\n", whd->SubChunk1Size);
                lprintf("AudioFormat:  0x%x\n", whd->AudioFormat  );
                lprintf("NumChannels:  0x%x\n", whd->NumChannels  );
                lprintf("SampleRate:     %d\n", whd->SampleRate   );
                lprintf("ByteRate:       %d\n", whd->ByteRate     );
                lprintf("BlockAlign:   0x%x\n", whd->BlockAlign   );
                lprintf("BitsPerSample:0x%x\n", whd->BitsPerSample);
                lprintf("SubChunk2ID:  0x%x\n", whd->SubChunk2ID  );
                lprintf("SubChunk2Size:  %d\n", whd->SubChunk2Size);
                if(0x46464952!=whd->ChunkID //'RIFF'
                        ||0x45564157!=whd->Format //'WAVE'
                        ||0x20746D66!=whd->SubChunk1ID  //'fmt '
                        ||0x1!=whd->AudioFormat  //PCM = 1
                        ){
                    lprintf("WARNING:not support wave file\n");
                }
                if(1!=whd->NumChannels){
                    lprintf("WARNING:not single channel wave file\n");
                }
                if(11025!=whd->SampleRate){
                    lprintf("WARNING:not 11025Hz\n");
                }
                wd=&whd->data;
                wd16b=(uint16_t*)&whd->data;
            }
            for(va=0;va<512;va++){
                while(dac_sound_pool_full());
                if(16==g_whd.BitsPerSample){
                    dac_put_sound((wd16b[va]+0x7fff)>>4);
                    if(255<=va){
                        break;
                    }
                }
                else{
                    dac_put_sound(wd[va]<<4);
                }
            }
            if(0==t_sbl || dac_data_ct++>20){
                lprintf("S+%d\n", t_sbl);
                dac_data_ct=0;
            }
            pack_index++;
            //lprintf_to("S-%d\n", dac_get_sound_size());
            break;
        default:
            break;
    }
}

void Dac1_DeInit(void)
{
	DAC_Cmd(DAC_Channel_1, DISABLE);
    DAC_DeInit();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, DISABLE);
    beep_by_timer_100(0);
    global_sound_mode = SOUND_BEEP_MODE;
    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
    GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
    lprintf("dac1 deinit done\n");
}
#endif
