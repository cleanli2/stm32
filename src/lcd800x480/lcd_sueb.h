//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103RCT6,����ԭ��MiniSTM32������,��Ƶ72MHZ������12MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//wiki������վ��http://www.lcdwiki.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//����֧��QQ:3002773612  3002778157
//��������QQȺ:324828016
//��������:2018/08/09
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
//     LCDģ��                STM32��Ƭ��
//      VDD          ��        DC5V/3.3V        //��Դ
//      GND          ��          GND            //��Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������Ϊ16λ��������
//     LCDģ��                STM32��Ƭ��    
//     DB0~DB7       ��        PB0~PB7         //Һ������8λ���������ź�
//     DB8~DB15      ��        PB8~PB15        //Һ������8λ���������ź�
//=======================================Һ���������߽���==========================================//
//     LCDģ�� 				        STM32��Ƭ�� 
//       WR          ��          PA11           //Һ����д���ݿ����ź�
//       RD          ��          PC14           //Һ���������ݿ����ź�
//       RS          ��          PC15           //Һ��������/��������ź�
//       RST         ��          PA8           //Һ������λ�����ź�
//       CS          ��          PA12           //Һ����Ƭѡ�����ź�
//       BL          ��          PC13          //Һ������������ź�
//=========================================������������=========================================//
//���ģ�鲻���������ܻ��ߴ��д������ܣ����ǲ���Ҫ�������ܣ�����Ҫ���д���������
//	   LCDģ��                STM32��Ƭ�� 
//       PEN         ��          PC1           //�����������ж��ź�
//       MISO        ��          PC2           //������SPI���߶��ź�
//       MOSI        ��          PC3           //������SPI����д�ź�
//       T_CS        ��          PC13          //������Ƭѡ�����ź�
//       CLK         ��          PC0           //������SPI����ʱ���ź�
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
#ifndef __LCD_SUEB_H
#define __LCD_SUEB_H		
#include "common.h"
#include "stdlib.h"

//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				  //LCD ID
	u8  dir;			  //���������������ƣ�0��������1��������	
	u16	 wramcmd;		//��ʼдgramָ��
	u16  rramcmd;   //��ʼ��gramָ��
	u16  setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��	 
}_lcd_dev; 	

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
/////////////////////////////////////�û�������///////////////////////////////////	 
#define USE_HORIZONTAL  	  3 //����Һ����˳ʱ����ת���� 	0-0����ת��1-90����ת��2-180����ת��3-270����ת
#define USE_VERTICAL  	 0


//TFTLCD������Ҫ���õĺ���		   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

//////////////////////////////////////////////////////////////////////
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��
	 
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���	 

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��

//������ɫ
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //��ɫ
#define BRRED 			0XFC07 //�غ�ɫ
#define GRAY  			0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	0X841F //ǳ��ɫ
#define LIGHTGRAY     0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 		0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE      	0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE          0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

#define DEFAULT_BL 10
#define DEFAULT_IDLE_BL 50
#define LCD_HW_GPIO_TEST 1
#define LCD_HW_WRITE_TEST 2
#define LCD_HW_READ_TEST 3
void lcd_sueb_init(int testitem);
void LCD_write(u16 VAL);
u16 LCD_read(void);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//����
void LCD_DrawPoint_Color(u16 x,u16 y,u16 color);//����
u16  LCD_ReadPoint(u16 x,u16 y); //����	   
void lcd_clr_window(u16 color, u16 xs, u16 ys, u16 xe, u16 ye);
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
u16 LCD_RD_DATA(void);//��ȡLCD����								    
u16 LCD_RD_REG(void);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_REG(u16 data);
void LCD_WR_DATA(u16 data);
void LCD_ReadReg(u16 LCD_Reg,u8 *Rval,int n);
void LCD_WriteRAM_Prepare(void);
void LCD_ReadRAM_Prepare(void);   
u16 Lcd_ReadData_16Bit(void);
void LCD_direction(u8 direction );
u16 Color_To_565(u8 r, u8 g, u8 b);
u16 LCD_Read_ID(void);
void set_BL_value(uint16_t v);
uint16_t get_BL_value();
void LCD_RESET();
void lcd_sueb_test();
void lcd_sueb_basicinit();
void lcd_w16(u16 c);

#if 0
#define Lcd_WriteData_16Bit(Data) {\
    LCD_RS_SET;\
    LCD_CS_CLR; \
    DATAOUT(Data);\
    LCD_WR_CLR; \
    LCD_WR_SET; \
    LCD_CS_SET; \
}
#endif
#define Lcd_WriteData_16Bit(Data) lcd_w16(Data)


//�����Ȼ�����ٶȲ����죬����ʹ������ĺ궨��,����ٶ�.
//ע��Ҫȥ��lcd.c��void LCD_WR_DATA(u16 data)��������Ŷ
/*
#if LCD_USE8BIT_MODEL==1//ʹ��8λ������������ģʽ
	#define LCD_WR_DATA(data){\
	LCD_RS_SET;\
	LCD_CS_CLR;\
	DATAOUT(data);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	DATAOUT(data<<8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
	}
	#else//ʹ��16λ������������ģʽ
	#define LCD_WR_DATA(data){\
	LCD_RS_SET;\
	LCD_CS_CLR;\
	DATAOUT(data);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
	} 	
#endif
*/
				  		 
#endif  
	 
	 



