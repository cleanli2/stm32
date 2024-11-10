//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103RCT6,正点原子MiniSTM32开发板,主频72MHZ，晶振12MHZ
//QDtech-TFT液晶驱动 for STM32 IO模拟
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtft.com
//淘宝网站：http://qdtech.taobao.com
//wiki技术网站：http://www.lcdwiki.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//技术支持QQ:3002773612  3002778157
//技术交流QQ群:324828016
//创建日期:2018/08/09
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================电源接线================================================//
//     LCD模块                STM32单片机
//      VDD          接        DC5V/3.3V        //电源
//      GND          接          GND            //电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为16位并口总线
//     LCD模块                STM32单片机    
//     DB0~DB7       接        PB0~PB7         //液晶屏低8位并口数据信号
//     DB8~DB15      接        PB8~PB15        //液晶屏高8位并口数据信号
//=======================================液晶屏控制线接线==========================================//
//     LCD模块 				        STM32单片机 
//       WR          接          PC7           //液晶屏写数据控制信号
//       RD          接          PC6           //液晶屏读数据控制信号
//       RS          接          PC8           //液晶屏数据/命令控制信号
//       RST         接          PC4           //液晶屏复位控制信号
//       CS          接          PC9           //液晶屏片选控制信号
//       BL          接          PC10          //液晶屏背光控制信号
//=========================================触摸屏触接线=========================================//
//如果模块不带触摸功能或者带有触摸功能，但是不需要触摸功能，则不需要进行触摸屏接线
//	   LCD模块                STM32单片机 
//       PEN         接          PC1           //触摸屏触摸中断信号
//       MISO        接          PC2           //触摸屏SPI总线读信号
//       MOSI        接          PC3           //触摸屏SPI总线写信号
//       T_CS        接          PC13          //触摸屏片选控制信号
//       CLK         接          PC0           //触摸屏SPI总线时钟信号
**************************************************************************************************/	
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/		
#include "lcd_sueb.h"
#include "stdlib.h"
#include "common.h"	 
#include "test.h"	 
#include "display.h"
#include "fs.h"

	   
typedef uint8_t uchar;
#ifdef ALIENTEK_MINI
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
#endif
void cam_xclk_off();
void cam_xclk_on();
void BL_PWM_init();
void BL_PWM_deinit();

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

void Enable_BL(int en)//点亮背光	 
{
	if(en){
	    LCD_LED_SET;     
	    //GPIOC->BSRR = 0x00002000;
	}
	else{
	    LCD_LED_CLR;     
	    //GPIOC->BRR = 0x00002000;
	}
}
#define CAM_DATA_OFFSET 1
#define CAM_GPIO_GROUP GPIOB
#define CAM_PWN_GPIO_GROUP GPIOC
#define VSNC GPIO_Pin_11
#define RRST GPIO_Pin_12
#define CAM_PWN GPIO_Pin_13
#define CAM_RST GPIO_Pin_0
#define RCK GPIO_Pin_10
//#define RE GPIO_Pin_11
#define OE GPIO_Pin_9
#define HREF GPIO_Pin_15
#define AL422_WG GPIOA
#define WE GPIO_Pin_1
#define WRST GPIO_Pin_0

void i2c_init();
uint8_t cam_r_reg(uint8_t addr);
int cam_w_reg(uint8_t addr, uint8_t data);
#define sensor_write_reg cam_w_reg
#define udelay delay_us
#if 1
void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height)
{
	unsigned int endx;
	unsigned int endy;// "v*2"必须
	unsigned char temp_reg1, temp_reg2;
	unsigned char temp=0;
	
	endx=(startx+width);
	endy=(starty+height+height);// "v*2"必须
    //rdOV7670Reg(0x03, &temp_reg1 );
    temp_reg1=cam_r_reg(0x03);
    temp_reg1 &= 0xf0;
    //rdOV7670Reg(0x32, &temp_reg2 );
    temp_reg2=cam_r_reg(0x32);
    temp_reg2 &= 0xc0;
	
	// Horizontal
	temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
	cam_w_reg(0x32, temp );
	temp = (startx&0x7F8)>>3;
	cam_w_reg(0x17, temp );
	temp = (endx&0x7F8)>>3;
	cam_w_reg(0x18, temp );
	
	// Vertical
	temp =temp_reg1|((endy&0x3)<<2)|(starty&0x3);
	cam_w_reg(0x03, temp );
	temp = starty>>2;
	cam_w_reg(0x19, temp );
	temp = endy>>2;
	cam_w_reg(0x1A, temp );
}

static void init_rgb565_qvga_25fps_new(void)
{
	sensor_write_reg(0x11, 0x80);
	sensor_write_reg(0x92, 0x7f);
	sensor_write_reg(0x93, 0x00);
	sensor_write_reg(0x9d, 0x9e);
	sensor_write_reg(0x3b, 0x0a);
	sensor_write_reg(0x13, 0xf2);
	sensor_write_reg(0x10, 0x00);
	sensor_write_reg(0x00, 0x00);
	sensor_write_reg(0x01, 0x80);
	sensor_write_reg(0x02, 0x80);
	sensor_write_reg(0x13, 0xd7);
	//
	sensor_write_reg(0x12, 0x14);
	sensor_write_reg(0x04, 0x00);
	sensor_write_reg(0x18, 0x4b);
	sensor_write_reg(0x17, 0x23);
	sensor_write_reg(0x32, 0xbf);
	sensor_write_reg(0x19, 0x02);
	sensor_write_reg(0x1a, 0x7a);
	sensor_write_reg(0x03, 0x00);
	//
	sensor_write_reg(0x0e, 0x84);
	sensor_write_reg(0x0f, 0x62);
	sensor_write_reg(0x15, 0x02);
	sensor_write_reg(0x16, 0x02);
	sensor_write_reg(0x1b, 0x01);
	sensor_write_reg(0x1e, 0x01);
	sensor_write_reg(0x29, 0x3c);
	sensor_write_reg(0x33, 0x00);
	sensor_write_reg(0x34, 0x07);
	sensor_write_reg(0x35, 0x84);
	sensor_write_reg(0x36, 0x00);
	sensor_write_reg(0x38, 0x13);
	sensor_write_reg(0x39, 0x43);
	sensor_write_reg(0x3a, 0x00);
	sensor_write_reg(0x3c, 0x6c);
	sensor_write_reg(0x3d, 0x90);
	sensor_write_reg(0x3f, 0x29);
	sensor_write_reg(0x40, 0xd1);
	sensor_write_reg(0x41, 0x20);
	sensor_write_reg(0x6b, 0x0a);
	sensor_write_reg(0xa1, 0xc8);
	//
	sensor_write_reg(0x69, 0x80);
	sensor_write_reg(0x43, 0xf0);
	sensor_write_reg(0x44, 0x10);
	sensor_write_reg(0x45, 0x78);
	sensor_write_reg(0x46, 0xa8);
	sensor_write_reg(0x47, 0x60);
	sensor_write_reg(0x48, 0x80);
	sensor_write_reg(0x59, 0xba);
	sensor_write_reg(0x5a, 0x9a);
	sensor_write_reg(0x5b, 0x22);
	sensor_write_reg(0x5c, 0xb9);
	sensor_write_reg(0x5d, 0x9b);
	sensor_write_reg(0x5e, 0x10);
	sensor_write_reg(0x5f, 0xe0);
	sensor_write_reg(0x60, 0x85);
	sensor_write_reg(0x61, 0x60);
	sensor_write_reg(0x9f, 0x9d);
	sensor_write_reg(0xa0, 0xa0);
	//
	sensor_write_reg(0x4f, 0xae);
	sensor_write_reg(0x50, 0x26);
	sensor_write_reg(0x51, 0x08);
	sensor_write_reg(0x52, 0x1a);
	sensor_write_reg(0x53, 0xa9);
	sensor_write_reg(0x54, 0x0f);
	sensor_write_reg(0x55, 0x05);
	sensor_write_reg(0x56, 0x46);
	sensor_write_reg(0x57, 0xcb);
	sensor_write_reg(0x58, 0x77);
	//
	sensor_write_reg(0x8b, 0xcc);
	sensor_write_reg(0x8c, 0xcc);
	sensor_write_reg(0x8d, 0xcf);
	//
	sensor_write_reg(0x6c, 0x40);
	sensor_write_reg(0x6d, 0x30);
	sensor_write_reg(0x6e, 0x4b);
	sensor_write_reg(0x6f, 0x60);
	sensor_write_reg(0x70, 0x70);
	sensor_write_reg(0x71, 0x70);
	sensor_write_reg(0x72, 0x70);
	sensor_write_reg(0x73, 0x70);
	sensor_write_reg(0x74, 0x60);
	sensor_write_reg(0x75, 0x60);
	sensor_write_reg(0x76, 0x50);
	sensor_write_reg(0x77, 0x48);
	sensor_write_reg(0x78, 0x3a);
	sensor_write_reg(0x79, 0x2e);
	sensor_write_reg(0x7a, 0x28);
	sensor_write_reg(0x7b, 0x22);
	sensor_write_reg(0x7c, 0x04);
	sensor_write_reg(0x7d, 0x07);
	sensor_write_reg(0x7e, 0x10);
	sensor_write_reg(0x7f, 0x28);
	sensor_write_reg(0x80, 0x36);
	sensor_write_reg(0x81, 0x44);
	sensor_write_reg(0x82, 0x52);
	sensor_write_reg(0x83, 0x60);
	sensor_write_reg(0x84, 0x6c);
	sensor_write_reg(0x85, 0x78);
	sensor_write_reg(0x86, 0x8c);
	sensor_write_reg(0x87, 0x9e);
	sensor_write_reg(0x88, 0xbb);
	sensor_write_reg(0x89, 0xd2);
	sensor_write_reg(0x8a, 0xe6);
	//
	sensor_write_reg(0x14, 0x2e);
	sensor_write_reg(0x24, 0xb8);
	sensor_write_reg(0x25, 0xa8);
}

static void init_rgb565_qvga_25fps(void)
{
	sensor_write_reg(0x12, 0x80);
	udelay(5000);
	sensor_write_reg(0x11, 0x80);
	sensor_write_reg(0x92, 0x7f);
	sensor_write_reg(0x93, 0x00);
	sensor_write_reg(0x9d, 0x9e);
	sensor_write_reg(0x3b, 0x0a);
	sensor_write_reg(0x13, 0xf2);
	sensor_write_reg(0x10, 0x00);
	sensor_write_reg(0x00, 0x00);
	sensor_write_reg(0x01, 0x80);
	sensor_write_reg(0x02, 0x80);
	sensor_write_reg(0x13, 0xf7);
	//
	sensor_write_reg(0x12, 0x14);
	sensor_write_reg(0x04, 0x00);
	sensor_write_reg(0x18, 0x4b);
	sensor_write_reg(0x17, 0x23);
	sensor_write_reg(0x32, 0xbf);
	sensor_write_reg(0x19, 0x02);
	sensor_write_reg(0x1a, 0x7a);
	sensor_write_reg(0x03, 0x00);
	//
	sensor_write_reg(0x0e, 0x84);
	sensor_write_reg(0x0f, 0x62);
	sensor_write_reg(0x15, 0x02);
	sensor_write_reg(0x16, 0x02);
	sensor_write_reg(0x1b, 0x01);
	sensor_write_reg(0x1e, 0x01);
	sensor_write_reg(0x29, 0x3c);
	sensor_write_reg(0x33, 0x00);
	sensor_write_reg(0x34, 0x07);
	sensor_write_reg(0x35, 0x84);
	sensor_write_reg(0x36, 0x00);
	sensor_write_reg(0x38, 0x13);
	sensor_write_reg(0x39, 0x43);
	sensor_write_reg(0x3a, 0x00);
	sensor_write_reg(0x3c, 0x6c);
	sensor_write_reg(0x3d, 0x90);
	sensor_write_reg(0x3f, 0x29);
	sensor_write_reg(0x40, 0xd1);
	sensor_write_reg(0x41, 0x20);
	sensor_write_reg(0x6b, 0x0a);
	sensor_write_reg(0xa1, 0xc8);
	//
	sensor_write_reg(0x69, 0x80);
	sensor_write_reg(0x43, 0xf0);
	sensor_write_reg(0x44, 0x10);
	sensor_write_reg(0x45, 0x78);
	sensor_write_reg(0x46, 0xa8);
	sensor_write_reg(0x47, 0x60);
	sensor_write_reg(0x48, 0x80);
	sensor_write_reg(0x59, 0xba);
	sensor_write_reg(0x5a, 0x9a);
	sensor_write_reg(0x5b, 0x22);
	sensor_write_reg(0x5c, 0xb9);
	sensor_write_reg(0x5d, 0x9b);
	sensor_write_reg(0x5e, 0x10);
	sensor_write_reg(0x5f, 0xe0);
	sensor_write_reg(0x60, 0x85);
	sensor_write_reg(0x61, 0x60);
	sensor_write_reg(0x9f, 0x9d);
	sensor_write_reg(0xa0, 0xa0);
	//
	sensor_write_reg(0x4f, 0xae);
	sensor_write_reg(0x50, 0x26);
	sensor_write_reg(0x51, 0x08);
	sensor_write_reg(0x52, 0x1a);
	sensor_write_reg(0x53, 0xa9);
	sensor_write_reg(0x54, 0x0f);
	sensor_write_reg(0x55, 0x05);
	sensor_write_reg(0x56, 0x46);
	sensor_write_reg(0x57, 0xcb);
	sensor_write_reg(0x58, 0x77);
	//
	sensor_write_reg(0x8b, 0xcc);
	sensor_write_reg(0x8c, 0xcc);
	sensor_write_reg(0x8d, 0xcf);
	//
	sensor_write_reg(0x6c, 0x40);
	sensor_write_reg(0x6d, 0x30);
	sensor_write_reg(0x6e, 0x4b);
	sensor_write_reg(0x6f, 0x60);
	sensor_write_reg(0x70, 0x70);
	sensor_write_reg(0x71, 0x70);
	sensor_write_reg(0x72, 0x70);
	sensor_write_reg(0x73, 0x70);
	sensor_write_reg(0x74, 0x60);
	sensor_write_reg(0x75, 0x60);
	sensor_write_reg(0x76, 0x50);
	sensor_write_reg(0x77, 0x48);
	sensor_write_reg(0x78, 0x3a);
	sensor_write_reg(0x79, 0x2e);
	sensor_write_reg(0x7a, 0x28);
	sensor_write_reg(0x7b, 0x22);
	sensor_write_reg(0x7c, 0x04);
	sensor_write_reg(0x7d, 0x07);
	sensor_write_reg(0x7e, 0x10);
	sensor_write_reg(0x7f, 0x28);
	sensor_write_reg(0x80, 0x36);
	sensor_write_reg(0x81, 0x44);
	sensor_write_reg(0x82, 0x52);
	sensor_write_reg(0x83, 0x60);
	sensor_write_reg(0x84, 0x6c);
	sensor_write_reg(0x85, 0x78);
	sensor_write_reg(0x86, 0x8c);
	sensor_write_reg(0x87, 0x9e);
	sensor_write_reg(0x88, 0xbb);
	sensor_write_reg(0x89, 0xd2);
	sensor_write_reg(0x8a, 0xe6);
	//
	sensor_write_reg(0x14, 0x2e);
	sensor_write_reg(0x24, 0xb8);
	sensor_write_reg(0x25, 0xa8);
}

static void init_rgb565_qvga_12fps(void)
{
	sensor_write_reg(0x12, 0x80);
	udelay(5000);
	sensor_write_reg(0x11, 0x83);
	sensor_write_reg(0x9e, 0x29);
	sensor_write_reg(0x3b, 0x02);
	sensor_write_reg(0x13, 0xf2);
	sensor_write_reg(0x00, 0x00);
	sensor_write_reg(0x10, 0x00);
	sensor_write_reg(0x01, 0x80);
	sensor_write_reg(0x02, 0x80);
	sensor_write_reg(0x13, 0xf7);
	//
//	sensor_write_reg(0x12, 0x15);
	sensor_write_reg(0x12, 0x14); // modified by me
	sensor_write_reg(0x04, 0x00);
	sensor_write_reg(0x0c, 0x04);
	sensor_write_reg(0x18, 0x4A);
	sensor_write_reg(0x17, 0x22);
	sensor_write_reg(0x32, 0x89);
	sensor_write_reg(0x19, 0x02);
	sensor_write_reg(0x1a, 0x7a);
	sensor_write_reg(0x03, 0x00);
	//
	sensor_write_reg(0x0e, 0x84);
	sensor_write_reg(0x0f, 0x62);
	sensor_write_reg(0x15, 0x02);
	sensor_write_reg(0x1b, 0x01);
	sensor_write_reg(0x1e, 0x01);
	sensor_write_reg(0x29, 0x3c);
	sensor_write_reg(0x33, 0x00);
	sensor_write_reg(0x34, 0x07);
	sensor_write_reg(0x35, 0x84);
	sensor_write_reg(0x36, 0x00);
	sensor_write_reg(0x38, 0x13);
	sensor_write_reg(0x39, 0x43);
	sensor_write_reg(0x3c, 0x68);
	sensor_write_reg(0x3d, 0x19);
//	sensor_write_reg(0x40, 0xc1);
	sensor_write_reg(0x40, 0xd1); // modified by me
	sensor_write_reg(0x69, 0x80);
	sensor_write_reg(0x6b, 0x0a);
	sensor_write_reg(0xa1, 0x08);
	//
	sensor_write_reg(0x8b, 0xcc);
	sensor_write_reg(0x8c, 0xcc);
	sensor_write_reg(0x8d, 0xcf);
	//
	sensor_write_reg(0x14, 0x2e);
	sensor_write_reg(0x25, 0x58);
	sensor_write_reg(0x24, 0x68);
}

static void init_yuv_12fps(void)
{
	sensor_write_reg(0x11, 0x00);
	sensor_write_reg(0x92, 0x66);
	sensor_write_reg(0x93, 0x00);
	sensor_write_reg(0x9d, 0x4c);
	sensor_write_reg(0x9e, 0x3f);
	sensor_write_reg(0x3b, 0x0a);
	sensor_write_reg(0x13, 0xf0);
	sensor_write_reg(0x10, 0x00);
	sensor_write_reg(0x00, 0x00);
//	sensor_write_reg(0x00, 0xff); // modified by me
//	sensor_write_reg(0x07, 0x3f); // added by me
	sensor_write_reg(0x01, 0x80);
	sensor_write_reg(0x02, 0x80);
	sensor_write_reg(0x13, 0xf5);
	//
	sensor_write_reg(0x12, 0x00);
	sensor_write_reg(0x04, 0x00);
	sensor_write_reg(0x18, 0x01);
	sensor_write_reg(0x17, 0x13);
	sensor_write_reg(0x32, 0x92);
	sensor_write_reg(0x19, 0x02);
	sensor_write_reg(0x1a, 0x7a);
	sensor_write_reg(0x03, 0x00);
	//
	sensor_write_reg(0x0e, 0x84);
	sensor_write_reg(0x0f, 0x62);
	sensor_write_reg(0x15, 0x02);
	sensor_write_reg(0x16, 0x02);
	sensor_write_reg(0x1b, 0x01);
	sensor_write_reg(0x1e, 0x01);
	sensor_write_reg(0x29, 0x3c);
	sensor_write_reg(0x33, 0x00);
	sensor_write_reg(0x34, 0x07);
	sensor_write_reg(0x35, 0x84);
	sensor_write_reg(0x36, 0x00);
	sensor_write_reg(0x38, 0x13);
	sensor_write_reg(0x39, 0x43);
	sensor_write_reg(0x3a, 0x00);
	sensor_write_reg(0x3c, 0x6c);
	sensor_write_reg(0x3d, 0x90);
	sensor_write_reg(0x3f, 0x29);
	sensor_write_reg(0x40, 0xc1);
	sensor_write_reg(0x41, 0x20);
	sensor_write_reg(0x6b, 0x0a);
	sensor_write_reg(0xa1, 0xc8);
	//
	sensor_write_reg(0x69, 0x80);
	sensor_write_reg(0x43, 0xf0);
	sensor_write_reg(0x44, 0x10);
	sensor_write_reg(0x45, 0x78);
	sensor_write_reg(0x46, 0xa8);
	sensor_write_reg(0x47, 0x60);
	sensor_write_reg(0x48, 0x80);
	sensor_write_reg(0x59, 0xba);
	sensor_write_reg(0x5a, 0x9a);
	sensor_write_reg(0x5b, 0x22);
	sensor_write_reg(0x5c, 0xb9);
	sensor_write_reg(0x5d, 0x9b);
	sensor_write_reg(0x5e, 0x10);
	sensor_write_reg(0x5f, 0xe0);
	sensor_write_reg(0x60, 0x85);
	sensor_write_reg(0x61, 0x60);
	sensor_write_reg(0x9f, 0x9d);
	sensor_write_reg(0xa0, 0xa0);
	//
	sensor_write_reg(0x4f, 0x66);
	sensor_write_reg(0x50, 0x6b);
	sensor_write_reg(0x51, 0x05);
	sensor_write_reg(0x52, 0x19);
	sensor_write_reg(0x53, 0x40);
	sensor_write_reg(0x54, 0x59);
	sensor_write_reg(0x55, 0x40);
	sensor_write_reg(0x56, 0x40);
	sensor_write_reg(0x57, 0x40);
	sensor_write_reg(0x58, 0x0d);
	//
	sensor_write_reg(0x8b, 0xcc);
	sensor_write_reg(0x8c, 0xcc);
	sensor_write_reg(0x8d, 0xcf);
	//
	sensor_write_reg(0x6c, 0x40);
	sensor_write_reg(0x6d, 0x30);
	sensor_write_reg(0x6e, 0x4b);
	sensor_write_reg(0x6f, 0x60);
	sensor_write_reg(0x70, 0x70);
	sensor_write_reg(0x71, 0x70);
	sensor_write_reg(0x72, 0x70);
	sensor_write_reg(0x73, 0x70);
	sensor_write_reg(0x74, 0x60);
	sensor_write_reg(0x75, 0x60);
	sensor_write_reg(0x76, 0x50);
	sensor_write_reg(0x77, 0x48);
	sensor_write_reg(0x78, 0x3a);
	sensor_write_reg(0x79, 0x2e);
	sensor_write_reg(0x7a, 0x28);
	sensor_write_reg(0x7b, 0x22);
	sensor_write_reg(0x7c, 0x04);
	sensor_write_reg(0x7d, 0x07);
	sensor_write_reg(0x7e, 0x10);
	sensor_write_reg(0x7f, 0x28);
	sensor_write_reg(0x80, 0x36);
	sensor_write_reg(0x81, 0x44);
	sensor_write_reg(0x82, 0x52);
	sensor_write_reg(0x83, 0x60);
	sensor_write_reg(0x84, 0x6c);
	sensor_write_reg(0x85, 0x78);
	sensor_write_reg(0x86, 0x8c);
	sensor_write_reg(0x87, 0x9e);
	sensor_write_reg(0x88, 0xbb);
	sensor_write_reg(0x89, 0xd2);
	sensor_write_reg(0x8a, 0xe6);
	//
	sensor_write_reg(0x14, 0x1a);
	sensor_write_reg(0x24, 0x78);
	sensor_write_reg(0x25, 0x6c);
	sensor_write_reg(0x62, 0x00);
	sensor_write_reg(0x63, 0x00);
	sensor_write_reg(0x64, 0x18);
	sensor_write_reg(0x65, 0x30);
	sensor_write_reg(0x66, 0x05);
	sensor_write_reg(0x94, 0x18);
	sensor_write_reg(0x95, 0x1c);
	udelay(5000);
	sensor_write_reg(0x13, 0xf7);
}

static void init_yuv_25fps(void)
{
	sensor_write_reg(0x12, 0x80);
	udelay(5000);
	sensor_write_reg(0x11, 0x80);
	sensor_write_reg(0x92, 0x66);
	sensor_write_reg(0x93, 0x00);
	sensor_write_reg(0x9d, 0x4c);
	sensor_write_reg(0x9e, 0x3f);
	sensor_write_reg(0x3b, 0x00);
	sensor_write_reg(0x13, 0xf0);
	sensor_write_reg(0x10, 0xff);
	sensor_write_reg(0x00, 0xff);
	sensor_write_reg(0x01, 0x80);
	sensor_write_reg(0x02, 0x80);
	sensor_write_reg(0x13, 0xf5);
	//
	sensor_write_reg(0x12, 0x00);
	sensor_write_reg(0x04, 0x00);
	sensor_write_reg(0x18, 0x61);
	sensor_write_reg(0x17, 0x11);
	sensor_write_reg(0x32, 0x80);
	sensor_write_reg(0x19, 0x03);
	sensor_write_reg(0x1a, 0x7b);
	sensor_write_reg(0x03, 0x00);
	//
	sensor_write_reg(0x0e, 0x84);
	sensor_write_reg(0x0f, 0x62);
	sensor_write_reg(0x15, 0x00);
	sensor_write_reg(0x16, 0x02);
	sensor_write_reg(0x1b, 0x01);
	sensor_write_reg(0x1e, 0x31);
	sensor_write_reg(0x29, 0x3c);
	sensor_write_reg(0x33, 0x00);
	sensor_write_reg(0x34, 0x07);
	sensor_write_reg(0x35, 0x84);
	sensor_write_reg(0x36, 0x00);
	sensor_write_reg(0x38, 0x13);
	sensor_write_reg(0x39, 0x43);
	sensor_write_reg(0x3a, 0x0d);
	sensor_write_reg(0x3c, 0x6c);
	sensor_write_reg(0x3d, 0x90);
	sensor_write_reg(0x3f, 0x29);
	sensor_write_reg(0x40, 0xc0);
	sensor_write_reg(0x41, 0x20);
	sensor_write_reg(0x6b, 0x0a);
	sensor_write_reg(0xa1, 0xc8);
	//
	sensor_write_reg(0x69, 0x80);
	sensor_write_reg(0x43, 0xf0);
	sensor_write_reg(0x44, 0x10);
	sensor_write_reg(0x45, 0x78);
	sensor_write_reg(0x46, 0xa8);
	sensor_write_reg(0x47, 0x60);
	sensor_write_reg(0x48, 0x80);
	sensor_write_reg(0x59, 0xba);
	sensor_write_reg(0x5a, 0x9a);
	sensor_write_reg(0x5b, 0x22);
	sensor_write_reg(0x5c, 0xb9);
	sensor_write_reg(0x5d, 0x9b);
	sensor_write_reg(0x5e, 0x10);
	sensor_write_reg(0x5f, 0xe0);
	sensor_write_reg(0x60, 0x85);
	sensor_write_reg(0x61, 0x60);
	sensor_write_reg(0x9f, 0x9d);
	sensor_write_reg(0xa0, 0xa0);
	//
	sensor_write_reg(0x4f, 0x66);
	sensor_write_reg(0x50, 0x6b);
	sensor_write_reg(0x51, 0x05);
	sensor_write_reg(0x52, 0x19);
	sensor_write_reg(0x53, 0x40);
	sensor_write_reg(0x54, 0x59);
	sensor_write_reg(0x55, 0x40);
	sensor_write_reg(0x56, 0x40);
	sensor_write_reg(0x57, 0x40);
	sensor_write_reg(0x58, 0x0d);
	//
	sensor_write_reg(0x8b, 0xcc);
	sensor_write_reg(0x8c, 0xcc);
	sensor_write_reg(0x8d, 0xcf);
	//
	sensor_write_reg(0x6c, 0x40);
	sensor_write_reg(0x6d, 0x30);
	sensor_write_reg(0x6e, 0x4b);
	sensor_write_reg(0x6f, 0x60);
	sensor_write_reg(0x70, 0x3a);
	sensor_write_reg(0x71, 0x35);
	sensor_write_reg(0x72, 0x11);
	sensor_write_reg(0x73, 0x00);
	sensor_write_reg(0x74, 0x60);
	sensor_write_reg(0x75, 0x60);
	sensor_write_reg(0x76, 0x50);
	sensor_write_reg(0x77, 0x48);
	sensor_write_reg(0x78, 0x3a);
	sensor_write_reg(0x79, 0x2e);
	sensor_write_reg(0x7a, 0x28);
	sensor_write_reg(0x7b, 0x22);
	sensor_write_reg(0x7c, 0x04);
	sensor_write_reg(0x7d, 0x07);
	sensor_write_reg(0x7e, 0x10);
	sensor_write_reg(0x7f, 0x28);
	sensor_write_reg(0x80, 0x36);
	sensor_write_reg(0x81, 0x44);
	sensor_write_reg(0x82, 0x52);
	sensor_write_reg(0x83, 0x60);
	sensor_write_reg(0x84, 0x6c);
	sensor_write_reg(0x85, 0x78);
	sensor_write_reg(0x86, 0x8c);
	sensor_write_reg(0x87, 0x9e);
	sensor_write_reg(0x88, 0xbb);
	sensor_write_reg(0x89, 0xd2);
	sensor_write_reg(0x8a, 0xe6);
	//
	sensor_write_reg(0x14, 0x1a);
	sensor_write_reg(0x24, 0x7b);
	sensor_write_reg(0x25, 0x6c);
	sensor_write_reg(0x26, 0xc4);
	//
	sensor_write_reg(0x62, 0x00);
	sensor_write_reg(0x63, 0x00);
	sensor_write_reg(0x64, 0x18);
	sensor_write_reg(0x65, 0x30);
	sensor_write_reg(0x66, 0x05);
	sensor_write_reg(0x94, 0x18);
	sensor_write_reg(0x95, 0x1c);
	udelay(4000);
	sensor_write_reg(0x13, 0xf7);
}

void set_OV7670reg_M(void)
{
	cam_w_reg(0x8c, 0x00);
	cam_w_reg(0x3a, 0x0c);//--------uyvy
	cam_w_reg(0x40, 0xc0);
	cam_w_reg(0x8c, 0x00);
	cam_w_reg(0x12, 0x00);//-----------------
	cam_w_reg(0x32, 0x80);
	cam_w_reg(0x17, 0x16);
	cam_w_reg(0x18, 0x04);
	cam_w_reg(0x19, 0x03);
	cam_w_reg(0x1a, 0x7b);
	cam_w_reg(0x03, 0x00);
	cam_w_reg(0x0c, 0x00);//----------------
	cam_w_reg(0x3e, 0x00);
	cam_w_reg(0x70, 0x3a);
	cam_w_reg(0x71, 0x35); 
	cam_w_reg(0x72, 0x11); 
	cam_w_reg(0x73, 0x00);
	cam_w_reg(0xa2, 0x00);
	cam_w_reg(0x11, 0x80);//-----------
	//cam_w_reg(0x15 , 0x31);
	cam_w_reg(0x7a, 0x20); 
	cam_w_reg(0x7b, 0x1c); 
	cam_w_reg(0x7c, 0x28); 
	cam_w_reg(0x7d, 0x3c);
	cam_w_reg(0x7e, 0x55); 
	cam_w_reg(0x7f, 0x68); 
	cam_w_reg(0x80, 0x76);
	cam_w_reg(0x81, 0x80); 
	cam_w_reg(0x82, 0x88);
	cam_w_reg(0x83, 0x8f);
	cam_w_reg(0x84, 0x96); 
	cam_w_reg(0x85, 0xa3);
	cam_w_reg(0x86, 0xaf);
	cam_w_reg(0x87, 0xc4); 
	cam_w_reg(0x88, 0xd7);
	cam_w_reg(0x89, 0xe8);
	 
	cam_w_reg(0x32,0xb6);
	
	cam_w_reg(0x13, 0xff); 
	cam_w_reg(0x00, 0x00);
	cam_w_reg(0x10, 0x00);
	cam_w_reg(0x0d, 0x00);
	cam_w_reg(0x14, 0x4e);
	cam_w_reg(0xa5, 0x05);
	cam_w_reg(0xab, 0x07); 
	cam_w_reg(0x24, 0x75);
	cam_w_reg(0x25, 0x63);
	cam_w_reg(0x26, 0xA5);
	cam_w_reg(0x9f, 0x78);
	cam_w_reg(0xa0, 0x68);
//	cam_w_reg(0xa1, 0x03);//0x0b,
	cam_w_reg(0xa6, 0xdf);
	cam_w_reg(0xa7, 0xdf);
	cam_w_reg(0xa8, 0xf0); 
	cam_w_reg(0xa9, 0x90); 
	cam_w_reg(0xaa, 0x94); 
	//cam_w_reg(0x13, 0xe5); 
	cam_w_reg(0x0e, 0x61);
	cam_w_reg(0x0f, 0x43);
	cam_w_reg(0x16, 0x02); 
	cam_w_reg(0x1e, 0x07);
	cam_w_reg(0x21, 0x02);
	cam_w_reg(0x22, 0x91);
	cam_w_reg(0x29, 0x07);
	cam_w_reg(0x33, 0x0b);
	cam_w_reg(0x35, 0x0b);
	cam_w_reg(0x37, 0x3f);
	cam_w_reg(0x38, 0x01);
	cam_w_reg(0x39, 0x00);
	cam_w_reg(0x3c, 0x78);
	cam_w_reg(0x4d, 0x40);
	cam_w_reg(0x4e, 0x20);
	cam_w_reg(0x69, 0x00);
	cam_w_reg(0x6b, 0x0a);//--------------------
	cam_w_reg(0x74, 0x19);
	cam_w_reg(0x8d, 0x4f);
	cam_w_reg(0x8e, 0x00);
	cam_w_reg(0x8f, 0x00);
	cam_w_reg(0x90, 0x00);
	cam_w_reg(0x91, 0x00);
	cam_w_reg(0x92, 0x00);
	cam_w_reg(0x96, 0x00);
	cam_w_reg(0x9a, 0x80);
	cam_w_reg(0xb0, 0x84);
	cam_w_reg(0xb1, 0x0c);
	cam_w_reg(0xb2, 0x0e);
	cam_w_reg(0xb3, 0x82);
	cam_w_reg(0xb8, 0x0a);
	cam_w_reg(0x43, 0x14);
	cam_w_reg(0x44, 0xf0);
	cam_w_reg(0x45, 0x34);
	cam_w_reg(0x46, 0x58);
	cam_w_reg(0x47, 0x28);
	cam_w_reg(0x48, 0x3a);
	
	cam_w_reg(0x59, 0x88);
	cam_w_reg(0x5a, 0x88);
	cam_w_reg(0x5b, 0x44);
	cam_w_reg(0x5c, 0x67);
	cam_w_reg(0x5d, 0x49);
	cam_w_reg(0x5e, 0x0e);
	
	cam_w_reg(0x64, 0x04);
	cam_w_reg(0x65, 0x20);
	cam_w_reg(0x66, 0x05);

	cam_w_reg(0x94, 0x04);
	cam_w_reg(0x95, 0x08);	 

	cam_w_reg(0x6c, 0x0a);
	cam_w_reg(0x6d, 0x55);
	cam_w_reg(0x6e, 0x11);
	cam_w_reg(0x6f, 0x9f); 

	cam_w_reg(0x6a, 0x40);
	cam_w_reg(0x01, 0x40);
	cam_w_reg(0x02, 0x40);
	
	//cam_w_reg(0x13, 0xe7);
	cam_w_reg(0x15, 0x00);
	cam_w_reg(0x4f, 0x80);
	cam_w_reg(0x50, 0x80);
	cam_w_reg(0x51, 0x00);
	cam_w_reg(0x52, 0x22);
	cam_w_reg(0x53, 0x5e);
	cam_w_reg(0x54, 0x80);
	cam_w_reg(0x58, 0x9e);

	cam_w_reg(0x41, 0x08);
	cam_w_reg(0x3f, 0x00);
	cam_w_reg(0x75, 0x05);
	cam_w_reg(0x76, 0xe1);

	cam_w_reg(0x4c, 0x00);
	cam_w_reg(0x77, 0x01);
	
	cam_w_reg(0x3d, 0xc0);//-------uyvy
	cam_w_reg(0x4b, 0x09);
	cam_w_reg(0xc9, 0x60);
	//cam_w_reg(0x41, 0x38);	
	cam_w_reg(0x56, 0x40);
	cam_w_reg(0x34, 0x11);
	cam_w_reg(0x3b, 0x02);
	cam_w_reg(0xa4, 0x89);
	
	cam_w_reg(0x96, 0x00);
	cam_w_reg(0x97, 0x30);
	cam_w_reg(0x98, 0x20);
	cam_w_reg(0x99, 0x30);
	cam_w_reg(0x9a, 0x84);
	cam_w_reg(0x9b, 0x29);
	cam_w_reg(0x9c, 0x03);
	cam_w_reg(0x9d, 0x4c);
	cam_w_reg(0x9e, 0x3f);	

	cam_w_reg(0x09, 0x00);
	//cam_w_reg(0x3b, 0xc2);//night mode

}
void set_OV7670reg(void)
{
	cam_w_reg(0x8c, 0x00);
	cam_w_reg(0x3a, 0x04);
	cam_w_reg(0x40, 0xd0);
	cam_w_reg(0x8c, 0x00);
	cam_w_reg(0x12, 0x14);
	cam_w_reg(0x32, 0x80);
	cam_w_reg(0x17, 0x16);
	cam_w_reg(0x18, 0x04);
	cam_w_reg(0x19, 0x02);
	cam_w_reg(0x1a, 0x7b);
	cam_w_reg(0x03, 0x06);
	cam_w_reg(0x0c, 0x04);
	cam_w_reg(0x3e, 0x00);
	cam_w_reg(0x70, 0x3a);
	cam_w_reg(0x71, 0x35); 
	cam_w_reg(0x72, 0x11); 
	cam_w_reg(0x73, 0x00);
	cam_w_reg(0xa2, 0x00);
	cam_w_reg(0x11, 0xff); 
	//cam_w_reg(0x15 , 0x31);
	cam_w_reg(0x7a, 0x20); 
	cam_w_reg(0x7b, 0x1c); 
	cam_w_reg(0x7c, 0x28); 
	cam_w_reg(0x7d, 0x3c);
	cam_w_reg(0x7e, 0x55); 
	cam_w_reg(0x7f, 0x68); 
	cam_w_reg(0x80, 0x76);
	cam_w_reg(0x81, 0x80); 
	cam_w_reg(0x82, 0x88);
	cam_w_reg(0x83, 0x8f);
	cam_w_reg(0x84, 0x96); 
	cam_w_reg(0x85, 0xa3);
	cam_w_reg(0x86, 0xaf);
	cam_w_reg(0x87, 0xc4); 
	cam_w_reg(0x88, 0xd7);
	cam_w_reg(0x89, 0xe8);
	 
	cam_w_reg(0x32,0xb6);
	
	cam_w_reg(0x13, 0xff); 
	cam_w_reg(0x00, 0x00);
	cam_w_reg(0x10, 0x00);
	cam_w_reg(0x0d, 0x00);
	cam_w_reg(0x14, 0x4e);
	cam_w_reg(0xa5, 0x05);
	cam_w_reg(0xab, 0x07); 
	cam_w_reg(0x24, 0x75);
	cam_w_reg(0x25, 0x63);
	cam_w_reg(0x26, 0xA5);
	cam_w_reg(0x9f, 0x78);
	cam_w_reg(0xa0, 0x68);
//	cam_w_reg(0xa1, 0x03);//0x0b,
	cam_w_reg(0xa6, 0xdf);
	cam_w_reg(0xa7, 0xdf);
	cam_w_reg(0xa8, 0xf0); 
	cam_w_reg(0xa9, 0x90); 
	cam_w_reg(0xaa, 0x94); 
	//cam_w_reg(0x13, 0xe5); 
	cam_w_reg(0x0e, 0x61);
	cam_w_reg(0x0f, 0x43);
	cam_w_reg(0x16, 0x02); 
	cam_w_reg(0x1e, 0x37);
	cam_w_reg(0x21, 0x02);
	cam_w_reg(0x22, 0x91);
	cam_w_reg(0x29, 0x07);
	cam_w_reg(0x33, 0x0b);
	cam_w_reg(0x35, 0x0b);
	cam_w_reg(0x37, 0x3f);
	cam_w_reg(0x38, 0x01);
	cam_w_reg(0x39, 0x00);
	cam_w_reg(0x3c, 0x78);
	cam_w_reg(0x4d, 0x40);
	cam_w_reg(0x4e, 0x20);
	cam_w_reg(0x69, 0x00);
	cam_w_reg(0x6b, 0x4a);
	cam_w_reg(0x74, 0x19);
	cam_w_reg(0x8d, 0x4f);
	cam_w_reg(0x8e, 0x00);
	cam_w_reg(0x8f, 0x00);
	cam_w_reg(0x90, 0x00);
	cam_w_reg(0x91, 0x00);
	cam_w_reg(0x92, 0x00);
	cam_w_reg(0x96, 0x00);
	cam_w_reg(0x9a, 0x80);
	cam_w_reg(0xb0, 0x84);
	cam_w_reg(0xb1, 0x0c);
	cam_w_reg(0xb2, 0x0e);
	cam_w_reg(0xb3, 0x82);
	cam_w_reg(0xb8, 0x0a);
	cam_w_reg(0x43, 0x14);
	cam_w_reg(0x44, 0xf0);
	cam_w_reg(0x45, 0x34);
	cam_w_reg(0x46, 0x58);
	cam_w_reg(0x47, 0x28);
	cam_w_reg(0x48, 0x3a);
	
	cam_w_reg(0x59, 0x88);
	cam_w_reg(0x5a, 0x88);
	cam_w_reg(0x5b, 0x44);
	cam_w_reg(0x5c, 0x67);
	cam_w_reg(0x5d, 0x49);
	cam_w_reg(0x5e, 0x0e);
	
	cam_w_reg(0x64, 0x04);
	cam_w_reg(0x65, 0x20);
	cam_w_reg(0x66, 0x05);

	cam_w_reg(0x94, 0x04);
	cam_w_reg(0x95, 0x08);	 

	cam_w_reg(0x6c, 0x0a);
	cam_w_reg(0x6d, 0x55);
	cam_w_reg(0x6e, 0x11);
	cam_w_reg(0x6f, 0x9f); 

	cam_w_reg(0x6a, 0x40);
	cam_w_reg(0x01, 0x40);
	cam_w_reg(0x02, 0x40);
	
	//cam_w_reg(0x13, 0xe7);
	cam_w_reg(0x15, 0x00);
	cam_w_reg(0x4f, 0x80);
	cam_w_reg(0x50, 0x80);
	cam_w_reg(0x51, 0x00);
	cam_w_reg(0x52, 0x22);
	cam_w_reg(0x53, 0x5e);
	cam_w_reg(0x54, 0x80);
	cam_w_reg(0x58, 0x9e);

	cam_w_reg(0x41, 0x08);
	cam_w_reg(0x3f, 0x00);
	cam_w_reg(0x75, 0x05);
	cam_w_reg(0x76, 0xe1);

	cam_w_reg(0x4c, 0x00);
	cam_w_reg(0x77, 0x01);
	
	cam_w_reg(0x3d, 0xc1);
	cam_w_reg(0x4b, 0x09);
	cam_w_reg(0xc9, 0x60);
	//cam_w_reg(0x41, 0x38);	
	cam_w_reg(0x56, 0x40);
	cam_w_reg(0x34, 0x11);
	cam_w_reg(0x3b, 0x02);
	cam_w_reg(0xa4, 0x89);
	
	cam_w_reg(0x96, 0x00);
	cam_w_reg(0x97, 0x30);
	cam_w_reg(0x98, 0x20);
	cam_w_reg(0x99, 0x30);
	cam_w_reg(0x9a, 0x84);
	cam_w_reg(0x9b, 0x29);
	cam_w_reg(0x9c, 0x03);
	cam_w_reg(0x9d, 0x4c);
	cam_w_reg(0x9e, 0x3f);	

	cam_w_reg(0x09, 0x00);
	cam_w_reg(0x3b, 0xc2);

}
#endif
char vbf[640*2];
char fbf[512];
u32 fbfs=0;
u16 ws[480];

int frames_wsize = 0;
u32 rn = 2;
u32 fn = 480;
int wtf(char*bf, u32 len, u32 ss)
{
    u32 llt = len;
    int wl;
    if(fbfs>0){//left last time
        memcpy(fbf+fbfs, bf, ss-fbfs);
        llt-=ss-fbfs;
        bf+=ss-fbfs;
        wl = write_sec_to_file((const char*)fbf);
        if(wl<0){
            lprintf("wtf-1\n");
            return -1;
        }
        frames_wsize = wl;
    }
    while(llt > ss){
        memcpy(fbf, bf, ss);
        llt-=ss;
        bf+=ss;
        wl = write_sec_to_file((const char*)fbf);
        if(wl<0){
            lprintf("wtf-2\n");
            return -1;
        }
        frames_wsize = wl;
    }
    fbfs = llt;
    if(fbfs>0){//left for next
        memcpy(fbf, bf, llt);
    }
    return fbfs;
}
void cam_set_rfn(u32 irn, u32 ifn)
{
    rn = irn;
    fn = ifn;
    lprintf("set:rn=%d, fn=%d\n", rn, fn);
}
#define READ_MODE 0
#define FREE_MODE 1
char tmstp[24];
void cam_read_line(int in_dump_line, u32 only_uart_dump)
{

    u32 linect = 0;
    u32 rec_count = 0;
    u32 linen = rn;
    memset(vbf, 0xff, 640*2);
    GPIO_SetBits(CAM_GPIO_GROUP,RCK);

    linect=in_dump_line;

    while(linen--){
        while(rec_count<640*2){
            vbf[rec_count]=CAM_GPIO_GROUP->IDR>>CAM_DATA_OFFSET;
            rec_count++;

            //rck=0
            GPIO_ResetBits(CAM_GPIO_GROUP,RCK);
            //rck=1
            GPIO_SetBits(CAM_GPIO_GROUP,RCK);
        }
        if(only_uart_dump){
            mem_print(vbf, 640*2*linect, 640*2);
        }
        else{
            yuv_line_buf_print_str(vbf, linect, 0, 0, tmstp);
            if(wtf(vbf, 640*2, 512)<0){
                lprintf_time("cam write to file error, linect %d\n", linect);
                return;
            }
            rec_count=0;
        }
        linect++;
    }
    lprintf(">");
    
}
void save_lines_to_al422(uint32_t line_ct_start, uint32_t line_num)
{
    while(!(CAM_GPIO_GROUP->IDR & VSNC));
    while((CAM_GPIO_GROUP->IDR & VSNC));
    while(line_ct_start--){
        while(!(CAM_GPIO_GROUP->IDR & HREF));
        while((CAM_GPIO_GROUP->IDR & HREF));
    }
    //al422 we = 1, start write
    GPIO_SetBits(AL422_WG,WE);
    GPIO_SetBits(AL422_WG,WRST);
    while(line_num--){
        while(!(CAM_GPIO_GROUP->IDR & HREF));
        while((CAM_GPIO_GROUP->IDR & HREF));
    }
    //al422 we = 0, stop write
    GPIO_ResetBits(AL422_WG,WE);
    //al422 wrst
    GPIO_ResetBits(AL422_WG,WRST);
}
extern int loop_stop;
int cam_save_lines(u32 ls, u32 le, u32 only_uart_dump)
{
    char ucbf[5]={0};
    uint32_t endct=0;
    int ucbfi=0;
    int w_start_line;
    int ret=0;
    frames_wsize = 0;
    slprintf(tmstp, "%s", get_rtc_time(NULL));
    //al422 we = 0, stop write
    GPIO_ResetBits(AL422_WG,WE);
    //save frame to al422
    save_lines_to_al422(ls, le-ls);


    //prepare read
    //al422 oe = 0
    GPIO_ResetBits(CAM_GPIO_GROUP,OE);
    //al422 rrst = 0
    GPIO_ResetBits(CAM_GPIO_GROUP,RRST);
    //rck=0
    GPIO_ResetBits(CAM_GPIO_GROUP,RCK);
    //rck=1
    GPIO_SetBits(CAM_GPIO_GROUP,RCK);
    //al422 rrst = 1
    GPIO_SetBits(CAM_GPIO_GROUP,RRST);

    for(w_start_line = ls; (u32)w_start_line <= le-rn; w_start_line+=rn){
        cam_read_line(w_start_line,only_uart_dump);
        if(get_sd_hw_err()){
            lprintf_time("SD hw error\n");
            goto quit;
        }
        if(con_is_recved()){
            ucbf[ucbfi++]=con_recv();
        }
        if(GPIOA->IDR & GPIO_Pin_13)
        {
            endct++;
        }
        else
        {
            endct=0;
        }
        if(ucbfi==4){
            if(!strcmp(ucbf, "quit")){
                lprintf_time("Get cmd:quit, abort!\n");
                loop_stop=1;
                ret=1;//abort
                goto quit;
            }
            else{
                lprintf("X:%s\n", ucbf);
                ucbfi=0;
            }
        }
        if(endct == 4){
            lprintf_time("Get end button, abort!\n");
            loop_stop=1;
            GPIO_ResetBits(LED1_GPIO_GROUP,LED1_GPIO_PIN);
            ret=1;//abort
            goto quit;
        }
    }
quit:
    //al422 oe = 1
    GPIO_SetBits(CAM_GPIO_GROUP,OE);
    return ret;

}
void cam_save_1_frame(u32 only_uart_dump)
{
    fbfs=0;
    if(cam_save_lines(0, 300, only_uart_dump))return;
    cam_save_lines(300, 480, only_uart_dump);
    memset(vbf, 0xff, 640*2);
    wtf(vbf, 640*2, 512);//write left in buffer
}
void cam_read_frame(int dump_line)
{
    u32 ct_bf_vsyn  = 0;
    u32 ct_bf_href  = 0;
    u32 last_href_lvl  = 0;
    u32 linect = 0;
    u32 count = 0;
    u32 rec_count = 0;
    u32 w_count = 0;
    //u32 frames_skip= 100;
    frames_wsize = 0;
    while(1){
        //while(CAM_GPIO_GROUP->IDR & RRST);
        while(!(CAM_GPIO_GROUP->IDR & RRST));
        while((CAM_GPIO_GROUP->IDR & RRST))ct_bf_vsyn++;
#if 0
        if(frames_skip--){
            prt_hex(frames_skip);
            continue;
        }
        else frames_skip=100;
#endif
        cam_xclk_off();
        while(!(CAM_GPIO_GROUP->IDR & RRST)){
        //while(CAM_GPIO_GROUP->IDR & RRST){
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);//XCLK = 0
            GPIO_SetBits(GPIOA, GPIO_Pin_8);//XCLK = 1
            if(CAM_GPIO_GROUP->IDR & VSNC){
                vbf[rec_count++]=CAM_GPIO_GROUP->IDR>>CAM_DATA_OFFSET;
                w_count++;
                count++;
                last_href_lvl  = 1;
            }
            else{
                if(count==0)ct_bf_href++;
                if(last_href_lvl){
                    if(dump_line>=0){
                        if((uint32_t)dump_line==linect){
                            mem_print(vbf, 640*2*linect, 640*2);
                        }
                    }
                    else{
                        if(wtf(vbf, 640*2, 512)<0){
                            lprintf("cam write to file error, linect %d\n", linect);
                            return;
                        }
                    }
                    ws[linect]=w_count;
                    w_count=0;
                    linect++;
                    rec_count = 0;
                }
                last_href_lvl  = 0;
            }
        }
        memset(vbf, 0xff, 512);
        mem_print((const char*)ws, 0, 480*2);
        lprintf("count %d linecount %d bfv %d bfh %d w %d left %d\n",
                count, linect, ct_bf_vsyn, ct_bf_href,
                count/linect, count%linect);
        lprintf("frame file size:%d(0x%x)\n", frames_wsize, frames_wsize);
        lprintf("================end of frame==========\r\n");
        count=0;
        cam_xclk_on();
        GPIOA->BRR = GPIO_Pin_8;
        ct_bf_vsyn  = 0;
        ct_bf_href=0;
        linect=0;
        return;
    }
}

/*****************camera i2c******************/
#define cam_SDA_PIN GPIO_Pin_13
#define cam_SCL_PIN GPIO_Pin_14
#define cam_I2C_GROUP GPIOB
#define cam_SCL_GROUP GPIOB
#define cam_SCL_GG GPIOB
#define cam_SDA_GROUP GPIOB
#define cam_SDA_GG GPIOB
#define cam_SDA_HIGH GPIO_WriteBit(cam_SDA_GG, cam_SDA_PIN, 1)
#define cam_SCL_HIGH GPIO_WriteBit(cam_SCL_GG, cam_SCL_PIN, 1)
#define cam_SDA_LOW GPIO_WriteBit(cam_SDA_GG, cam_SDA_PIN, 0)
#define cam_SCL_LOW GPIO_WriteBit(cam_SCL_GG, cam_SCL_PIN, 0)
#define cam_GET_SDA GPIO_ReadInputDataBit(cam_SDA_GG, cam_SDA_PIN)
void cam_SDA_set_input(uint8_t en)
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO

    GPIO_InitStructure.GPIO_Pin = cam_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        if(en){
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //上拉输入
            GPIO_Init(cam_SDA_GG, &GPIO_InitStructure);
        }
        else{
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出 
            GPIO_Init(cam_SDA_GG, &GPIO_InitStructure);
        }
    GPIO_SetBits(cam_SDA_GG, cam_SDA_PIN);	
}

/********************************************
  内部函数，延时1
 ********************************************/
void cam_Delay()
{
    delay_us(1);
}
/********************************************
  内部函数，I2C开始
 ********************************************/
void cam_Start()
{ 
    cam_SDA_HIGH;
    cam_SCL_HIGH;
    cam_Delay();
    cam_SDA_LOW;
    cam_Delay();
    cam_SCL_LOW;
}
/********************************************
  内部函数，I2C结束
 ********************************************/
void cam_Stop()
{
    cam_SDA_LOW;
    cam_SCL_LOW;
    cam_Delay();
    cam_SCL_HIGH;
    cam_Delay();
    cam_SDA_HIGH;
    cam_Delay();
}
/********************************************
  内部函数，输出ACK ,每个字节传输完成，输出ack=0,结束读书据，ack=1;
 ********************************************/
void cam_WriteACK(uchar ack)
{
    if(ack)cam_SDA_HIGH;
    else cam_SDA_LOW;
    cam_Delay();
    cam_SCL_HIGH;
    cam_Delay();
    cam_SCL_LOW;
}   
/********************************************
  内部函数，等待ACK
 ********************************************/
uint8_t cam_WaitACK()
{  
    uint8_t ret = 1;
    uchar errtime=20;
    cam_SDA_HIGH;
    cam_Delay(); /*读ACK*/
    cam_SDA_set_input(1);
    cam_SCL_HIGH;
    cam_Delay();
    while(cam_GET_SDA)
    {  
        errtime--;
        if(!errtime){
            lprintf("wait ack timeout\n");
            ret = 0;
            break;
        }
    }
    cam_SCL_LOW;
    cam_Delay();
    cam_SDA_set_input(0);
    return ret;
}
/********************************************
  内部函数.输出数据字节
  入口:B=数据
 ********************************************/
uint8_t cam_writebyte(uchar wdata)
{
    uchar i;
    //lprintf("i2c write=%x\n", wdata);
    for(i=0;i<8;i++)
    {
        if(wdata&0x80) 
            cam_SDA_HIGH;
        else 
            cam_SDA_LOW;
        wdata<<=1;
        cam_SCL_HIGH;
        cam_Delay();
        cam_SCL_LOW;
        cam_Delay();
    }
    return cam_WaitACK();     //I2C器件或通讯出错，将会退出I2C通讯
}
/********************************************
  内部函数.输入数据
  出口:B
 ********************************************/
uchar cam_Readbyte()
{
    uchar i,bytedata;
    cam_SDA_HIGH;
    cam_SDA_set_input(1);
    for(i=0;i<8;i++)
    {
        cam_SCL_HIGH; 
        cam_Delay();
        bytedata<<=1;
        bytedata|=cam_GET_SDA;
        cam_SCL_LOW;
        cam_Delay();
    }
    cam_SDA_set_input(0);
    //lprintf("i2c read=%x\n", bytedata);
    return(bytedata);
}
void cam_i2c_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = cam_SDA_PIN|cam_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(cam_I2C_GROUP,cam_SDA_PIN|cam_SCL_PIN);
}

int cam_w_reg(uint8_t addr, uint8_t data)
{
    int ret;
    cam_Start();
    ret=cam_writebyte(0x42); /*写命令*/
    if(ret==0)goto err;
    ret=cam_writebyte(addr); /*写地址*/
    if(ret==0)goto err;
    ret=cam_writebyte(data); /*写数据*/
    cam_Stop();
    return ret;
err:
    cam_Stop();
    lprintf("cam writeData addr %b data %b error\n", addr, data);
    return ret;
}
uint8_t cam_r_regn(uchar addr,uchar n,uchar * buf) /*多字节*/
{  
    uint8_t ret;
    uchar i;
    cam_Start();
    ret=cam_writebyte(0x42); /*写命令*/
    if(ret==0)goto err;
    ret=cam_writebyte(addr); /*写地址*/
    if(ret==0)goto err;
    cam_Stop();
    cam_Start();
    ret=cam_writebyte(0x43); /*读命令*/
    if(ret==0)goto err;
    for(i=0;i<n;i++)
    {
        buf[i]=cam_Readbyte();
        if(i<n-1) 
            cam_WriteACK(0);
    }
    cam_WriteACK(1);
err:
    cam_Stop();
    return ret;
}  
uint8_t cam_r_reg(uint8_t addr)
{
    uint8_t ret;
    cam_r_regn(addr,1,&ret);
    return ret;
}

#define CAM_DATA_PORT_GPIO_Pins 0x1fe
void cam_deinit()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    lprintf_time("cam down\n");
    GPIO_SetBits(CAM_GPIO_GROUP,CAM_PWN);
    cam_xclk_off();
    //END detect gpio pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_13);
}
void cam_al422(const char*ps, uint32_t p2)
{
    if(!strcmp(ps, "re")){
        if(p2){
        }
        else{
        }
    }
    if(!strcmp(ps, "oe")){
        if(p2){
            GPIO_SetBits(CAM_GPIO_GROUP,OE);
        }
        else{
            GPIO_ResetBits(CAM_GPIO_GROUP,OE);
        }
    }
    if(!strcmp(ps, "rrst")){
        if(p2){
            GPIO_SetBits(CAM_GPIO_GROUP,RRST);
        }
        else{
            GPIO_ResetBits(CAM_GPIO_GROUP,RRST);
        }
    }
    if(!strcmp(ps, "rck")){
        if(p2){
            GPIO_SetBits(CAM_GPIO_GROUP,RCK);
        }
        else{
            GPIO_ResetBits(CAM_GPIO_GROUP,RCK);
        }
    }
    if(!strcmp(ps, "wck")){
        if(p2){
        }
        else{
        }
    }
    if(!strcmp(ps, "we")){
        if(p2){
            GPIO_SetBits(AL422_WG,WE);
        }
        else{
            GPIO_ResetBits(AL422_WG,WE);
        }
    }
    if(!strcmp(ps, "wrst")){
        if(p2){
            GPIO_SetBits(AL422_WG,WRST);
        }
        else{
            GPIO_ResetBits(AL422_WG,WRST);
        }
    }
    if(!strcmp(ps, "d")){
        GPIO_InitTypeDef  GPIO_InitStructure;
        if(p2>=0x100){//input
            //cam data port
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_InitStructure.GPIO_Pin = CAM_DATA_PORT_GPIO_Pins;//D1-D8
            GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
            GPIO_SetBits(GPIOB,CAM_DATA_PORT_GPIO_Pins);
            //cam data port end
            lprintf("input data %x\n", CAM_GPIO_GROUP->IDR>>CAM_DATA_OFFSET);
        }
        else{//output
            //cam data port
            CAM_GPIO_GROUP->ODR&=(~(0xff<<CAM_DATA_OFFSET));
            CAM_GPIO_GROUP->ODR|=p2<<CAM_DATA_OFFSET;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStructure.GPIO_Pin = CAM_DATA_PORT_GPIO_Pins;//D1-D8
            GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
            //cam data port end
            lprintf("output data %x\n", 0xff&(CAM_GPIO_GROUP->ODR>>CAM_DATA_OFFSET));
        }
    }
}
void cam_init(int choose)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    int clks=100;

    //cam data port
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = CAM_DATA_PORT_GPIO_Pins;//D1-D8
    GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
    GPIO_SetBits(GPIOB,CAM_DATA_PORT_GPIO_Pins);
    //cam data port end

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = VSNC | HREF;
    GPIO_Init(CAM_GPIO_GROUP, &GPIO_InitStructure);
    GPIO_SetBits(CAM_GPIO_GROUP,VSNC | HREF);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = CAM_PWN;
    GPIO_Init(CAM_PWN_GPIO_GROUP, &GPIO_InitStructure);
    lprintf("cam pwn=0\n");
    GPIO_ResetBits(CAM_GPIO_GROUP,CAM_PWN);

    GPIO_InitStructure.GPIO_Pin = CAM_RST|RRST|RCK|OE;
    GPIO_Init(CAM_GPIO_GROUP, &GPIO_InitStructure); //GPIOB
    lprintf("cam rst=1|RRST|RCK|OE\n");
    GPIO_SetBits(CAM_GPIO_GROUP,CAM_RST|RRST|RCK|OE);

    GPIO_InitStructure.GPIO_Pin = WRST|WE;
    GPIO_Init(AL422_WG, &GPIO_InitStructure); //GPIOA
    GPIO_SetBits(AL422_WG,WRST);
    GPIO_ResetBits(AL422_WG,WE);

    //END detect gpio pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_13);

    cam_i2c_init();
    cam_xclk_on();
    delay_ms(2);
    lprintf_time("cam reset return %x\n", cam_w_reg(0x12, 0x80));
    delay_ms(20);
    //read cam id
    while(1){
        if(0x76==cam_r_reg(0x0A)){
            break;
        }
        lprintf("cam read 0x0A=%b\n", cam_r_reg(0x0A));
        delay_ms(10);
    }
    lprintf("cam read 0x0A=%b\n", cam_r_reg(0x0A));
    lprintf("cam read 0x0B=%b\n", cam_r_reg(0x0B));
    while(clks--){
        GPIO_SetBits(CAM_GPIO_GROUP,RCK);
        delay_ms(2);
        GPIO_ResetBits(CAM_GPIO_GROUP,RCK);
        delay_ms(2);
    }
    switch(choose){
        case 1:
            lprintf_time("set_OV7670reg\n");
            set_OV7670reg();
            break;
            //OV7670_config_window(272,12,320,240);//
        case 2:
            lprintf_time("init_rgb565_qvga_12fps\n");
            init_rgb565_qvga_12fps();
            break;
        case 3:
            lprintf_time("init_rgb565_qvga_25fps_new\n");
            init_rgb565_qvga_25fps_new();
            break;
        case 4:
            lprintf_time("init_rgb565_qvga_25fps\n");
            init_rgb565_qvga_25fps();
            break;
        case 5:
            lprintf_time("init_yuv_25fps\n");
            init_yuv_25fps();
            break;
        case 6:
            lprintf_time("init_yuv_12fps\n");
            init_yuv_12fps();
            break;
        case 7:
            lprintf_time("modified set_OV7670reg\n");
            set_OV7670reg_M();
            break;
        default:
            lprintf("cam w 0x1e return %x\n", cam_w_reg(0x1e, 0x30|cam_r_reg(0x1e)));
            lprintf_time("no init regs\n");
    }
    //lprintf("cam w 0x70 return %x\n", cam_w_reg(0x70, 0x80|cam_r_reg(0x70)));
    //lprintf("cam w 0x71 return %x\n", cam_w_reg(0x71, 0x80|cam_r_reg(0x71)));
    lprintf("cam read 0x12=%b\n", cam_r_reg(0x12));

}

void LCD_BUS_To_write(int write)
{
    (void)write;
#ifndef NO_LCD
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if(write){
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Pin = LCD_PORT_GPIO_Pins;
        GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
        GPIO_SetBits(GPIOB,LCD_PORT_GPIO_Pins);
    }
    else{
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = LCD_PORT_GPIO_Pins;
        GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
        GPIO_SetBits(GPIOB,LCD_PORT_GPIO_Pins);
    }
#endif
}

void LCD_write(u16 VAL)
{
    (void)VAL;
#ifndef NO_LCD
	LCD_CS_CLR;  
	DATAOUT(VAL);
	LCD_WR_CLR; 
	LCD_WR_SET; 
	LCD_CS_SET;
#endif
}

u16 LCD_read(void)
{
	u16 data=0;
#ifndef NO_LCD
	LCD_CS_CLR;
	LCD_RD_CLR;
#ifdef ALIENTEK_MINI
	delay_us(2);//延时1us	
#endif
	data = DATAIN();
	LCD_RD_SET;
	LCD_CS_SET;
#endif
	return data;
}

/*****************************************************************************
 * @name       :void LCD_WR_REG(u16 data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u16 data)
{ 
   LCD_RS_CLR;     
	 #if LCD_USE8BIT_MODEL
	 LCD_write(data);
	 LCD_write(data<<8);
	 #else
	 LCD_write(data);
	 #endif
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u16 data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u16 data)
{
	 LCD_RS_SET;
	 #if LCD_USE8BIT_MODEL
	 LCD_write(data);
	 LCD_write(data<<8);
	 #else
	 LCD_write(data);
	 #endif
}

/*****************************************************************************
 * @name       :u16 LCD_RD_DATA(void)
 * @date       :2018-11-13 
 * @function   :Read an 16-bit value from the LCD screen
 * @parameters :None
 * @retvalue   :read value
******************************************************************************/
u16 LCD_RD_DATA(void)
{
	LCD_RS_SET; 
	#if LCD_USE8BIT_MODEL
	return LCD_read()>>8;
	#else
	return LCD_read();
	#endif
}

u16 LCD_RD_REG(void)
{
	LCD_RS_CLR; 
	#if LCD_USE8BIT_MODEL
	return (LCD_read()>>8);
	#else
	return LCD_read();
	#endif
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_ReadReg(u16 LCD_Reg,u16 *Rval,int n)
 * @date       :2018-11-13 
 * @function   :read value from specially registers
 * @parameters :LCD_Reg:Register address
 * @retvalue   :read value
******************************************************************************/
void LCD_ReadReg(u16 LCD_Reg,u8 *Rval,int n)
{
    (void)LCD_Reg;
    (void)Rval;
    (void)n;
#ifndef NO_LCD
	LCD_WR_REG(LCD_Reg); 
	LCD_BUS_To_write(0);
	/*
	GPIOB->CRL=0X88888888; //PB0-7  上拉输入
	GPIOB->CRH=0X88888888; //PB8-15 上拉输入
	GPIOA->CRL|=0X00008800; //PA2-3 上拉输入
	GPIOB->ODR=0X0000;     //全部输出高
	*/
	while(n--)
	{		
		*(Rval++) = LCD_RD_DATA();
	}
	LCD_BUS_To_write(1);
	
	/*
	GPIOB->CRL=0X33333333; 		//PB0-7  上拉输出
	GPIOB->CRH=0X33333333; 		//PB8-15 上拉输出
	GPIOA->CRL|=0X00003300; //PA2-3
	GPIOB->ODR=0XFFFF;    		//全部输出高  
	*/
#endif
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void LCD_ReadRAM_Prepare(void)
 * @date       :2018-11-13 
 * @function   :Read GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_ReadRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.rramcmd);
}

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(u16 Data)
{	
   LCD_RS_SET; 
	 #if LCD_USE8BIT_MODEL
		LCD_CS_CLR;
		DATAOUT(Data);
		LCD_WR_CLR; 
		LCD_WR_SET;
		DATAOUT(Data<<8);
		LCD_WR_CLR; 
		LCD_WR_SET;
		LCD_CS_SET;
 //  LCD_write(Data&0xFF00);
//	 LCD_write(Data<<8);
	 #else
	 LCD_write(Data);
	 #endif
}

u16 Color_To_565(u8 r, u8 g, u8 b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

/*****************************************************************************
 * @name       :u16 Lcd_ReadData_16Bit(void)
 * @date       :2018-11-13 
 * @function   :Read an 16-bit value from the LCD screen
 * @parameters :None
 * @retvalue   :read value
******************************************************************************/	
u16 Lcd_ReadData_16Bit(void)
{
	u16 r=0,g=0,b=0;
#if 0
	LCD_RS_SET;
	LCD_CS_CLR;
	
	//dummy data
	LCD_RD_CLR;
	delay_us(1);//延时1us	
	r = DATAIN()>>8;
	LCD_RD_SET;
	
	//8bit:red data
	//16bit:red and green data
	LCD_RD_CLR;
	delay_us(1);//延时1us	
	r = DATAIN()>>8;
	LCD_RD_SET;
	
	//8bit:green data
	//16bit:blue data
	LCD_RD_CLR;
	delay_us(1);//延时1us	
	g = DATAIN()>>8;
	LCD_RD_SET;
	
	#if LCD_USE8BIT_MODEL	
	//blue data
	LCD_RD_CLR;
	delay_us(1);//延时1us	
	b = DATAIN()>>8;
	LCD_RD_SET;
	#else
	b = g>>8;
	g = r&0xFF; 
	r = r>>8;	
	#endif
	LCD_CS_SET;
#endif
	return Color_To_565(r, g, b);
}

void LCD_DrawPoint_Color(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);//设置光标位置 
	Lcd_WriteData_16Bit(color); 
}
/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//设置光标位置 
	Lcd_WriteData_16Bit(POINT_COLOR); 
}

#ifdef ALIENTEK_MINI
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);  			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		 	 
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(x>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(x&0XFF);	  
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(y>>8);  
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(y&0XFF); 
	}else if(lcddev.id==0X1963)
	{
		if(lcddev.dir==0)x=lcddev.width-1-x;
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 		
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 		
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		
	}else if(lcddev.id==0X6804)
	{		    
		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏时处理
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		
	}else
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏其实就是调转x,y坐标
		LCD_WriteReg(lcddev.setxcmd,x);
		LCD_WriteReg(lcddev.setycmd,y);
	}	
	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(lcddev.wramcmd);//写指令  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	LCD_WR_DATA(color);		//写数据
}

void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	if((lcddev.dir==1&&lcddev.id!=0X6804&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))//横屏时，对6804和1963不改变扫描方向！竖屏时1963改变方向
	{			   
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X1963)//9341/6804/5310/5510/1963,特殊处理
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510)&&(lcddev.id!=0X1963))regval|=0X08;//5310/5510/1963不需要BGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	   
		LCD_WriteReg(dirreg,regval);
		if(lcddev.id!=0X1963)//1963不做坐标处理
		{
			if(regval&0X20)
			{
				if(lcddev.width<lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}else  
			{
				if(lcddev.width>lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}  
		}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
		}
  	}else 
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(1<<5)|(1<<4)|(0<<3); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(0<<5)|(1<<4)|(0<<3); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(0<<5)|(0<<4)|(0<<3); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(1<<5)|(1<<4)|(1<<3); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(1<<5)|(0<<4)|(1<<3); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(0<<5)|(1<<4)|(1<<3); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(0<<5)|(0<<4)|(1<<3); 
				break;	 
		} 
		dirreg=0X03;
		regval|=1<<12; 
		LCD_WriteReg(dirreg,regval);
	}
}     
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=480;
			lcddev.height=800;
		}else if(lcddev.id==0X1963)
		{
			lcddev.wramcmd=0X2C;	//设置写入GRAM的指令 
			lcddev.setxcmd=0X2B;	//设置写X坐标指令
			lcddev.setycmd=0X2A;	//设置写Y坐标指令
			lcddev.width=480;		//设置宽度480
			lcddev.height=800;		//设置高度800  
		}else
		{
			lcddev.wramcmd=0X22;
	 		lcddev.setxcmd=0X20;
			lcddev.setycmd=0X21;  
		}
	}else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}else if(lcddev.id==0X6804)	 
		{
 			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2B;
			lcddev.setycmd=0X2A; 
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=800;
			lcddev.height=480;
		}else if(lcddev.id==0X1963)
		{
			lcddev.wramcmd=0X2C;	//设置写入GRAM的指令 
			lcddev.setxcmd=0X2A;	//设置写X坐标指令
			lcddev.setycmd=0X2B;	//设置写Y坐标指令
			lcddev.width=800;		//设置宽度800
			lcddev.height=480;		//设置高度480  
		}else
		{
			lcddev.wramcmd=0X22;
	 		lcddev.setxcmd=0X21;
			lcddev.setycmd=0X20;  
		}
		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{ 	 
			lcddev.width=480;
			lcddev.height=320; 			
		}
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval; 
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
    if(sx > lcddev.width || sy > lcddev.height){
        lprintf("out of screen\n");
        return;
    }
    if(sx+width-1 > lcddev.width || sy+height-1 > lcddev.height){
        lprintf("out of screen\n");
        return;
    }
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804||(lcddev.dir==1&&lcddev.id==0X1963))
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(twidth>>8); 
		LCD_WR_DATA(twidth&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(theight>>8); 
		LCD_WR_DATA(theight&0XFF); 
	}else if(lcddev.id==0X1963)//1963竖屏特殊处理
	{
		sx=lcddev.width-width-sx; 
		height=sy+height-1; 
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA((sx+width-1)>>8); 
		LCD_WR_DATA((sx+width-1)&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(height>>8); 
		LCD_WR_DATA(height&0XFF); 		
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(sx>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(sx&0XFF);	  
		LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(twidth>>8);   
		LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(twidth&0XFF);   
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(sy>>8);   
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(sy&0XFF);  
		LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(theight>>8);   
		LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(theight&0XFF);  
	}else	//其他驱动IC
	{
		if(lcddev.dir==1)//横屏
		{
			//窗口值
			hsaval=sy;				
			heaval=theight;
			vsaval=lcddev.width-twidth-1;
			veaval=lcddev.width-sx-1;				
		}else
		{ 
			hsaval=sx;				
			heaval=twidth;
			vsaval=sy;
			veaval=theight;
		} 
		hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
		vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器	   							  
		//设置寄存器值
		LCD_WriteReg(hsareg,hsaval);
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);
		LCD_WriteReg(veareg,veaval);		
		LCD_SetCursor(sx,sy);	//设置光标位置
	}
}
#endif

/*****************************************************************************
 * @name       :u16 LCD_ReadPoint(u16 x,u16 y)
 * @date       :2018-11-13 
 * @function   :Read a pixel color value at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :the read color value
******************************************************************************/	
u16 LCD_ReadPoint(u16 x,u16 y)
{
	u16 color=0;
    (void)x;
    (void)y;
#ifndef NO_LCD
	if(x>=lcddev.width||y>=lcddev.height)
	{
		return 0;	//超过了范围,直接返回	
	}
	LCD_SetCursor(x,y);//设置光标位置 
	LCD_ReadRAM_Prepare();
	LCD_BUS_To_write(0);
	
	color = Lcd_ReadData_16Bit();
	
	LCD_BUS_To_write(1);
#endif
	return color;
}

void lcd_clr_window(u16 color, u16 xs, u16 ys, u16 xe, u16 ye)
{
    //lprintf("clrw:%d %d %d %d\n", xs, ys, xe, ye);
#ifdef ALIENTEK_MINI
    u16 w, h;
    w = xe - xs + 1;
    h = ye - ys + 1;
	u32 index=0;      
	//u32 totalpoint=lcddev.width;
	u32 totalpoint=w;
	totalpoint*=h; 			//得到总点数
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804横屏的时候特殊处理  
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_SetCursor(0x00,0x0000);		//设置光标位置  
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else LCD_SetCursor(0x00,0x0000);	//设置光标位置 
LCD_Set_Window(xs,ys,w, h);
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	  	  
	for(index=0;index<totalpoint;index++)LCD_WR_DATA(color);	
#else
  unsigned int i;//,m;
	LCD_SetWindows(xs,ys,xe,ye);   
	for(i=0;i<((unsigned int)xe-xs+1)*(ye-ys+1);i++)
	{
 //   for(m=0;m<lcddev.width;m++)
  //  {	
			Lcd_WriteData_16Bit(color);
	//	}
	}
#endif
} 

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
	Proxy_lcd_clr_window(Color, 0,0,lcddev.width-1,lcddev.height-1);
}

/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_GPIOInit(void)
{
#ifndef NO_LCD
	GPIO_InitTypeDef  GPIO_InitStructure;
	      
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
#ifdef ALIENTEK_MINI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOC,GPIO_Pin_6 |GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14| GPIO_Pin_15; //GPIOC14,15,13
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_1| GPIO_Pin_0; //GPIOA8, 0, 1
	GPIO_Init(GPIOA, &GPIO_InitStructure); //GPIOA
	GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_1| GPIO_Pin_8);	
#endif

	LCD_BUS_To_write(1);
	lprintf("LCD gpio init done\n");
#endif
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
#ifndef NO_LCD
#ifdef ALIENTEK_MINI
#else
	LCD_RST_CLR;
	delay_ms(100);	
	LCD_RST_SET;
	delay_ms(50);
#endif
#endif
}

void LCD_hw_test(int testitem)
{
    (void)testitem;
#ifndef NO_LCD
	u16 testdata = 1;
	int i = 0;
	switch(testitem){
		case LCD_HW_GPIO_TEST:
			lprintf("LCD_HW_GPIO_TEST:\n");
			while(1){
				lprintf("LCD RST low\n");
				LCD_RST_CLR;
					delay_ms(1000);	
					lprintf("LCD RST high\n");
				LCD_RST_SET;
					delay_ms(1000);

					lprintf("LCD CS low\n");
				LCD_CS_CLR;     
					delay_ms(1000);	
					lprintf("LCD CS high\n");
				LCD_CS_SET;
					delay_ms(1000);

					lprintf("LCD RD low\n");
				LCD_RD_CLR;     
					delay_ms(1000);	
					lprintf("LCD RD high\n");
				LCD_RD_SET;
					delay_ms(1000);

					lprintf("LCD WR low\n");
				LCD_WR_CLR;     
					delay_ms(1000);	
					lprintf("LCD WR high\n");
				LCD_WR_SET;
					delay_ms(1000);

					lprintf("LCD RS low\n");
				LCD_RS_CLR;     
					delay_ms(1000);	
					lprintf("LCD RS high\n");
				LCD_RS_SET;
					delay_ms(1000);
					lprintf("LCD LED low\n");
				LCD_LED_CLR;     
					delay_ms(1000);	
					lprintf("LCD LED high\n");
				LCD_LED_SET;
					delay_ms(1000);
			}
			break;
		case LCD_HW_WRITE_TEST:
			lprintf("LCD_HW_WRITE_TEST:\n");
			while(1){
				lprintf("B%x=1 testdata %x\n", i++, testdata);
				LCD_write(testdata);
				testdata<<=1;
				if(i>15){
					i = 0;
					testdata = 1;
				}
				delay_ms(1000);	
			}
			break;
               case LCD_HW_READ_TEST:
                       lprintf("LCD_HW_READ_TEST:\n");
		       LCD_BUS_To_write(0);
                       while(1){
                               lprintf("Read %x\n", LCD_RD_DATA());
                               delay_ms(1000);
                       }
                       break;

		default:
			lprintf("no HW test request\n");
	}
#endif
}

void lcd_sueb_basicinit()
{
	LCD_GPIOInit();//LCD GPIO初始化
	delay_ms(100);
 	LCD_RESET(); //LCD 复位
}

//#define PWMV 2
uint32_t PWMV = 2;
//PWM freq=72000/(PWMV)/2=2Mhz
#define BLPWM_ARR (PWMV-1)
#define BLPWM_PSC 1
void BL_PWM_init()
{
    uint16_t arr = BLPWM_ARR;
    uint16_t psc = BLPWM_PSC;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

    //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    TIM_TimeBaseStructure.TIM_Period = arr;//auto reloaded value
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

    TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 

    TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器

    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}

void cam_xclk_on()
{
    GPIO_InitTypeDef GPIO_InitS_xclk_on;
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

    GPIO_InitS_xclk_on.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
    GPIO_InitS_xclk_on.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitS_xclk_on.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitS_xclk_on);
}

void cam_xclk_off()
{
    GPIO_InitTypeDef GPIO_InitS_xclk_off;
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

    GPIO_InitS_xclk_off.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitS_xclk_off.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitS_xclk_off.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitS_xclk_off);
    GPIOA->BRR = GPIO_Pin_8;
}

void set_xclk(uint32_t fct)
{
    PWMV = fct;
    BL_PWM_deinit();
    BL_PWM_init();
    lprintf("xclk=%dM\n", 36/fct);
    cam_xclk_off();
    cam_xclk_on();
}

void BL_PWM_deinit()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    TIM_Cmd(TIM1, DISABLE);

    TIM_ARRPreloadConfig(TIM1, DISABLE);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);

    TIM_CtrlPWMOutputs(TIM1,DISABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);
}

//0-100
static uint16_t cur_bl_value = 0;
void set_BL_value(uint16_t v)
{
    static int BL_PWM_inited = 0;
    uint16_t comp_v;
    lprintf("-----------------------------------v=%d\n", v);
    if(v>100){
        lprintf("err:BL v>100\n");
        return;
    }
    cur_bl_value = v;
    if(v>0){
        comp_v = PWMV*(100-v)/100;
        lprintf("comp_v=%x\n", comp_v);
        TIM_SetCompare1(TIM1,comp_v);
        if(!BL_PWM_inited){
            BL_PWM_init();
            BL_PWM_inited = 1;
        }
        TIM_SetCompare1(TIM1,comp_v);
    }
    else{//v=0
        BL_PWM_deinit();
        BL_PWM_inited = 0;
    }
}

uint16_t get_BL_value()
{
    //lprintf("%d\n", (uint32_t)cur_bl_value);
    return cur_bl_value;
}

/*****************************************************************************
 * @name       :void lcd_sueb_init(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void lcd_sueb_init(int testitem)
{
    (void)testitem;
    set_BL_value(DEFAULT_BL);//quater bright
}
#if 0
void lcd_sueb_init(int testitem)
{  
	u16 lcd_id;
	lcd_sueb_basicinit();
	LCD_hw_test(testitem);
    lcd_id = LCD_Read_ID();
  	lcddev.id = lcd_id;
	lprintf("read id 0x%x\n", lcd_id);
#ifdef ALIENTEK_MINI
		LCD_WriteReg(0X00,0X0001);   
		delay_ms(10);   
		LCD_WriteReg(0X10,0X1628);   
		LCD_WriteReg(0X12,0X000e);//0x0006    
		LCD_WriteReg(0X13,0X0A39);   
		delay_ms(10);   
		LCD_WriteReg(0X11,0X0040);   
		LCD_WriteReg(0X15,0X0050);   
		delay_ms(10);   
		LCD_WriteReg(0X12,0X001e);//16    
		delay_ms(10);   
		LCD_WriteReg(0X10,0X1620);   
		LCD_WriteReg(0X13,0X2A39);   
		delay_ms(10);   
		LCD_WriteReg(0X01,0X0100);   
		LCD_WriteReg(0X02,0X0300);   
		LCD_WriteReg(0X03,0X1038);//改变方向的   
		LCD_WriteReg(0X08,0X0202);   
		LCD_WriteReg(0X0A,0X0008);   
		LCD_WriteReg(0X30,0X0000);   
		LCD_WriteReg(0X31,0X0402);   
		LCD_WriteReg(0X32,0X0106);   
		LCD_WriteReg(0X33,0X0503);   
		LCD_WriteReg(0X34,0X0104);   
		LCD_WriteReg(0X35,0X0301);   
		LCD_WriteReg(0X36,0X0707);   
		LCD_WriteReg(0X37,0X0305);   
		LCD_WriteReg(0X38,0X0208);   
		LCD_WriteReg(0X39,0X0F0B);   
		LCD_WriteReg(0X41,0X0002);   
		LCD_WriteReg(0X60,0X2700);   
		LCD_WriteReg(0X61,0X0001);   
		LCD_WriteReg(0X90,0X0210);   
		LCD_WriteReg(0X92,0X010A);   
		LCD_WriteReg(0X93,0X0004);   
		LCD_WriteReg(0XA0,0X0100);   
		LCD_WriteReg(0X07,0X0001);   
		LCD_WriteReg(0X07,0X0021);   
		LCD_WriteReg(0X07,0X0023);   
		LCD_WriteReg(0X07,0X0033);   
		LCD_WriteReg(0X07,0X0133);   
		LCD_WriteReg(0XA0,0X0000); 
	delay_ms(50);
	LCD_Display_Dir(0);		 	//默认为竖屏
	LCD_LED_SET;					//点亮背光
	LCD_Clear(WHITE);//清全屏白色
#else
//************* OTM8009初始化**********//	
	//3.97inch OTM8009 Init 20190116
	LCD_WR_REG(0xff00);
	LCD_WR_DATA(0x80);
	LCD_WR_REG(0xff01);
	LCD_WR_DATA(0x09);
	LCD_WR_REG(0xff02);
	LCD_WR_DATA(0x01);

	LCD_WR_REG(0xff80);
	LCD_WR_DATA(0x80);
	LCD_WR_REG(0xff81);
	LCD_WR_DATA(0x09);

	LCD_WR_REG(0xff03);
	LCD_WR_DATA(0x01);

	//add ==========20131216============================//
	LCD_WR_REG(0xf5b6); 
	LCD_WR_DATA(0x06); 
	LCD_WR_REG(0xc480); 
	LCD_WR_DATA(0x30); 
	LCD_WR_REG(0xc48a); 
	LCD_WR_DATA(0x40); 
	//===================================================//
	LCD_WR_REG(0xc0a3);
	LCD_WR_DATA(0x1B);

	//LCD_WR_REG(0xc0ba);  //No
	//LCD_WR_DATA(0x50);

	LCD_WR_REG(0xc0ba); //--> (0xc0b4); // column inversion //  2013.12.16 modify
	LCD_WR_DATA(0x50); 

	LCD_WR_REG(0xc181);
	LCD_WR_DATA(0x66);

	LCD_WR_REG(0xc1a1);
	LCD_WR_DATA(0x0E);

	LCD_WR_REG(0xc481);
	LCD_WR_DATA(0x83);

	LCD_WR_REG(0xc582);
	LCD_WR_DATA(0x83);

	LCD_WR_REG(0xc590);
	LCD_WR_DATA(0x96);

	LCD_WR_REG(0xc591);
	LCD_WR_DATA(0x2B);

	LCD_WR_REG(0xc592);
	LCD_WR_DATA(0x01);


	LCD_WR_REG(0xc594);
	LCD_WR_DATA(0x33);

	LCD_WR_REG(0xc595);
	LCD_WR_DATA(0x34);


	LCD_WR_REG(0xc5b1);
	LCD_WR_DATA(0xa9);

	LCD_WR_REG(0xce80);
	LCD_WR_DATA(0x86);
	LCD_WR_REG(0xce81);
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xce82);
	LCD_WR_DATA(0x00); 

	LCD_WR_REG(0xce83);
	LCD_WR_DATA(0x85); 
	LCD_WR_REG(0xce84);
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xce85);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xce86);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xce87);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xce88);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xce89);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xce8A);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xce8B);
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0xcea0);// cea1[7:0] : clka1_width[3:0], clka1_shift[11:8]                         
	LCD_WR_DATA(0x18); 
	LCD_WR_REG(0xcea1);// cea2[7:0] : clka1_shift[7:0]                                            
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcea2);// cea3[7:0] : clka1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]   
	LCD_WR_DATA(0x03); 
	LCD_WR_REG(0xcea3);// cea4[7:0] : clka1_switch[7:0]                                               
	LCD_WR_DATA(0x21); 
	LCD_WR_REG(0xcea4);// cea5[7:0] : clka1_extend[7:0]                                           
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcea5);// cea6[7:0] : clka1_tchop[7:0]                                            
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcea6);// cea7[7:0] : clka1_tglue[7:0]                                            
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcea7);// cea8[7:0] : clka2_width[3:0], clka2_shift[11:8]                         
	LCD_WR_DATA(0x18); 
	LCD_WR_REG(0xcea8);// cea9[7:0] : clka2_shift[7:0]                                            
	LCD_WR_DATA(0x03);
	LCD_WR_REG(0xcea9);// ceaa[7:0] : clka2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]   
	LCD_WR_DATA(0x03); 
	LCD_WR_REG(0xceaa);// ceab[7:0] : clka2_switch[7:0]                                                
	LCD_WR_DATA(0x22);
	LCD_WR_REG(0xceab);// ceac[7:0] : clka2_extend                                                
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xceac);// cead[7:0] : clka2_tchop                                                 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcead);// ceae[7:0] : clka2_tglue 
	LCD_WR_DATA(0x00);    

	LCD_WR_REG(0xceb0);// ceb1[7:0] : clka3_width[3:0], clka3_shift[11:8]                          
	LCD_WR_DATA(0x18);
	LCD_WR_REG(0xceb1);// ceb2[7:0] : clka3_shift[7:0]                                             
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xceb2);// ceb3[7:0] : clka3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]    
	LCD_WR_DATA(0x03); 
	LCD_WR_REG(0xceb3);// ceb4[7:0] : clka3_switch[7:0]                                               
	LCD_WR_DATA(0x23); 
	LCD_WR_REG(0xceb4);// ceb5[7:0] : clka3_extend[7:0]                                            
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xceb5);// ceb6[7:0] : clka3_tchop[7:0]                                             
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xceb6);// ceb7[7:0] : clka3_tglue[7:0]                                             
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xceb7);// ceb8[7:0] : clka4_width[3:0], clka2_shift[11:8]                          
	LCD_WR_DATA(0x18);
	LCD_WR_REG(0xceb8);// ceb9[7:0] : clka4_shift[7:0]                                             
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xceb9);// ceba[7:0] : clka4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]    
	LCD_WR_DATA(0x03); 
	LCD_WR_REG(0xceba);// cebb[7:0] : clka4_switch[7:0]                                                
	LCD_WR_DATA(0x24); 
	LCD_WR_REG(0xcebb);// cebc[7:0] : clka4_extend                                                 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcebc);// cebd[7:0] : clka4_tchop                                                  
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcebd);// cebe[7:0] : clka4_tglue                                                  
	LCD_WR_DATA(0x00); 


	LCD_WR_REG(0xcfc0);// cfc1[7:0] : eclk_normal_width[7:0]   
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xcfc1);// cfc2[7:0] : eclk_partial_width[7:0]                                                                                  
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xcfc2);// cfc3[7:0] : all_normal_tchop[7:0]                                                                                    
	LCD_WR_DATA(0x20); 
	LCD_WR_REG(0xcfc3);// cfc4[7:0] : all_partial_tchop[7:0]                                                                                   
	LCD_WR_DATA(0x20); 
	LCD_WR_REG(0xcfc4);// cfc5[7:0] : eclk1_follow[3:0], eclk2_follow[3:0]                                                                     
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcfc5);// cfc6[7:0] : eclk3_follow[3:0], eclk4_follow[3:0]                                                                     
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcfc6);// cfc7[7:0] : 00, vstmask, vendmask, 00, dir1, dir2 (0=VGL, 1=VGH)                                                     
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xcfc7);// cfc8[7:0] : reg_goa_gnd_opt, reg_goa_dpgm_tail_set, reg_goa_f_gating_en, reg_goa_f_odd_gating, toggle_mod1, 2, 3, 4  
	LCD_WR_DATA(0x00);    // GND OPT1 (00-->80  2011/10/28)
	LCD_WR_REG(0xcfc8);// cfc9[7:0] : duty_block[3:0], DGPM[3:0]                                                                               
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcfc9);// cfca[7:0] : reg_goa_gnd_period[7:0]                                                                                  
	LCD_WR_DATA(0x00);    // Gate PCH (CLK base) (00-->0a  2011/10/28)

	LCD_WR_REG(0xcfd0);// cfd1[7:0] : 0000000, reg_goa_frame_odd_high
	LCD_WR_DATA(0x00); 

	LCD_WR_REG(0xcbc0);//cbc1[7:0] : enmode H-byte of sig1  (pwrof_0, pwrof_1, norm, pwron_4 )           
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbc1);//cbc2[7:0] : enmode H-byte of sig2  (pwrof_0, pwrof_1, norm, pwron_4 )          
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbc2);//cbc3[7:0] : enmode H-byte of sig3  (pwrof_0, pwrof_1, norm, pwron_4 )           
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbc3);//cbc4[7:0] : enmode H-byte of sig4  (pwrof_0, pwrof_1, norm, pwron_4 )        
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbc4);//cbc5[7:0] : enmode H-byte of sig5  (pwrof_0, pwrof_1, norm, pwron_4 )             
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbc5);//cbc6[7:0] : enmode H-byte of sig6  (pwrof_0, pwrof_1, norm, pwron_4 )           
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbc6);//cbc7[7:0] : enmode H-byte of sig7  (pwrof_0, pwrof_1, norm, pwron_4 )           
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbc7);//cbc8[7:0] : enmode H-byte of sig8  (pwrof_0, pwrof_1, norm, pwron_4 )           
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbc8);//cbc9[7:0] : enmode H-byte of sig9  (pwrof_0, pwrof_1, norm, pwron_4 )           
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbc9);//cbca[7:0] : enmode H-byte of sig10 (pwrof_0, pwrof_1, norm, pwron_4 )        
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbca);//cbcb[7:0] : enmode H-byte of sig11 (pwrof_0, pwrof_1, norm, pwron_4 )        
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbcb);//cbcc[7:0] : enmode H-byte of sig12 (pwrof_0, pwrof_1, norm, pwron_4 )        
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbcc);//cbcd[7:0] : enmode H-byte of sig13 (pwrof_0, pwrof_1, norm, pwron_4 )        
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbcd);//cbce[7:0] : enmode H-byte of sig14 (pwrof_0, pwrof_1, norm, pwron_4 ) 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbce);//cbcf[7:0] : enmode H-byte of sig15 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 

	LCD_WR_REG(0xcbd0);//cbd1[7:0] : enmode H-byte of sig16 (pwrof_0, pwrof_1, norm, pwron_4 )           
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbd1);//cbd2[7:0] : enmode H-byte of sig17 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbd2);//cbd3[7:0] : enmode H-byte of sig18 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbd3);//cbd4[7:0] : enmode H-byte of sig19 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbd4);//cbd5[7:0] : enmode H-byte of sig20 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbd5);//cbd6[7:0] : enmode H-byte of sig21 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbd6);//cbd7[7:0] : enmode H-byte of sig22 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbd7);//cbd8[7:0] : enmode H-byte of sig23 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbd8);//cbd9[7:0] : enmode H-byte of sig24 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbd9);//cbda[7:0] : enmode H-byte of sig25 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbda);//cbdb[7:0] : enmode H-byte of sig26 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xcbdb);//cbdc[7:0] : enmode H-byte of sig27 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbdc);//cbdd[7:0] : enmode H-byte of sig28 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbdd);//cbde[7:0] : enmode H-byte of sig29 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbde);//cbdf[7:0] : enmode H-byte of sig30 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 

	LCD_WR_REG(0xcbe0);//cbe1[7:0] : enmode H-byte of sig31 (pwrof_0, pwrof_1, norm, pwron_4 )             
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe1);//cbe2[7:0] : enmode H-byte of sig32 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe2);//cbe3[7:0] : enmode H-byte of sig33 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe3);//cbe4[7:0] : enmode H-byte of sig34 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe4);//cbe5[7:0] : enmode H-byte of sig35 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe5);//cbe6[7:0] : enmode H-byte of sig36 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe6);//cbe7[7:0] : enmode H-byte of sig37 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe7);//cbe8[7:0] : enmode H-byte of sig38 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe8);//cbe9[7:0] : enmode H-byte of sig39 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcbe9);//cbea[7:0] : enmode H-byte of sig40 (pwrof_0, pwrof_1, norm, pwron_4 )
	LCD_WR_DATA(0x00);

	// cc8x   
	LCD_WR_REG(0xcc80);//cc81[7:0] : reg setting for signal01 selection with u2d mode   
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc81);//cc82[7:0] : reg setting for signal02 selection with u2d mode 
	LCD_WR_DATA(0x26); 
	LCD_WR_REG(0xcc82);//cc83[7:0] : reg setting for signal03 selection with u2d mode 
	LCD_WR_DATA(0x09); 
	LCD_WR_REG(0xcc83);//cc84[7:0] : reg setting for signal04 selection with u2d mode 
	LCD_WR_DATA(0x0B); 
	LCD_WR_REG(0xcc84);//cc85[7:0] : reg setting for signal05 selection with u2d mode 
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xcc85);//cc86[7:0] : reg setting for signal06 selection with u2d mode 
	LCD_WR_DATA(0x25); 
	LCD_WR_REG(0xcc86);//cc87[7:0] : reg setting for signal07 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc87);//cc88[7:0] : reg setting for signal08 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc88);//cc89[7:0] : reg setting for signal09 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc89);//cc8a[7:0] : reg setting for signal10 selection with u2d mode 
	LCD_WR_DATA(0x00);  

	// cc9x   
	LCD_WR_REG(0xcc90);//cc91[7:0] : reg setting for signal11 selection with u2d mode   
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc91);//cc92[7:0] : reg setting for signal12 selection with u2d mode
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc92);//cc93[7:0] : reg setting for signal13 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc93);//cc94[7:0] : reg setting for signal14 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc94);//cc95[7:0] : reg setting for signal15 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc95);//cc96[7:0] : reg setting for signal16 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc96);//cc97[7:0] : reg setting for signal17 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc97);//cc98[7:0] : reg setting for signal18 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc98);//cc99[7:0] : reg setting for signal19 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc99);//cc9a[7:0] : reg setting for signal20 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc9a);//cc9b[7:0] : reg setting for signal21 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcc9b);//cc9c[7:0] : reg setting for signal22 selection with u2d mode 
	LCD_WR_DATA(0x26); 
	LCD_WR_REG(0xcc9c);//cc9d[7:0] : reg setting for signal23 selection with u2d mode 
	LCD_WR_DATA(0x0A); 
	LCD_WR_REG(0xcc9d);//cc9e[7:0] : reg setting for signal24 selection with u2d mode 
	LCD_WR_DATA(0x0C); 
	LCD_WR_REG(0xcc9e);//cc9f[7:0] : reg setting for signal25 selection with u2d mode 
	LCD_WR_DATA(0x02);  
	// ccax   
	LCD_WR_REG(0xcca0);//cca1[7:0] : reg setting for signal26 selection with u2d mode   
	LCD_WR_DATA(0x25); 
	LCD_WR_REG(0xcca1);//cca2[7:0] : reg setting for signal27 selection with u2d mode
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca2);//cca3[7:0] : reg setting for signal28 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca3);//cca4[7:0] : reg setting for signal29 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca4);//cca5[7:0] : reg setting for signal20 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca5);//cca6[7:0] : reg setting for signal31 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca6);//cca7[7:0] : reg setting for signal32 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca7);//cca8[7:0] : reg setting for signal33 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca8);//cca9[7:0] : reg setting for signal34 selection with u2d mode 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xcca9);//ccaa[7:0] : reg setting for signal35 selection with u2d mode 
	LCD_WR_DATA(0x00); 

  LCD_WR_REG(0x3A00);//ccaa[7:0] : reg setting for signal35 selection with u2d mode 
	LCD_WR_DATA(0x55);//0x55
		
	LCD_WR_REG(0x1100);
	delay_ms(100);
	LCD_WR_REG(0x2900);
	delay_ms(50);
	LCD_WR_REG(0x2C00); 
  LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
    set_BL_value(DEFAULT_BL);//quater bright
	LCD_Clear(WHITE);//清全屏白色

    prtline;
    //sd init
    do
    {
        SD_CardInfo mycard;
        lprintf("Detecting sd card...");
        if(SD_Init() != SD_OK )
        {
            lprintf("retry once\r\n");
            if(SD_Init() != SD_OK){
                lprintf("Fail\n");
                break;
            }
            lprintf("retry OK\r\n");
        }
        else{
            lprintf("OK\n");
        }
        if(SD_GetCardInfo(&mycard) != SD_OK)
        {
            lprintf("get card info Fail\n");
            break;
        }
        else{
            lprintf("block size %d\n", mycard.CardBlockSize);
            lprintf("block capacity %d\n", mycard.CardCapacity);
        }
        lprintf("filesize 0x%x\r\n", get_file_size("YUV1", "BIN"));
        close_file();
    }
    while(0);
    //sd init end
#endif
}
#endif
 
void pure_lcd_sueb_test()
{
	//while(1)
	{
#if 0
		lprintf("test main interface.Any key continue...\n");
		main_test(); 		//测试主界面
		//Test_Read();     //读ID和颜色值测试
		lprintf("test simple fill color.Any key continue...\n");
		Test_Color();  		//简单刷屏填充测试
		lprintf("test rect drawing.Any key continue...\n");
		Test_FillRec();		//GUI矩形绘图测试
		lprintf("test circle drawing.Any key continue...\n");
		Test_Circle(); 		//GUI画圆测试
		lprintf("test trigangle drawing.Any key continue...\n");
		Test_Triangle();    //GUI三角形绘图测试
		lprintf("test english font.Any key continue...\n");
		English_Font_test();//英文字体示例测试
		lprintf("test chinese font.Any key continue...\n");
		Chinese_Font_test();//中文字体示例测试
		lprintf("test pic display.Any key continue...\n");
		Pic_test();			//图片显示示例测试
		lprintf("test rotate display.Any key continue...\n");
#endif
		Rotate_Test();   //旋转显示测试
		lprintf("test touch.\n");
		//如果不带触摸，或者不需要触摸功能，请注释掉下面触摸屏测试项
		Touch_Test();		//触摸屏手写测试		
	}
}

void lcd_sueb_test()
{
	//while(1)
	{
		lprintf("test touch.Any key continue...\n");
		con_recv();
		//如果不带触摸，或者不需要触摸功能，请注释掉下面触摸屏测试项
		Touch_Test();		//触摸屏手写测试		
		lprintf("test main interface.Any key continue...\n");
#if 0
		main_test(); 		//测试主界面
		//Test_Read();     //读ID和颜色值测试
		lprintf("test simple fill color.Any key continue...\n");
		con_recv();
		Test_Color();  		//简单刷屏填充测试
		lprintf("test rect drawing.Any key continue...\n");
		con_recv();
		Test_FillRec();		//GUI矩形绘图测试
		lprintf("test circle drawing.Any key continue...\n");
		con_recv();
		Test_Circle(); 		//GUI画圆测试
		lprintf("test trigangle drawing.Any key continue...\n");
		con_recv();
		Test_Triangle();    //GUI三角形绘图测试
		lprintf("test english font.Any key continue...\n");
		con_recv();
		English_Font_test();//英文字体示例测试
		lprintf("test chinese font.Any key continue...\n");
		con_recv();
		Chinese_Font_test();//中文字体示例测试
		lprintf("test pic display.Any key continue...\n");
		con_recv();
		Pic_test();			//图片显示示例测试
#endif
		lprintf("test rotate display.Any key continue...\n");
		con_recv();
		Rotate_Test();   //旋转显示测试
	}
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
#ifdef ALIENTEK_MINI
    //lprintf("lcdwin:%d %d %d %d\n", (u32)xStar, (u32)yStar, (u32)xEnd, (u32)yEnd);
LCD_Set_Window(xStar,yStar,xEnd-xStar+1,yEnd-yStar+1);
LCD_WriteRAM_Prepare();
#else
	LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(xStar>>8);  
	LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(xStar&0XFF);	  
	LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(xEnd>>8);   
	LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(xEnd&0XFF);   
	LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(yStar>>8);   
	LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(yStar&0XFF);  
	LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(yEnd>>8);   
	LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(yEnd&0XFF); 

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
#endif
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
#ifdef ALIENTEK_MINI
    if(Xpos > lcddev.width || Ypos > lcddev.height){
        lprintf("out of screen\n");
        return;
    }
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
	}else if(lcddev.id==0X6804)
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏时处理
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 
	}else if(lcddev.id==0X1963)
	{  			 		
		if(lcddev.dir==0)//x坐标需要变换
		{
			Xpos=lcddev.width-1-Xpos;
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0); 		
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);		 	 
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 		
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);		 	 			
		}	
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF); 			 		
		
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(Xpos>>8); 		
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(Xpos&0XFF);			 
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(Ypos>>8);  		
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(Ypos&0XFF);			
	}else
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
		LCD_WriteReg(lcddev.setxcmd, Xpos);
		LCD_WriteReg(lcddev.setycmd, Ypos);
	}	 
LCD_WriteRAM_Prepare();
#else
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
#endif
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
	lcddev.setxcmd=0x2A00;
	lcddev.setycmd=0x2B00;
	lcddev.wramcmd=0x2C00;
	lcddev.rramcmd=0x2E00;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x3600,0x00);
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x3600,(1<<5)|(1<<6));
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x3600,(1<<7)|(1<<6));
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x3600,(1<<7)|(1<<5));
		break;	
		default:break;
	}	
}	 

/*****************************************************************************
 * @name       :u16 LCD_Read_ID(void)
 * @date       :2018-11-13 
 * @function   :Read ID
 * @parameters :None
 * @retvalue   :ID value
******************************************************************************/ 
u16 LCD_Read_ID(void)
{
#ifdef ALIENTEK_MINI
	u16 val;
    delay_ms(50); // delay 50 ms 
	LCD_WriteReg(0x0000,0x0001);
	delay_ms(50); // delay 50 ms 
	LCD_ReadReg(0,(u8*)&val,1);
	return val;
#else
	u8 val[4] = {0};
	LCD_ReadReg(0xD300,val,4);
	return (val[2]<<8)|val[3];
#endif
}
