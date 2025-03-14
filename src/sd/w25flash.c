#include "w25flash.h" 
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//W25Q64 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/14 
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//升级说明
//********************************************************************************
//V1.1 20140314
//修改SPI_Flash_Write,以支持动态内存管理部分							  
////////////////////////////////////////////////////////////////////////////////// 	  
 
u16 SPI_FLASH_TYPE=W25Q64;//默认就是25Q64

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q64
//容量为8M字节,共有128个Block,2048个Sector 

//初始化SPI FLASH的IO口
#if 0
void SPI_Flash_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  //SPI CS
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
	SPI1_Init();		   //初始化SPI
	SPI1_SetSpeed(SPI_BaudRatePrescaler_4);	//设置为18M时钟,高速模式
	SPI_FLASH_TYPE=SPI_Flash_ReadID();//读取FLASH ID.
}
#endif

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SF_CS_LOW();                            //使能器件   
	SPI1_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPI1_ReadWriteByte(0Xff);             //读取一个字节  
	SF_CS_HIGH();                            //取消片选     
	return byte;   
} 
//写SPI_FLASH状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SF_CS_LOW();                            //使能器件   
	SPI1_ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
	SPI1_ReadWriteByte(sr);               //写入一个字节  
	SF_CS_HIGH();                            //取消片选     	      
}   
//SPI_FLASH写使能	
//将WEL置位   
void SPI_FLASH_Write_Enable(void)   
{
	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_WriteEnable);      //发送写使能  
	SF_CS_HIGH();                            //取消片选     	      
} 
//SPI_FLASH写禁止	
//将WEL清零  
void SPI_FLASH_Write_Disable(void)   
{  
	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令    
	SF_CS_HIGH();                            //取消片选     	      
} 			    
//读取芯片ID W25X16的ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SF_CS_LOW();				    
	SPI1_ReadWriteByte(0x90);//发送读取ID命令	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	SF_CS_HIGH();				    
    lprintf("W25Flash ID=%x\n", Temp);
    switch(Temp){
        case W25Q80:
            lprintf("W25Q80\n");
            break;
        case W25Q64:
            lprintf("W25Q64\n");
            break;
        case W25Q32:
            lprintf("W25Q32\n");
            break;
        case W25Q16:
            lprintf("W25Q16\n");
            break;
        default:
            lprintf("unknown ID\n");
    }
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_ReadData);         //发送读取命令   
    SPI1_ReadWriteByte((u8)((ReadAddr)>>16));  //发送24bit地址    
    SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI1_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI1_ReadWriteByte(0XFF);   //循环读数  
    }
	SF_CS_HIGH();                            //取消片选     	      
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void SPI_Flash_Write_Page(const u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_PageProgram);      //发送写页命令   
    SPI1_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
    SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI1_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//循环写数  
	SF_CS_HIGH();                            //取消片选 
	SPI_Flash_Wait_Busy();					   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(const u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
#ifdef WRITE_W25F
u8 SPI_FLASH_BUFFER[4096];
#endif

void SPI_Flash_Write_direct_erase(const u8* pBuffer,u32 WriteAddr,u32 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    lprintf("sfwde:%X  %X %x", pBuffer, WriteAddr, NumByteToWrite);
    secpos=WriteAddr/4096;//sector addr
    secoff=WriteAddr%4096;//offset inside sector
    secremain=4096-secoff;//left in sectore
    if(NumByteToWrite<=secremain){
        secremain=NumByteToWrite;
    }
    while(1)
    {
        lprintf("pos %x\n", secpos);
        if(0==secoff)//erase
        {
            lprintf("erase sector %x\n", WriteAddr);
            SPI_Flash_Erase_Sector(secpos);//erase sector
            lprintf("erase sector %x done\n", WriteAddr);
        }
        lprintf("write %x %x\n", WriteAddr, secremain);
        SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//
        lprintf("write %x %x done\n", WriteAddr, secremain);
        if(NumByteToWrite==secremain){
            break;//write end
        }
        else//not end yet
        {
            secpos++;
            secoff=0;
            pBuffer+=secremain;
            WriteAddr+=secremain;
            NumByteToWrite-=secremain;
            if(NumByteToWrite>4096){
                secremain=4096;	//next sector not end
            }
            else{
                secremain=NumByteToWrite;//end in next sector
            }
        }
    };
    lprintf("write_de end\n");
}

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)  		   
void SPI_Flash_Write(const u8* pBuffer,u32 WriteAddr,u32 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i; 
	u8 * SPI_FLASH_BUF;	  
    lprintf("sfw:%X  %X %x", pBuffer, WriteAddr, NumByteToWrite);
#ifdef MEM_ALLOC_TABLE_SIZE			
	SPI_FLASH_BUF=mymalloc(4096);	//使用内存管理 
	if(SPI_FLASH_BUF==NULL)return;	//申请失败
#else
#ifdef WRITE_W25F
	SPI_FLASH_BUF=SPI_FLASH_BUFFER;	//不使用内存管理
#else
    lprintf("SPI_Flash_Write:You can't use this func for lack of buffer\n");
    return;
#endif
#endif	 
	secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小    
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{
        lprintf("pos %x\n", secpos);
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
        lprintf("read done\n");
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
            lprintf("need erase sector\n");
			SPI_Flash_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
            lprintf("write sector\n");
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  

		}else{
            lprintf("write %x %x\n", WriteAddr, secremain);
            SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
        }
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
#ifdef MEM_ALLOC_TABLE_SIZE			
	myfree(SPI_FLASH_BUF);		//释放内存	 	 
#endif
    lprintf("write end\n");
}
//擦除整个芯片
//整片擦除时间:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//等待时间超长...
void SPI_Flash_Erase_Chip(void)   
{                                             
    lprintf("erase all chip\nwrite enable\n");
    SPI_FLASH_Write_Enable();                  //SET WEL 
    lprintf("waite busy...\n");
    SPI_Flash_Wait_Busy();   
    lprintf("done.\n");
    lprintf("sending erase cmd...\n");
  	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
	SF_CS_HIGH();                            //取消片选     	      
    lprintf("done.\nWaiting...\n");
	SPI_Flash_Wait_Busy();   				   //等待芯片擦除结束
    lprintf("done.\n");
}   
//擦除一个扇区
//Dst_Addr:扇区地址 0~2047 for W25Q64
//擦除一个山区的最少时间:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	lprintf("fe:%x\r\n",Dst_Addr);	
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI1_ReadWriteByte((u8)Dst_Addr);  
	SF_CS_HIGH();                            //取消片选     	      
    SPI_Flash_Wait_Busy();   				   //等待擦除完成
}  
//等待空闲
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_PowerDown);        //发送掉电命令  
	SF_CS_HIGH();                            //取消片选     	      
    delay_us(3);                               //等待TPD  
}   
//唤醒
void SPI_Flash_WAKEUP(void)   
{  
  	SF_CS_LOW();                            //使能器件   
    SPI1_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB    
	SF_CS_HIGH();                            //取消片选     	      
    delay_us(3);                               //等待TRES1
}   
u8 SPI_Flash_Read_Byte(u32 ReadAddr)
{
    u8 ret;
    SPI_Flash_Read(&ret,ReadAddr,1);
    return ret;
}

void SPI_Flash_Write_Byte(u8 data,u32 WriteAddr)
{
    SPI_Flash_Write_Page(&data,WriteAddr,1);
}


























