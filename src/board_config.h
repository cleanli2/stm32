#ifndef _BOARD_CONDIG_H
#define _BOARD_CONDIG_H

#include"stdint.h"
#include "sys.h"
#include "stdlib.h"








#ifdef ALIENTEK_MINI
/////////////////////////////////////ALIENTEK_MINI//////////////////////////////////////////////
#define BEEP_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define BEEP_GPIO_GROUP GPIOA
#define BEEP_GPIO_PIN GPIO_Pin_14

#define LED1_GPIO_PERIPH RCC_APB2Periph_GPIOD
#define LED1_GPIO_GROUP GPIOD
#define LED1_GPIO_PIN GPIO_Pin_2
#define LED0_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED0_GPIO_GROUP GPIOA
#define LED0_GPIO_PIN GPIO_Pin_8

//lcd
//#define LCD_USE8BIT_MODEL   0	//定义数据总线是否使用8位模式 0,使用16位模式.1,使用8位模式

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






#else//default
/////////////////////////////////////HAMMER//////////////////////////////////////////////









#define BEEP_GPIO_PERIPH RCC_APB2Periph_GPIOB
#define BEEP_GPIO_GROUP GPIOB
#define BEEP_GPIO_PIN GPIO_Pin_5

#define LED1_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED1_GPIO_GROUP GPIOA
#define LED1_GPIO_PIN GPIO_Pin_14
#define LED0_GPIO_PERIPH RCC_APB2Periph_GPIOA
#define LED0_GPIO_GROUP GPIOA
#define LED0_GPIO_PIN GPIO_Pin_15

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
#define DATAOUT(x) {GPIOB->ODR=((x)&0xff00)|(GPIOB->ODR&0xff);}
//#define DATAIN     GPIOB->IDR; //数据输入
static inline uint16_t DATAIN()
{
	uint16_t ret = GPIOB->IDR;
	//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)?(ret|=0x10):(ret&=(~0x10));
	//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)?(ret|=0x08):(ret&=(~0x08));
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////	  
//定义LCD的尺寸
#define LCD_W 480
#define LCD_H 800


#endif




#endif
