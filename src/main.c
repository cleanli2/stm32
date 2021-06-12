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
void delay_init()
{

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us=SystemCoreClock/8000000;

    fac_ms=(u16)fac_us*1000;
}

void led_flash(u32 led_flag, u32 ms_ct)
{
    if(led_flag & 0x2){
        GPIO_ResetBits(GPIOA,GPIO_Pin_2);	
    }
    if(led_flag & 0x1){
        GPIO_ResetBits(GPIOA,GPIO_Pin_12);	
    }
    delay_ms(ms_ct);
    if(led_flag & 0x2){
        GPIO_SetBits(GPIOA,GPIO_Pin_2);	
    }
    if(led_flag & 0x1){
        GPIO_SetBits(GPIOA,GPIO_Pin_12);	
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
    Show_Str(20, 630,0xff00,0xffff,"Power off in 3 seconds",24,0);
    beep(1000, 500);
    delay_ms(1000);
    beep(1000, 500);
    delay_ms(1000);
    beep(1000, 500);
    delay_ms(1000);
    auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS);
    GPIO_SetBits(GPIOB,GPIO_Pin_0);	
}

void my_repeat_timer(uint32_t w_repts, uint32_t seconds)
{
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
}

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
  //led
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,GPIO_Pin_12|GPIO_Pin_2);	

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
  //Touch_Test();
  while (looptimes--)
  {
      led_flash(0x3, 100);
  }
  //run_cmd_interface();
  ict=0;
  lcd_clr_window(0xf00f, 0, 0, 100, 100);
  lcd_clr_window(0xff0f, 0, 100, 100, 200);
  lcd_clr_window(0xff0f, 0, 200, 100, 300);
  lcd_lprintf(5, 250, "PowerOff");
  lcd_clr_window(0, 120, 760, 400, 780);
  auto_time_alert_set(AUTO_TIME_ALERT_INC_MINS);
  {
      uint8_t rtc_alrt;
      char*date = get_rtc_time(0);
      Show_Str(190, 700,0,0xffff,date,24,0);
      if(check_rtc_alert_and_clear()){
          my_repeat_timer(3, 300);
      }
  }
  while(1){
      int tx = 0, ty=0;
      if(ict++>1000){
          char*date = get_rtc_time(0);
          
          Show_Str(190, 700,0,0xffff,date,24,0);
          adc_test();
          ict = 0;
      }
      delay_ms(1);
      if(get_TP_point(&tx, &ty)){
          TP_Draw_Big_Point(tx,ty,BLACK);		//画图	  			   
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
      }
      if(con_is_recved() && (con_recv() == 'c')){
          run_cmd_interface();
      }
  }
}

void soft_reset_system()
{
    set_BL_value(0);
    __disable_fault_irq();
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

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
