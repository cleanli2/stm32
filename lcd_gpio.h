#ifndef __GPIO_H
#define __GPIO_H

/*
	HW connect:
	CS-------PA4------
	CD-------PB0------
	RST------PB1------
	SCK------PB10-----
	SDA------PB11-----
*/

#define Port_Clock   (RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB)

#define CS						GPIO_Pin_4
#define CD						GPIO_Pin_0					
#define RST						GPIO_Pin_1						
#define SCK 					GPIO_Pin_10						
#define SDA 					GPIO_Pin_11						

#define CS_1()  			GPIO_SetBits(GPIOA,CS)				
#define CS_0()  			GPIO_ResetBits(GPIOA,CS)			

#define CD_1()  			GPIO_SetBits(GPIOB,CD)				
#define CD_0()  			GPIO_ResetBits(GPIOB,CD)			

#define RST_1()  			GPIO_SetBits(GPIOB,RST)				
#define RST_0()  			GPIO_ResetBits(GPIOB,RST)			

#define SCK_1()  			GPIO_SetBits(GPIOB,SCK)				
#define SCK_0()  			GPIO_ResetBits(GPIOB,SCK)			

#define SDA_1()				GPIO_SetBits(GPIOB,SDA)				
#define SDA_0()				GPIO_ResetBits(GPIOB,SDA)			


void Gpio_Init(void);											

#endif

