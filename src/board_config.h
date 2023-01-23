#ifndef _BOARD_CONDIG_H
#define _BOARD_CONDIG_H

#include"stdint.h"
#include "sys.h"
#include "stdlib.h"








#ifdef ALIENTEK_MINI
/////////////////////////////////////ALIENTEK_MINI//////////////////////////////////////////////
#define POWER_MONITOR
#define GPIO_ADC_VREF_PIN GPIO_Pin_1
#define GPIO_ADC_V4_2_PIN GPIO_Pin_4
#define GPIO_ADC_IBAT_PIN GPIO_Pin_0
#define ADC_Channel_VREF ADC_Channel_1
#define ADC_Channel_V4_2 ADC_Channel_4
#define ADC_Channel_IBAT ADC_Channel_0
#define GPIO_GROUP_VREF GPIOA
#define GPIO_GROUP_V4_2_IBAT GPIOA
#define CURRENT_MEASUREMENT_CALIBRATION 350/1000
#define V4_2_RATIO 2
#define ADC_DEBUG

#define BEEP_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define BEEP_GPIO_GROUP GPIOA
#define BEEP_GPIO_PIN GPIO_Pin_14

#define LED1_GPIO_PERIPH RCC_APB2Periph_GPIOD
#define LED1_GPIO_GROUP GPIOD
#define LED1_GPIO_PIN GPIO_Pin_2
#define LED0_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED0_GPIO_GROUP GPIOA
#define LED0_GPIO_PIN GPIO_Pin_8

#define POWEROFF_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define POWEROFF_GPIO_GROUP GPIOA
#define POWEROFF_GPIO_PIN GPIO_Pin_15
//lcd
//#define LCD_USE8BIT_MODEL   0	//定义数据总线是否使用8位模式 0,使用16位模式.1,使用8位模式

#define RTC_8563
#define I2C_GPIO_PERIPH RCC_APB2Periph_GPIOC
#define I2C_GROUP GPIOC
#define SDA_GG GPIOC
#define SCL_GG GPIOC
#define SDA_PIN GPIO_Pin_11
#define SCL_PIN GPIO_Pin_12


////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define GPIO_TYPE  GPIOC  //GPIO组类型
#define LED      10        //背光控制引脚      PA8
#define LCD_CS   9       //片选引脚            PA0
#define LCD_RS   8       //寄存器/数据选择引脚 PA1 
#define LCD_RST  13       //复位引脚            PC13
#define LCD_WR   7       //写引脚              PC15
#define LCD_RD   6       //读引脚              PC14

//如果使用官方库函数定义下列底层，速度将会下降到14帧每秒，建议采用我司推荐方法
//以下IO定义直接操作寄存器，快速IO操作，刷屏速率可以达到28帧每秒！ 

//GPIO置位（拉高）
#define	LCD_CS_SET  GPIOC->BSRR=1<<LCD_CS    //片选端口  	
#define	LCD_RS_SET	GPIOC->BSRR=1<<LCD_RS    //数据/命令    
#define	LCD_RST_SET	GPIOC->BSRR=1<<LCD_RST   //复位			  
#define	LCD_WR_SET	GPIOC->BSRR=1<<LCD_WR    //写 	
#define LCD_RD_SET  GPIOC->BSRR=1<<LCD_RD    //读		  
#define LCD_LED_SET  GPIOC->BSRR=1<<LED      //读		  

//GPIO复位（拉低）							    
#define	LCD_CS_CLR  GPIOC->BRR=1<<LCD_CS     //片选端口  	
#define	LCD_RS_CLR	GPIOC->BRR=1<<LCD_RS     //数据/命令  	 
#define	LCD_RST_CLR	GPIOC->BRR=1<<LCD_RST    //复位			  
#define	LCD_WR_CLR	GPIOC->BRR=1<<LCD_WR     //写
#define LCD_RD_CLR  GPIOC->BRR=1<<LCD_RD     //读	  		  
#define LCD_LED_CLR  GPIOC->BRR=1<<LED      //读		  

#define LCD_PORT_GPIO_Pins               ((uint16_t)0xFFFF)  /*!< High 8 pins selected */
//PB0~15,作为数据线
//注意：如果使用8位模式数据总线，则液晶屏的数据高8位是接到MCU的高8位总线上
//举例：如果接8位模式则本示例接线为液晶屏DB10-DB17对应接至单片机GPIOB_Pin8-GPIOB_Pin15
//举例：如果是16位模式：DB0-DB7分别接GPIOB_Pin0-GPIOB_Pin7,DB10-DB17对应接至单片机GPIOB_Pin8-GPIOB_Pin15
//Note:DB4<->A3 DB3<->A2 for HW problem
//NOTE:the HW problem above is not problem, it's caused by error config
#define DATAOUT(x) {GPIOB->ODR=(x);}
#define DATAIN()     (GPIOB->IDR) //数据输入

//////////////////////////////////////////////////////////////////////////////////	  
//定义LCD的尺寸
#define LCD_W 240
#define LCD_H 320
//#define LARGE_SCREEN

//touch screen
//与触摸屏芯片连接引脚	   
#define PEN_GG GPIOC
#define DOUT_GG GPIOC
#define TDIN_GG GPIOC
#define TCLK_GG GPIOC
#define TCS_GG GPIOC

#define PEN_PIN GPIO_Pin_1
#define DOUT_PIN GPIO_Pin_2
#define TDIN_PIN GPIO_Pin_3
#define TCLK_PIN GPIO_Pin_0
#define TCS_PIN GPIO_Pin_13

#define RCC_APB2Periph_GG_PEN RCC_APB2Periph_GPIOC
#define RCC_APB2Periph_GG_TCS RCC_APB2Periph_GPIOC
#define RCC_APB2Periph_GG_TCLK RCC_APB2Periph_GPIOC
#define RCC_APB2Periph_GG_DOUT RCC_APB2Periph_GPIOC
#define RCC_APB2Periph_GG_TDIN RCC_APB2Periph_GPIOC
//touch screen use spi common with sd/flash
//#define TS_SPI_USE_COMMON 0

#define TOUCH_CALI_xFAC 0xFF9DE77F
#define TOUCH_CALI_yFAC 0xFF774800
#define TOUCH_CALI_xOFF 0xFA
#define TOUCH_CALI_yOFF 0x15D

//spi flash
#define SF_CS_PIN                 GPIO_Pin_2
#define SF_CS_GPIO_PORT           GPIOA
#define SF_CS_GPIO_CLK            RCC_APB2Periph_GPIOA

#define SD_CS_PIN                 GPIO_Pin_3
#define SD_CS_GPIO_PORT           GPIOA
#define SD_CS_GPIO_CLK            RCC_APB2Periph_GPIOA

#define	SPI_FLASH_CS PAout(2)  //选中FLASH					 

#elif defined SUNRISE
/////////////////////////////////////SUNRISE//////////////////////////////////////////////

#define DAC_SUPPORT

#define POWER_MONITOR
#define GPIO_ADC_VREF_PIN GPIO_Pin_5
#define GPIO_ADC_V4_2_PIN GPIO_Pin_2
#define GPIO_ADC_IBAT_PIN GPIO_Pin_3
#define ADC_Channel_VREF ADC_Channel_15
#define ADC_Channel_V4_2 ADC_Channel_2
#define ADC_Channel_IBAT ADC_Channel_3
#define GPIO_GROUP_VREF GPIOC
#define GPIO_GROUP_V4_2_IBAT GPIOA
#define CURRENT_MEASUREMENT_CALIBRATION 1025/1000
#define V4_2_RATIO 2
//#define ADC_DEBUG


#define BEEP_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define BEEP_GPIO_GROUP GPIOA
#define BEEP_GPIO_PIN GPIO_Pin_4

#define LED1_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED1_GPIO_GROUP GPIOA
#define LED1_GPIO_PIN GPIO_Pin_14
#define LED0_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED0_GPIO_GROUP GPIOA
#define LED0_GPIO_PIN GPIO_Pin_15

#define POWEROFF_GPIO_PERIPH RCC_APB2Periph_GPIOC
#define POWEROFF_GPIO_GROUP GPIOC
#define POWEROFF_GPIO_PIN GPIO_Pin_0
//lcd
#define LCD_USE8BIT_MODEL   0	//定义数据总线是否使用8位模式 0,使用16位模式.1,使用8位模式

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define GPIO_TYPE  GPIOC  //GPIO组类型
#define LED      8        //背光控制引脚      PA8
#define LCD_CS   0       //片选引脚            PA0
#define LCD_RS   1       //寄存器/数据选择引脚 PA1 
#define LCD_RST  13       //复位引脚            PC13
#define LCD_WR   15       //写引脚              PC15
#define LCD_RD   14       //读引脚              PC14

#define RTC_8563
#define I2C_GPIO_PERIPH RCC_APB2Periph_GPIOC
#define I2C_GROUP GPIOC
#define SDA_GG GPIOC
#define SCL_GG GPIOC
#define SDA_PIN GPIO_Pin_11
#define SCL_PIN GPIO_Pin_10

//如果使用官方库函数定义下列底层，速度将会下降到14帧每秒，建议采用我司推荐方法
//以下IO定义直接操作寄存器，快速IO操作，刷屏速率可以达到28帧每秒！ 

//GPIO置位（拉高）
#define	LCD_CS_SET  GPIOA->BSRR=1<<LCD_CS    //片选端口  	
#define	LCD_RS_SET	GPIOA->BSRR=1<<LCD_RS    //数据/命令    
#define	LCD_RST_SET	GPIOC->BSRR=1<<LCD_RST   //复位			  
#define	LCD_WR_SET	GPIOC->BSRR=1<<LCD_WR    //写 	
#define LCD_RD_SET  GPIOC->BSRR=1<<LCD_RD    //读		  
#define LCD_LED_SET  GPIOA->BSRR=1<<LED      //读		  

//GPIO复位（拉低）							    
#define	LCD_CS_CLR  GPIOA->BRR=1<<LCD_CS     //片选端口  	
#define	LCD_RS_CLR	GPIOA->BRR=1<<LCD_RS     //数据/命令  	 
#define	LCD_RST_CLR	GPIOC->BRR=1<<LCD_RST    //复位			  
#define	LCD_WR_CLR	GPIOC->BRR=1<<LCD_WR     //写
#define LCD_RD_CLR  GPIOC->BRR=1<<LCD_RD     //读	  		  
#define LCD_LED_CLR  GPIOA->BRR=1<<LED      //读		  

#define LCD_PORT_GPIO_Pins               ((uint16_t)0xFFFF)  /*!< High 8 pins selected */
//PB0~15,作为数据线
//注意：如果使用8位模式数据总线，则液晶屏的数据高8位是接到MCU的高8位总线上
//举例：如果接8位模式则本示例接线为液晶屏DB10-DB17对应接至单片机GPIOB_Pin8-GPIOB_Pin15
//举例：如果是16位模式：DB0-DB7分别接GPIOB_Pin0-GPIOB_Pin7,DB10-DB17对应接至单片机GPIOB_Pin8-GPIOB_Pin15
//Note:DB4<->A3 DB3<->A2 for HW problem
//NOTE:the HW problem above is not problem, it's caused by error config
#define DATAOUT(x) (GPIOB->ODR = x)
#define DATAIN()     (GPIOB->IDR)
/*
static inline uint16_t DATAIN()
{
	uint16_t ret = GPIOB->IDR;
	//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)?(ret|=0x10):(ret&=(~0x10));
	//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)?(ret|=0x08):(ret&=(~0x08));
	return ret;
}
*/

//////////////////////////////////////////////////////////////////////////////////	  
//定义LCD的尺寸
#define LCD_W 480
#define LCD_H 800
#define LARGE_SCREEN 1


//touch screen
//与触摸屏芯片连接引脚	   
#define PEN_GG GPIOC
#define DOUT_GG GPIOA
#define TDIN_GG GPIOA
#define TCLK_GG GPIOA
#define TCS_GG GPIOC

#define PEN_PIN GPIO_Pin_3
#define DOUT_PIN GPIO_Pin_6
#define TDIN_PIN GPIO_Pin_7
#define TCLK_PIN GPIO_Pin_5
#define TCS_PIN GPIO_Pin_2

#define RCC_APB2Periph_GG_PEN RCC_APB2Periph_GPIOC
#define RCC_APB2Periph_GG_TCS RCC_APB2Periph_GPIOC
#define RCC_APB2Periph_GG_TCLK RCC_APB2Periph_GPIOA
#define RCC_APB2Periph_GG_DOUT RCC_APB2Periph_GPIOA
#define RCC_APB2Periph_GG_TDIN RCC_APB2Periph_GPIOA
//touch screen use spi common with sd/flash
#define TS_SPI_USE_COMMON 1

#define TOUCH_CALI_xFAC 0xC252BF
#define TOUCH_CALI_yFAC 0xFEC8C368
#define TOUCH_CALI_xOFF 0xFFEB
#define TOUCH_CALI_yOFF 0x32B

//spi flash
#define SF_CS_PIN                 GPIO_Pin_4
#define SF_CS_GPIO_PORT           GPIOC
#define SF_CS_GPIO_CLK            RCC_APB2Periph_GPIOC

#define SD_CS_PIN                 GPIO_Pin_1
#define SD_CS_GPIO_PORT           GPIOC
#define SD_CS_GPIO_CLK            RCC_APB2Periph_GPIOC

#define	SPI_FLASH_CS PCout(4)  //choose spi flash



#else//default
/////////////////////////////////////HAMMER//////////////////////////////////////////////


#define POWER_MONITOR
#define GPIO_ADC_VREF_PIN GPIO_Pin_3
#define GPIO_ADC_V4_2_PIN GPIO_Pin_4
#define GPIO_ADC_IBAT_PIN GPIO_Pin_2
#define ADC_Channel_VREF ADC_Channel_3
#define ADC_Channel_V4_2 ADC_Channel_4
#define ADC_Channel_IBAT ADC_Channel_2
#define GPIO_GROUP_VREF GPIOA
#define GPIO_GROUP_V4_2_IBAT GPIOA
#define CURRENT_MEASUREMENT_CALIBRATION 1025/1000
#define V4_2_RATIO (330+680)/330


#define BEEP_GPIO_PERIPH RCC_APB2Periph_GPIOB
#define BEEP_GPIO_GROUP GPIOB
#define BEEP_GPIO_PIN GPIO_Pin_5

#define LED1_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED1_GPIO_GROUP GPIOA
#define LED1_GPIO_PIN GPIO_Pin_14
#define LED0_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED0_GPIO_GROUP GPIOA
#define LED0_GPIO_PIN GPIO_Pin_15

#define POWEROFF_GPIO_PERIPH RCC_APB2Periph_GPIOB
#define POWEROFF_GPIO_GROUP GPIOB
#define POWEROFF_GPIO_PIN GPIO_Pin_0
//lcd
#define LCD_USE8BIT_MODEL   1	//定义数据总线是否使用8位模式 0,使用16位模式.1,使用8位模式

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define GPIO_TYPE  GPIOC  //GPIO组类型
#define LED      8        //背光控制引脚      PA8
#define LCD_CS   0       //片选引脚            PA0
#define LCD_RS   1       //寄存器/数据选择引脚 PA1 
#define LCD_RST  13       //复位引脚            PC13
#define LCD_WR   15       //写引脚              PC15
#define LCD_RD   14       //读引脚              PC14

#define RTC_8563
#define I2C_GPIO_PERIPH RCC_APB2Periph_GPIOB
#define I2C_GROUP GPIOB
#define SDA_GG GPIOB
#define SCL_GG GPIOB
#define SDA_PIN GPIO_Pin_7
#define SCL_PIN GPIO_Pin_6


//如果使用官方库函数定义下列底层，速度将会下降到14帧每秒，建议采用我司推荐方法
//以下IO定义直接操作寄存器，快速IO操作，刷屏速率可以达到28帧每秒！ 

//GPIO置位（拉高）
#define	LCD_CS_SET  GPIOA->BSRR=1<<LCD_CS    //片选端口  	
#define	LCD_RS_SET	GPIOA->BSRR=1<<LCD_RS    //数据/命令    
#define	LCD_RST_SET	GPIOC->BSRR=1<<LCD_RST   //复位			  
#define	LCD_WR_SET	GPIOC->BSRR=1<<LCD_WR    //写 	
#define LCD_RD_SET  GPIOC->BSRR=1<<LCD_RD    //读		  
#define LCD_LED_SET  GPIOA->BSRR=1<<LED      //读		  

//GPIO复位（拉低）							    
#define	LCD_CS_CLR  GPIOA->BRR=1<<LCD_CS     //片选端口  	
#define	LCD_RS_CLR	GPIOA->BRR=1<<LCD_RS     //数据/命令  	 
#define	LCD_RST_CLR	GPIOC->BRR=1<<LCD_RST    //复位			  
#define	LCD_WR_CLR	GPIOC->BRR=1<<LCD_WR     //写
#define LCD_RD_CLR  GPIOC->BRR=1<<LCD_RD     //读	  		  
#define LCD_LED_CLR  GPIOA->BRR=1<<LED      //读		  

#define LCD_PORT_GPIO_Pins               ((uint16_t)0xFF00)  /*!< High 8 pins selected */
//PB0~15,作为数据线
//注意：如果使用8位模式数据总线，则液晶屏的数据高8位是接到MCU的高8位总线上
//举例：如果接8位模式则本示例接线为液晶屏DB10-DB17对应接至单片机GPIOB_Pin8-GPIOB_Pin15
//举例：如果是16位模式：DB0-DB7分别接GPIOB_Pin0-GPIOB_Pin7,DB10-DB17对应接至单片机GPIOB_Pin8-GPIOB_Pin15
//Note:DB4<->A3 DB3<->A2 for HW problem
//NOTE:the HW problem above is not problem, it's caused by error config
#define DATAOUT(x) {GPIOB->BRR = 0xff00; GPIOB->BSRR = (0xff00&x);}
#define DATAIN()     (GPIOB->IDR)
/*
static inline uint16_t DATAIN()
{
	uint16_t ret = GPIOB->IDR;
	//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)?(ret|=0x10):(ret&=(~0x10));
	//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)?(ret|=0x08):(ret&=(~0x08));
	return ret;
}
*/

//////////////////////////////////////////////////////////////////////////////////	  
//定义LCD的尺寸
#define LCD_W 480
#define LCD_H 800
#define LARGE_SCREEN 1


//touch screen
//与触摸屏芯片连接引脚	   
#define PEN_GG GPIOB
#define DOUT_GG GPIOA
#define TDIN_GG GPIOA
#define TCLK_GG GPIOA
#define TCS_GG GPIOB

#define PEN_PIN GPIO_Pin_3
#define DOUT_PIN GPIO_Pin_6
#define TDIN_PIN GPIO_Pin_7
#define TCLK_PIN GPIO_Pin_5
#define TCS_PIN GPIO_Pin_2

#define RCC_APB2Periph_GG_PEN RCC_APB2Periph_GPIOB
#define RCC_APB2Periph_GG_TCS RCC_APB2Periph_GPIOB
#define RCC_APB2Periph_GG_TCLK RCC_APB2Periph_GPIOA
#define RCC_APB2Periph_GG_DOUT RCC_APB2Periph_GPIOA
#define RCC_APB2Periph_GG_TDIN RCC_APB2Periph_GPIOA
//touch screen use spi common with sd/flash
#define TS_SPI_USE_COMMON 1

#define TOUCH_CALI_xFAC 0xC252BF
#define TOUCH_CALI_yFAC 0xFEC8C368
#define TOUCH_CALI_xOFF 0xFFEB
#define TOUCH_CALI_yOFF 0x32B

//spi flash
#define SF_CS_PIN                 GPIO_Pin_4
#define SF_CS_GPIO_PORT           GPIOB
#define SF_CS_GPIO_CLK            RCC_APB2Periph_GPIOB

#define SD_CS_PIN                 GPIO_Pin_1
#define SD_CS_GPIO_PORT           GPIOB
#define SD_CS_GPIO_CLK            RCC_APB2Periph_GPIOB

#define	SPI_FLASH_CS PBout(4)  //选中FLASH					 



#endif





//////////////////////////////common//////////////////////////////////
//touch
#define PEN  GPIO_ReadInputDataBit(PEN_GG, PEN_PIN)    //PA1  INT
#define TCS(x)  GPIO_WriteBit(TCS_GG, TCS_PIN, x)   //PA0 CS

//gpio spi
#define DOUT GPIO_ReadInputDataBit(DOUT_GG, DOUT_PIN)     //PA6  MISO
#define TDIN(x) GPIO_WriteBit(TDIN_GG, TDIN_PIN, x)   //PA7  MOSI
#define TCLK(x) GPIO_WriteBit(TCLK_GG, TCLK_PIN, x)    //PA5  SCLK


#endif
