#include "stm32f10x.h"
#include "board.h"
#include "common.h"
#include "cmd.h"
#include "lprintf.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "display.h"
#include "mock_uart.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */
extern unsigned long debug_enable;

u32 intrpt_time[NUM_INTRPT]={0};
u32 debug_mode = 0;
#define COUNTS_PER_US 6

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef g_gpio_inits;
USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

u32 last_systick;
u32 interv_systick;
u32 g_cam_r70p_e=0;
u32 g_cam_r71p_e=0;
u32 g_tlcd=0;
u32 g_pcf8574_hw=0;
#ifdef SVR
u32 g_lockposi=0;
#endif
static int sound_enable=1;
volatile uint32_t g_10ms_count = 0;
uint32_t g_ms_count = 0;
void compute_cpu_occp();
struct emulate_touch g_fake_touch = {0};
struct emulate_touch *gftp=&g_fake_touch;
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

void SysTick_Handler()
{
    g_ms_count++;
}

void systick_init()
{

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_Config(8000-1);
    NVIC_SetPriority (PendSV_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    NVIC_SetPriorityGrouping(0x7);//no interrupt preempt
}

uint32_t shot_msct;
uint32_t shot_systick_val;
uint64_t get_system_us()
{
    uint64_t system_us_count;

    shot_msct=g_ms_count;
    shot_systick_val=SysTick->VAL;
    system_us_count = (uint64_t)shot_msct * 1000 + (8000-1-shot_systick_val)/8;
    return system_us_count;
}

u32 dt_us_last()
{
    static uint64_t last_s_us=0;
    uint64_t cur_s_us=get_system_us();
    u32 dt = (u32)(cur_s_us-last_s_us);
    last_s_us = cur_s_us;
    return dt;
}

void cam_working_confirm();
u32*TIM2_IRQHandler_local(u32*stack_data)
{
    (void)stack_data;
    tm_cpt_start();
	//if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    g_10ms_count++;
    if((g_10ms_count%3000)==0){//30seconds
    }
    //*(u32*)0xe000ed04=0x10000000;
    intrpt_time[INTTIM2]=tm_cpt_end();
    return stack_data;
}

static int g_led_cache=0;
void toggle_led(int i)
{
    (void)i;
    if(g_led_cache){
        g_led_cache=0;
        GPIO_ResetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);
    }
    else{
        g_led_cache=1;
        GPIO_SetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);
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
            //GPIO_ResetBits(LED0_GPIO_GROUP,LED0_GPIO_PIN);
        }
        else{
            //GPIO_SetBits(LED0_GPIO_GROUP,LED0_GPIO_PIN);
        }
    }
}

void led_flash(u32 led_flag, u32 ms_ct)
{
    (void)led_flag;
    toggle_led(0);
    toggle_led(1);
    delay_ms(ms_ct);
    toggle_led(0);
    toggle_led(1);
    delay_ms(ms_ct);
}

void delay_us(u32 nus)
{
#if 0
    uint64_t s=get_system_us();
    s+=nus;
    while(get_system_us()<s);
#else
    volatile int a=nus;
    while(a--);
#endif
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

u32 device_serial0, device_serial1, device_serial2;
void get_mcu_id(void)
{
    device_serial0 = *(u32*)(0x1ffff7e8);
    device_serial1 = *(u32*)(0x1ffff7ec);
    device_serial2 = *(u32*)(0x1ffff7f0);
    lprintf_time("MCU ID=%X %X %X\n",
            device_serial0, device_serial1, device_serial2);
}

void power_off()
{
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
void poweroff(char *p);
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

#if 0
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
#endif
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

  RCC_APB2PeriphClockCmd(LED1_GPIO_PERIPH, ENABLE);
  g_gpio_inits.GPIO_Mode = GPIO_Mode_Out_PP;
  g_gpio_inits.GPIO_Pin = LED1_GPIO_PIN;
  g_gpio_inits.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_GPIO_GROUP, &g_gpio_inits);
  GPIO_SetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);

#ifdef SVR
  RCC_APB2PeriphClockCmd(MOS_PERIPH, ENABLE);
  GPIO_ResetBits(MOS_GP,MOS_PIN);
  g_gpio_inits.GPIO_Mode = GPIO_Mode_Out_PP;
  g_gpio_inits.GPIO_Pin = MOS_PIN;
  g_gpio_inits.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MOS_GP, &g_gpio_inits);

  RCC_APB2PeriphClockCmd(LOCKPOSI_PERIPH, ENABLE);
  GPIO_SetBits(LOCKPOSI_GP,LOCKPOSI_PIN);
  g_gpio_inits.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  g_gpio_inits.GPIO_Pin = LOCKPOSI_PIN;
  g_gpio_inits.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LOCKPOSI_GP, &g_gpio_inits);

  RCC_APB2PeriphClockCmd(LOCKLOCK_PERIPH, ENABLE);
  GPIO_SetBits(LOCKLOCK_GP,LOCKLOCK_PIN);
  g_gpio_inits.GPIO_Mode = GPIO_Mode_IPU;
  g_gpio_inits.GPIO_Pin = LOCKLOCK_PIN;
  g_gpio_inits.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LOCKLOCK_GP, &g_gpio_inits);

  g_lockposi=get_env_uint("lpe", 0);;
#endif

  //72M/72=1M, 1us/count
  //72M/12=6M, 1/6us / count
  systick_init();

  lprintf("~~~~~~\n");
  lprintf_time("\n\n================c6t6 board start================\n");
  lprintf("SystemCoreClock=%d\n", SystemCoreClock);

  RCC_GetClocksFreq(&RCC_ClocksStatus);
  get_mcu_id();
  lprintf_time("Version %s%s\n", VERSION, GIT_SHA1);

  lprintf("clk %d %d %d %d %d Hz\n\r",
		  RCC_ClocksStatus.SYSCLK_Frequency,
		  RCC_ClocksStatus.HCLK_Frequency,
		  RCC_ClocksStatus.PCLK1_Frequency,
		  RCC_ClocksStatus.PCLK2_Frequency,
		  RCC_ClocksStatus.ADCCLK_Frequency);
  //lprintf_time("NO lcd init.\n");
  //SD_LowLevel_Init();

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  if(PWR_GetFlagStatus(PWR_FLAG_WU)!=RESET){
      lprintf("boot<Standby\n");
      PWR_ClearFlag(PWR_FLAG_SB);
      PWR_WakeUpPinCmd (DISABLE);
  }
  else{
      lprintf("boot<reset\n");
  }
  uint32_t s1=get_system_us();
  uint32_t s2=get_system_us();
  uint32_t s3=get_system_us();
  uint32_t s4=get_system_us();
  prt_dec(s2-s1);
  prt_dec(s3-s2);
  prt_dec(s4-s3);
  prt_dec(s1);
  prt_dec(s2);
  prt_dec(s3);
  mock_uart_init();
  delay_ms(200);
  if(con_is_recved()){
      if('c'==con_recv()){
          run_cmd_interface();
      }
  }
#ifdef SVR
/*
  if(g_lockposi){
      if(0==GPIO_ReadInputDataBit(LOCKLOCK_GP, LOCKLOCK_PIN)){
          lock_lock();
          poweroff("standby");
      }
  }
  */
#endif
}


void soft_reset_system()
{
    lprintf_time("system reset\n");
    NVIC_SystemReset();
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
