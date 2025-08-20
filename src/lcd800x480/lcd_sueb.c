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
#include "lcd_sueb.h"
#include "stdlib.h"
#include "common.h"	 
#include "test.h"	 

	   
#ifdef ALIENTEK_MINI
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
#endif

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//������ɫ,������ɫ
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

void Enable_BL(int en)//��������	 
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

void LCD_BUS_To_write(int write)
{
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
}

u16 LCD_read(void)
{
	u16 data;
	LCD_CS_CLR;
	LCD_RD_CLR;
#ifdef ALIENTEK_MINI
	delay_us(2);//��ʱ1us	
#endif
	data = DATAIN();
	LCD_RD_SET;
	LCD_CS_SET;
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
	LCD_WR_REG(LCD_Reg); 
	LCD_BUS_To_write(0);
	/*
	GPIOB->CRL=0X88888888; //PB0-7  ��������
	GPIOB->CRH=0X88888888; //PB8-15 ��������
	GPIOA->CRL|=0X00008800; //PA2-3 ��������
	GPIOB->ODR=0X0000;     //ȫ�������
	*/
	while(n--)
	{		
		*(Rval++) = LCD_RD_DATA();
	}
	LCD_BUS_To_write(1);
	
	/*
	GPIOB->CRL=0X33333333; 		//PB0-7  �������
	GPIOB->CRH=0X33333333; 		//PB8-15 �������
	GPIOA->CRL|=0X00003300; //PA2-3
	GPIOB->ODR=0XFFFF;    		//ȫ�������  
	*/
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
	u16 r,g,b;
	LCD_RS_SET;
	LCD_CS_CLR;
	
	//dummy data
	LCD_RD_CLR;
	delay_us(1);//��ʱ1us	
	r = DATAIN()>>8;
	LCD_RD_SET;
	
	//8bit:red data
	//16bit:red and green data
	LCD_RD_CLR;
	delay_us(1);//��ʱ1us	
	r = DATAIN()>>8;
	LCD_RD_SET;
	
	//8bit:green data
	//16bit:blue data
	LCD_RD_CLR;
	delay_us(1);//��ʱ1us	
	g = DATAIN()>>8;
	LCD_RD_SET;
	
	#if LCD_USE8BIT_MODEL	
	//blue data
	LCD_RD_CLR;
	delay_us(1);//��ʱ1us	
	b = DATAIN()>>8;
	LCD_RD_SET;
	#else
	b = g>>8;
	g = r&0xFF; 
	r = r>>8;	
	#endif
	LCD_CS_SET;
	return Color_To_565(r, g, b);
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
	LCD_SetCursor(x,y);//���ù��λ�� 
	Lcd_WriteData_16Bit(POINT_COLOR); 
}

#ifdef ALIENTEK_MINI
//���ٻ���
//x,y:����
//color:��ɫ
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
		if(lcddev.dir==1)x=lcddev.width-1-x;//����ʱ����
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		
	}else
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;//������ʵ���ǵ�תx,y����
		LCD_WriteReg(lcddev.setxcmd,x);
		LCD_WriteReg(lcddev.setycmd,y);
	}	
	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(lcddev.wramcmd);//дָ��  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	LCD_WR_DATA(color);		//д����
}

void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	if((lcddev.dir==1&&lcddev.id!=0X6804&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))//����ʱ����6804��1963���ı�ɨ�跽������ʱ1963�ı䷽��
	{			   
		switch(dir)//����ת��
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
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X1963)//9341/6804/5310/5510/1963,���⴦��
	{
		switch(dir)
		{
			case L2R_U2D://������,���ϵ���
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://������,���µ���
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://���ҵ���,���ϵ���
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://���ҵ���,���µ���
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://���ϵ���,������
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://���ϵ���,���ҵ���
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://���µ���,������
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://���µ���,���ҵ���
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510)&&(lcddev.id!=0X1963))regval|=0X08;//5310/5510/1963����ҪBGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804��BIT6��9341�ķ���	   
		LCD_WriteReg(dirreg,regval);
		if(lcddev.id!=0X1963)//1963�������괦��
		{
			if(regval&0X20)
			{
				if(lcddev.width<lcddev.height)//����X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}else  
			{
				if(lcddev.width>lcddev.height)//����X,Y
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
			case L2R_U2D://������,���ϵ���
				regval|=(1<<5)|(1<<4)|(0<<3); 
				break;
			case L2R_D2U://������,���µ���
				regval|=(0<<5)|(1<<4)|(0<<3); 
				break;
			case R2L_U2D://���ҵ���,���ϵ���
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://���ҵ���,���µ���
				regval|=(0<<5)|(0<<4)|(0<<3); 
				break;	 
			case U2D_L2R://���ϵ���,������
				regval|=(1<<5)|(1<<4)|(1<<3); 
				break;
			case U2D_R2L://���ϵ���,���ҵ���
				regval|=(1<<5)|(0<<4)|(1<<3); 
				break;
			case D2U_L2R://���µ���,������
				regval|=(0<<5)|(1<<4)|(1<<3); 
				break;
			case D2U_R2L://���µ���,���ҵ���
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
	if(dir==0)			//����
	{
		lcddev.dir=0;	//����
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
			lcddev.wramcmd=0X2C;	//����д��GRAM��ָ�� 
			lcddev.setxcmd=0X2B;	//����дX����ָ��
			lcddev.setycmd=0X2A;	//����дY����ָ��
			lcddev.width=480;		//���ÿ��480
			lcddev.height=800;		//���ø߶�800  
		}else
		{
			lcddev.wramcmd=0X22;
	 		lcddev.setxcmd=0X20;
			lcddev.setycmd=0X21;  
		}
	}else 				//����
	{	  				
		lcddev.dir=1;	//����
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
			lcddev.wramcmd=0X2C;	//����д��GRAM��ָ�� 
			lcddev.setxcmd=0X2A;	//����дX����ָ��
			lcddev.setycmd=0X2B;	//����дY����ָ��
			lcddev.width=800;		//���ÿ��800
			lcddev.height=480;		//���ø߶�480  
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
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}	 
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ�Ⱥ͸߶�,�������0!!
//�����С:width*height. 
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
	}else if(lcddev.id==0X1963)//1963�������⴦��
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
	}else	//��������IC
	{
		if(lcddev.dir==1)//����
		{
			//����ֵ
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
		hsareg=0X50;heareg=0X51;//ˮƽ���򴰿ڼĴ���
		vsareg=0X52;veareg=0X53;//��ֱ���򴰿ڼĴ���	   							  
		//���üĴ���ֵ
		LCD_WriteReg(hsareg,hsaval);
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);
		LCD_WriteReg(veareg,veaval);		
		LCD_SetCursor(sx,sy);	//���ù��λ��
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
	u16 color;
	if(x>=lcddev.width||y>=lcddev.height)
	{
		return 0;	//�����˷�Χ,ֱ�ӷ���	
	}
	LCD_SetCursor(x,y);//���ù��λ�� 
	LCD_ReadRAM_Prepare();
	LCD_BUS_To_write(0);
	
	color = Lcd_ReadData_16Bit();
	
	LCD_BUS_To_write(1);
	return color;
}

void color16_lcd(u16 color, u32 n);
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
	totalpoint*=h; 			//�õ��ܵ���
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804������ʱ�����⴦��  
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_SetCursor(0x00,0x0000);		//���ù��λ��  
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
LCD_Set_Window(xs,ys,w, h);
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	  	  
	for(index=0;index<totalpoint;index++)LCD_WR_DATA(color);	
#else
	LCD_SetWindows(xs,ys,xe,ye);   
    color16_lcd(color, ((unsigned int)xe-xs+1)*(ye-ys+1));
#if 0
	for(i=0;i<((unsigned int)xe-xs+1)*(ye-ys+1);i++)
	{
 //   for(m=0;m<lcddev.width;m++)
  //  {	
			Lcd_WriteData_16Bit(color);
	//	}
	}
#endif
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
	lcd_clr_window(Color, 0,0,lcddev.width-1,lcddev.height-1);
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
	GPIO_InitTypeDef  GPIO_InitStructure;
	      
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
#ifdef ALIENTEK_MINI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOC,GPIO_Pin_6 |GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14| GPIO_Pin_15; //GPIOC14,15,13
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_1| GPIO_Pin_0; //GPIOA8, 0, 1
	GPIO_Init(GPIOA, &GPIO_InitStructure); //GPIOA
	GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_1| GPIO_Pin_8);	
#endif

	LCD_BUS_To_write(1);
	lprintf("LCD gpio init done\n");
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
#ifdef ALIENTEK_MINI
#else
	LCD_RST_CLR;
	delay_ms(100);	
	LCD_RST_SET;
	delay_ms(50);
#endif
}

void LCD_hw_test(int testitem)
{
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
}

void lcd_sueb_basicinit()
{
	LCD_GPIOInit();//LCD GPIO��ʼ��
	delay_ms(100);
 	LCD_RESET(); //LCD ��λ
}

//PWM freq=72000/(0x9000)/2=976hz
#define BLPWM_ARR (0x9000-1)
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


    //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    TIM_TimeBaseStructure.TIM_Period = arr;//auto reloaded value
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

    TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��	

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	 

    TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
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
    //lprintf("v=%d\n", v);
    if(v>100){
        lprintf("err:BL v>100\n");
        return;
    }
    cur_bl_value = v;
    if(v>0){
        comp_v = 0x9000/100*(100-v);
        //lprintf("comp_v=%x\n", comp_v);
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
		LCD_WriteReg(0X03,0X1038);//�ı䷽���   
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
	LCD_Display_Dir(0);		 	//Ĭ��Ϊ����
	LCD_LED_SET;					//��������
#else
//************* OTM8009��ʼ��**********//	
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
  LCD_direction(USE_HORIZONTAL);//����LCD��ʾ����
    set_BL_value(DEFAULT_BL);//quater bright
	LCD_Clear(WHITE);//��ȫ����ɫ
#endif
}
 
void pure_lcd_sueb_test()
{
	//while(1)
	{
#if 0
		lprintf("test main interface.Any key continue...\n");
		main_test(); 		//����������
		//Test_Read();     //��ID����ɫֵ����
		lprintf("test simple fill color.Any key continue...\n");
		Test_Color();  		//��ˢ��������
		lprintf("test rect drawing.Any key continue...\n");
		Test_FillRec();		//GUI���λ�ͼ����
		lprintf("test circle drawing.Any key continue...\n");
		Test_Circle(); 		//GUI��Բ����
		lprintf("test trigangle drawing.Any key continue...\n");
		Test_Triangle();    //GUI�����λ�ͼ����
		lprintf("test english font.Any key continue...\n");
		English_Font_test();//Ӣ������ʾ������
		lprintf("test chinese font.Any key continue...\n");
		Chinese_Font_test();//��������ʾ������
		lprintf("test pic display.Any key continue...\n");
		Pic_test();			//ͼƬ��ʾʾ������
		lprintf("test rotate display.Any key continue...\n");
#endif
		Rotate_Test();   //��ת��ʾ����
		lprintf("test touch.\n");
		//����������������߲���Ҫ�������ܣ���ע�͵����津����������
		Touch_Test();		//��������д����		
	}
}

void lcd_sueb_test()
{
	//while(1)
	{
		lprintf("test touch.Any key continue...\n");
		con_recv();
		//����������������߲���Ҫ�������ܣ���ע�͵����津����������
		Touch_Test();		//��������д����		
		lprintf("test main interface.Any key continue...\n");
#if 0
		main_test(); 		//����������
		//Test_Read();     //��ID����ɫֵ����
		lprintf("test simple fill color.Any key continue...\n");
		con_recv();
		Test_Color();  		//��ˢ��������
		lprintf("test rect drawing.Any key continue...\n");
		con_recv();
		Test_FillRec();		//GUI���λ�ͼ����
		lprintf("test circle drawing.Any key continue...\n");
		con_recv();
		Test_Circle(); 		//GUI��Բ����
		lprintf("test trigangle drawing.Any key continue...\n");
		con_recv();
		Test_Triangle();    //GUI�����λ�ͼ����
		lprintf("test english font.Any key continue...\n");
		con_recv();
		English_Font_test();//Ӣ������ʾ������
		lprintf("test chinese font.Any key continue...\n");
		con_recv();
		Chinese_Font_test();//��������ʾ������
		lprintf("test pic display.Any key continue...\n");
		con_recv();
		Pic_test();			//ͼƬ��ʾʾ������
#endif
		lprintf("test rotate display.Any key continue...\n");
		con_recv();
		Rotate_Test();   //��ת��ʾ����
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

	LCD_WriteRAM_Prepare();	//��ʼд��GRAM			
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
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//����ʱ����
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 
	}else if(lcddev.id==0X1963)
	{  			 		
		if(lcddev.dir==0)//x������Ҫ�任
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
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//������ʵ���ǵ�תx,y����
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
