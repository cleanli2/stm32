/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "board.h"
#include "common.h"
#include "cmd.h"
#include "lprintf.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sd/stm32_eval_spi_sd.h"

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

static u8  fac_us=0;
static u16 fac_ms=0;
static uint32_t g_10ms_count = 0;
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
}

void led_flash(u32 led_flag, u32 ms_ct)
{
    if(led_flag & 0x2){
        GPIO_ResetBits(GPIOA,GPIO_Pin_14);	
    }
    if(led_flag & 0x1){
        GPIO_ResetBits(GPIOA,GPIO_Pin_15);	
    }
    delay_ms(ms_ct);
    if(led_flag & 0x2){
        GPIO_SetBits(GPIOA,GPIO_Pin_14);	
    }
    if(led_flag & 0x1){
        GPIO_SetBits(GPIOA,GPIO_Pin_15);	
    }
    delay_ms(200);
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

void beep(uint32_t hz, uint32_t t_ms)
{
    uint32_t pd, ct;
    if(hz > 1000000)return;

    pd = 1000000/hz/2;
    ct = hz*t_ms/1000;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,GPIO_Pin_5);	
    while(ct--){
	    GPIO_ResetBits(GPIOB,GPIO_Pin_5);	
	    delay_us(pd);
	    GPIO_SetBits(GPIOB,GPIO_Pin_5);	
	    delay_us(pd);
    }
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
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
    check_rtc_alert_and_clear();
    Show_Str(20, 630,RED,0xffff,(uint8_t*)"Power off in 3 seconds",24,0);
    beep(1000, 400);
    delay_ms(200);
    beep(800, 400);
    delay_ms(200);
    beep(600, 400);
    delay_ms(200);
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS);
    GPIO_SetBits(GPIOB,GPIO_Pin_0);	
    delay_ms(200);
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
date_info_t g_cur_date = {0};

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
static uint32_t single_timer_len = 16;
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
  int looptimes = 3;
  uint32_t ict;

  //PB3 PB4 PA15 PA13 PA14 set to gpio instead of SWJ
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
  //Touch_Test();

  //led
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
  //led end

  RCC_ClocksTypeDef RCC_ClocksStatus;
  delay_init();
#if 0
  /* GPIOC Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif

  USART_InitStructure.USART_BaudRate = 9600;
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
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  lprintf("Version %s%s\n", VERSION, GIT_SHA1);
  lprintf("clk %d %d %d %d %d Hz\n\r",
		  RCC_ClocksStatus.SYSCLK_Frequency,
		  RCC_ClocksStatus.HCLK_Frequency,
		  RCC_ClocksStatus.PCLK1_Frequency,
		  RCC_ClocksStatus.PCLK2_Frequency,
		  RCC_ClocksStatus.ADCCLK_Frequency);
  lcd_sueb_init(0);

  /*1us/timer_count, 10ms/timer_intrpt*/
  timer_init(10000, 72);
  while (looptimes--)
  {
      led_flash(0x3, 100);
  }
  //run_cmd_interface();
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
}

void soft_reset_system()
{
    set_BL_value(0);
    __disable_fault_irq();
    NVIC_SystemReset();
}

void reboot_download()
{
    GPIO_SetBits(GPIOA,GPIO_Pin_13);
    beep(800, 1000);
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

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
