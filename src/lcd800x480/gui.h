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
//       WR          ��          PC7           //Һ����д���ݿ����ź�
//       RD          ��          PC6           //Һ���������ݿ����ź�
//       RS          ��          PC8           //Һ��������/��������ź�
//       RST         ��          PC4           //Һ������λ�����ź�
//       CS          ��          PC9           //Һ����Ƭѡ�����ź�
//       BL          ��          PC10          //Һ������������ź�
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
#ifndef __GUI_H__
#define __GUI_H__

typedef struct window{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t dx;
    uint32_t dy;
} *win_pt, win;

#define RATIO_BASE_OF_LENGTH 20
void GUI_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawLine_direction(u16 x1, u16 y1, u16 x2, u16 y2, int start, int end);
void Draw_Circle(u16 x0,u16 y0,u16 fc,u8 r);
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode);
void set_LCD_Char_scale(u16 scale);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u8 size,const char *p,u8 mode);
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, const char *s,u8 mode);
void GUI_DrawZikuFont16(u16 x, u16 y, u16 fc, u16 bc, const char *s,u8 mode);
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, const char *s,u8 mode);
void GUI_DrawZikuFont24(u16 x, u16 y, u16 fc, u16 bc, const char *s,u8 mode, int scale);
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, const char *s,u8 mode);
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, const char *str,u8 size,u8 mode);
const char* Show_Str_win_raw(u32 *xp, u32 *yp, u32 fc, u32 bc, const char *str, u32 size, u32 mode, win_pt wd, int);
void Show_Str_win(u32 x, u32 y, u32 fc, u32 bc, const char *str, u32 size, u32 mode, u32 win_width, u32 win_height);
void Gui_Drawbmp16(u16 x,u16 y,u16 w,u16 h,const unsigned char *p); //��ʾ40*40 QQͼƬ
void gui_circle(int xc, int yc,u16 c,int r, int fill);
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, char *str,u8 size,u8 mode);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
const char* area_show_str(win_pt wdp, u32 *xp, u32 *yp, const char*string, int);
#endif

