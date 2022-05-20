/**
  ******************************************************************************
  * @file    stm32_eval_spi_sd.c
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file provides a set of functions needed to manage the SPI SD 
  *          Card memory mounted on STM32xx-EVAL board (refer to stm32_eval.h
  *          to know about the boards supporting this memory). 
  *          It implements a high level communication layer for read and write 
  *          from/to this memory. The needed STM32 hardware resources (SPI and 
  *          GPIO) are defined in stm32xx_eval.h file, and the initialization is 
  *          performed in SD_LowLevel_Init() function declared in stm32xx_eval.c 
  *          file.
  *          You can easily tailor this driver to any other development board, 
  *          by just adapting the defines for hardware resources and 
  *          SD_LowLevel_Init() function.
  *            
  *          +-------------------------------------------------------+
  *          |                     Pin assignment                    |
  *          +-------------------------+---------------+-------------+
  *          |  STM32 SPI Pins         |     SD        |    Pin      |
  *          +-------------------------+---------------+-------------+
  *          | SD_SPI_CS_PIN           |   ChipSelect  |    1        |
  *          | SD_SPI_MOSI_PIN / MOSI  |   DataIn      |    2        |
  *          |                         |   GND         |    3 (0 V)  |
  *          |                         |   VDD         |    4 (3.3 V)|
  *          | SD_SPI_SCK_PIN / SCLK   |   Clock       |    5        |
  *          |                         |   GND         |    6 (0 V)  |
  *          | SD_SPI_MISO_PIN / MISO  |   DataOut     |    7        |
  *          +-------------------------+---------------+-------------+
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
#include "stm32_eval_spi_sd.h"
#include "common.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */
  
/** @addtogroup STM32_EVAL_SPI_SD
  * @brief      This file includes the SD card driver of STM32-EVAL boards.
  * @{
  */ 

/** @defgroup STM32_EVAL_SPI_SD_Private_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32_EVAL_SPI_SD_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_EVAL_SPI_SD_Private_Macros
  * @{
  */
/**
  * @}
  */ 
  

/** @defgroup STM32_EVAL_SPI_SD_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32_EVAL_SPI_SD_Private_Function_Prototypes
  * @{
  */
/**
  * @}
  */ 


/** @defgroup STM32_EVAL_SPI_SD_Private_Functions
  * @{
  */ 

uint32_t logv = 0;
uint8_t  SD_Type = 0;//no card
uint8_t SD_WaitReady(void);
void SD_DisSelect(void);

uint8_t SD_SPI_ReadWriteByte(uint8_t data)
{
	return SD_WriteByte(data);
}
//选择sd卡,并且等待卡准备OK
//返回值:0,成功;1,失败;
u8 SD_Select(void)
{
    lprintf("sd select\n");
	SD_CS_LOW();
	if(SD_WaitReady()==0)return 0;//等待成功
	SD_DisSelect();
	return 1;//等待失败
}

void SD_DisSelect(void)
{
    lprintf("sd deselect\n");
	SD_CS_HIGH();
 	SD_SPI_ReadWriteByte(0xff);//Ìá¹©¶îÍâµÄ8¸öÊ±ÖÓ
}

uint8_t SD_WaitReady(void)
{
    u32 retrys = 0x2FF;
    uint32_t t=0;
    do
    {
        if(SD_SPI_ReadWriteByte(0XFF)==0XFF){
            lprintf("rdy@%x\n", t);
            return 0;//OK
        }
        t++;
    }while(t<retrys);//
    lprintf("sms nord\n");
    return 0;
}

/**
  * @brief  Send 5 bytes command to the SD card.
  * @param  Cmd: The user expected command to send to SD card.
  * @param  Arg: The command argument.
  * @param  Crc: The CRC.
  * @retval None
  */
u8 SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
  u8 r1;
  u8 Retry=0; 
  uint32_t i = 0x00;
  SD_DisSelect();//取消上次片选
  if(SD_Select())return 0XFF;//片选失效 
  
  uint8_t Frame[6];

  Frame[0] = (Cmd | 0x40); /*!< Construct byte 1 */
  
  Frame[1] = (uint8_t)(Arg >> 24); /*!< Construct byte 2 */
  
  Frame[2] = (uint8_t)(Arg >> 16); /*!< Construct byte 3 */
  
  Frame[3] = (uint8_t)(Arg >> 8); /*!< Construct byte 4 */
  
  Frame[4] = (uint8_t)(Arg); /*!< Construct byte 5 */
  
  Frame[5] = (Crc); /*!< Construct CRC: byte 6 */
  
  for (i = 0; i < 6; i++)
  {
    SD_WriteByte(Frame[i]); /*!< Send the Cmd bytes */
  }
  if(Cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
  //等待响应，或超时退出
  Retry=0X1F;
  do
  {
      r1=SD_SPI_ReadWriteByte(0xFF);
  }while((r1&0X80) && Retry--);
  //返回状态值
  lprintf("SD_SendCmd %b %X %b\n", Cmd, Arg, Crc);
  return r1;
}

/**
  * @brief  Returns the SD response.
  * @param  None
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetResponse(uint8_t Response)
{
  uint32_t Count = 0xFFF;
  uint8_t res;
  //lprintf("%s:%d\n", __func__, __LINE__);

  /*!< Check if response is got or a timeout is happen */
  while (((res=SD_ReadByte()) != Response) && Count)
  {
    Count--;
  }
  if (Count == 0)
  {
    /*!< After time out */
    lprintf("%s:fail res %x\n", __func__, res);
    return SD_RESPONSE_FAILURE;
  }
  else
  {
    /*!< Right response got */
    //lprintf("%s:%d Noerr res %x\n", __func__, __LINE__, res);
    return SD_RESPONSE_NO_ERROR;
  }
}

//从sd卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
//返回值:0,成功;其他,失败;	
u8 SD_RecvData(u8*buf,u16 len)
{			  	  
	if(SD_GetResponse(0xFE))return 1;//等待SD卡发回数据起始令牌0xFE
    while(len--)//开始接收数据
    {
        *buf=SPI1_ReadWriteByte(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);									  					    
    return 0;//读取成功
}
//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）	    
//返回值:0：NO_ERR
//		 1：错误														   
u8 SD_GetCXD(u8 *csd_data, u8 cmd)
{
    u8 r1;	 
    r1=SD_SendCmd(cmd,0,0x01);//发CMD9命令，读CSD
    if(r1==0)
	{
        lprintf("get data\n");
    	r1=SD_RecvData(csd_data, 16);//接收16个字节的数据 
    }
	SD_DisSelect();//取消片选
	if(r1){
        lprintf("read %b fail\n", cmd);
        return 1;
    }
	else{
        lprintf("read %b OK\n", cmd);
        return 0;
    }
}  

void sf_read_id()
{
    uint16_t id;
	SD_CS_HIGH();
	SF_CS_HIGH();
	SF_CS_LOW();
	SD_WriteByte(0x90);
	SD_WriteByte(0x00);
	SD_WriteByte(0x00);
	SD_WriteByte(0x00);

    id = SD_ReadByte();
    id <<= 8;
    id |= SD_ReadByte();
    lprintf("id=%x\n", id);
	SF_CS_HIGH();
}

uint8_t SD_GetRes(void)
{
	uint16_t Retry=0X1fff;
	uint8_t r1;
	do
	{
		r1=SD_SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);	 
	//·µ»Ø×´Ì¬Öµ
	//lprintf("SD_GetRes %x Rty %x\n", r1, Retry);
	return r1;
}

uint8_t getres_SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
	//lprintf("cmd %x arg %x crc %x\n", Cmd, Arg, Crc);
	SD_SendCmd(Cmd, Arg, Crc);
	return SD_GetRes();
}

/**
  * @brief  DeInitializes the SD/SD communication.
  * @param  None
  * @retval None
  */
void SD_DeInit(void)
{
  SD_LowLevel_DeInit();
}

/**
  * @brief  Initializes the SD/SD communication.
  * @param  None
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_Init(void)
{
  uint32_t i = 0;
  SD_Error ret;

  lprintf("SD_LowLevel_Init()\n");
  /*!< Initialize SD_SPI */
  SD_LowLevel_Init(); 

  //lower spi speed in SD detect
  uint16_t speed = spi_speed(SPI_BaudRatePrescaler_256);

  /*!< SD chip select high */
  SD_CS_HIGH();

  lprintf("send SD dummy bytes\n");
  /*!< Send dummy byte 0xFF, 10 times with CS high */
  /*!< Rise CS and MOSI for 80 clocks cycles */
  for (i = 0; i <= 9; i++)
  {
    /*!< Send dummy byte 0xFF */
    SD_WriteByte(SD_DUMMY_BYTE);
  }
  /*------------Put SD in SPI mode--------------*/
  /*!< SD initialized and set to SPI mode properly */
  ret = SD_GoIdleState();
  //recover the spi speed
  spi_speed(speed);
  return ret;
}

SD_Error SD_get_type(void)
{
	uint8_t buf[4], r1;
	//uint32_t argument;
	int retry, i;
    if(1==SD_SendCmd(SD_CMD_SEND_IF_COND,0x1AA,0x87))
    {
        for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
        if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
        {
            retry=0XFFFE;
            do
            {
                SD_SendCmd(CMD55,0,0X01);	//发送CMD55
                r1=SD_SendCmd(CMD41,0x40000000,0X01);//发送CMD41
            }while(r1&&retry--);
            if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
            {
                for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);//得到OCR值
                if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //检查CCS
                else SD_Type=SD_TYPE_V2;   
            }
        }
    }else//SD V1.x/ MMC	V3
    {
        SD_SendCmd(CMD55,0,0X01);		//发送CMD55
        r1=SD_SendCmd(CMD41,0,0X01);	//发送CMD41
        if(r1<=1)
        {		
            SD_Type=SD_TYPE_V1;
            retry=0XFFFE;
            do //等待退出IDLE模式
            {
                SD_SendCmd(CMD55,0,0X01);	//发送CMD55
                r1=SD_SendCmd(CMD41,0,0X01);//发送CMD41
            }while(r1&&retry--);
        }else//MMC卡不支持CMD55+CMD41识别
        {
            SD_Type=SD_TYPE_MMC;//MMC V3
            retry=0XFFFE;
            do //等待退出IDLE模式
            {											    
                r1=SD_SendCmd(CMD1,0,0X01);//发送CMD1
            }while(r1&&retry--);  
        }
        if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//错误的卡
    }
    return SD_OK;
#if 0
	int retry;
	SD_SendCmd(SD_CMD_SEND_IF_COND,0x1AA,0x87);
	r1 = SD_GetRes();
        if (r1 == (SD_IN_IDLE_STATE))
	{//SD V2.0
		lprintf("SD v2.0\n");
		for(int i=0;i<4;i++)buf[i]=SD_ReadByte();	//Get trailing return value of R7 resp
		for(int i=0;i<4;i++)lprintf("buf[%d]=%x\n",i,buf[i]);
		if(buf[2]==0X01&&buf[3]==0XAA)//¿¨ÊÇ·ñÖ§³Ö2.7~3.6V
		{
			SD_CS_HIGH();
			SD_WriteByte(SD_DUMMY_BYTE);
			SD_CS_LOW();
			if(getres_SD_SendCmd(CMD58,0,0X01)==1)//
			{
                for(int i=0;i<4;i++)buf[i]=SD_ReadByte();//µÃµ½OCRÖµ
                lprintf("cmd58 return:\n");
                for(int i=0;i<4;i++)lprintf("buf[%d]=%x\n",i,buf[i]);
                if(buf[0]&0x40){
                    SD_Type=SD_TYPE_V2HC;    //¼ì²éCCS
                    lprintf("SD_TYPE_V2HC %b\n", SD_Type);
                }
                else{
                    SD_Type=SD_TYPE_V2;   
                    lprintf("SD_TYPE_V2\n");
                }
                //argument = 0x40000000 + (buf[1]<<16) + (buf[2]<<8);
				argument = 0x40ff8000;
				retry=100;
				do
				{
					//lprintf("55\n");
					getres_SD_SendCmd(CMD55,0,0X01);
					r1 = SD_ReadByte();
					//lprintf("41\n");
					r1 = getres_SD_SendCmd(CMD41,argument,1);//·¢ËÍCMD41
#if 0
					SD_SendCmd(SD_CMD_SEND_OP_COND, 0x00ffc000, 0xFF);
					r1 = SD_GetRes();
#endif
				lprintf("r1 %x rety %x\n", r1, retry);
				}while(r1&&retry--);
				lprintf("r1 %x rety %x\n", r1, retry);
			}
		}
	}else//SD V1.x/ MMC	V3
	{
		lprintf("SD v1.x/ MMC V3\n");
		SD_SendCmd(CMD55,0,0Xff);		//·¢ËÍCMD55
		r1 = SD_GetRes();
		SD_SendCmd(CMD41,0,0X01);	//·¢ËÍCMD41
		r1=SD_GetRes();
		if(r1<=1)
		{		
			SD_Type=SD_TYPE_V1;
			lprintf("SD_TYPE_V1\n");
			retry=0XFFFE;
			do //µÈ´ýÍË³öIDLEÄ£Ê½
			{
				SD_SendCmd(CMD55,0,0X01);	//·¢ËÍCMD55
				SD_SendCmd(CMD41,0,0X01);//·¢ËÍCMD41
				r1=SD_GetRes();
			}while(r1&&retry--);
		}else//MMC¿¨²»Ö§³ÖCMD55+CMD41Ê¶±ð
		{
			SD_Type=SD_TYPE_MMC;//MMC V3
			lprintf("SD_TYPE_MMC\n");
			retry=0XFFFE;
			do //µÈ´ýÍË³öIDLEÄ£Ê½
			{											    
				SD_SendCmd(CMD1,0,0Xff);//·¢ËÍCMD1
				r1=SD_GetRes();
			}while(r1&&retry--);  
		}
		if(retry==0||getres_SD_SendCmd(CMD16,512,0X01)!=0){
			lprintf("SD_TYPE_ERR\n");
			SD_Type=SD_TYPE_ERR;//´íÎóµÄ¿¨
		}
	}
	SD_DisSelect();//È¡ÏûÆ¬Ñ¡
	//SD_SPI_SpeedHigh();//¸ßËÙ
    lprintf("SD_Type = 0x%b\n", SD_Type);
	if(SD_Type)return SD_RESPONSE_NO_ERROR;
	else return SD_RESPONSE_FAILURE;//ÆäËû´íÎó
#endif
}

/**
 * @brief  Detect if SD card is correctly plugged in the memory slot.
 * @param  None
 * @retval Return if SD is detected or not
 */
uint8_t SD_Detect(void)
{
  __IO uint8_t status = SD_PRESENT;

#ifndef NO_SD_DETECT
  /*!< Check GPIO to detect SD */
  if (GPIO_ReadInputData(SD_DETECT_GPIO_PORT) & SD_DETECT_PIN)
  {
    status = SD_NOT_PRESENT;
  }
#endif
  return status;
}

/**
  * @brief  Returns information about specific card.
  * @param  cardinfo: pointer to a SD_CardInfo structure that contains all SD 
  *         card information.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
  SD_Error status = SD_RESPONSE_FAILURE;

  status = SD_GetCSDRegister(&(cardinfo->SD_csd));
  status = SD_GetCIDRegister(&(cardinfo->SD_cid));
  cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
  cardinfo->CardCapacity = SD_GetSectorCount();

  /*!< Returns the reponse */
  return status;
}



//读SD卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
    lprintf("R sec %x, n %d\n", sector, cnt);
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//转换为字节地址
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//读命令
		if(r1==0)//指令发送成功
		{
			r1=SD_RecvData(buf,512);//接收512个字节	   
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//连续读命令
		do
		{
			r1=SD_RecvData(buf,512);//接收512个字节	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//发送停止命令
	}   
	SD_DisSelect();//取消片选
	return r1;//
}

u32 SD_GetSectorCount(void)
{
#if 0
    u8 csd[16];
    u32 Capacity;  
    u8 n, i;
    u16 csize;                          


    //取CSD信息，如果期间出错，返回0
    /*!< SD chip select low */
    SD_CS_LOW();
    /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
    SD_SendCmd(SD_CMD_SEND_CSD, 0, 0xFF);
    /*!< Wait for response in the R1 format (0x00 is no errors) */
    if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
    {
        if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
        {
            for (i = 0; i < 16; i++)
            {
                /*!< Store CSD register value on csd */
                csd[i] = SD_ReadByte();
            }
        }
        else{
            lprintf("sd Respose error when read block\n");
            return 0;
        }
        /*!< Get CRC bytes (not really needed by us, but required by SD) */
        SD_WriteByte(SD_DUMMY_BYTE);
        SD_WriteByte(SD_DUMMY_BYTE);
        /*!< Set response value to success */
    }
    else{
        lprintf("sd Respose error when read csd\n");
        return 0;
    }
    /*!< SD chip select high */
    SD_CS_HIGH();
    /*!< Send dummy byte: 8 Clock pulses of delay */
    SD_WriteByte(SD_DUMMY_BYTE);

    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)  //V2.00的卡
    {   
        csize = csd[9] + ((u16)csd[8] << 8) + 1;
        Capacity = (u32)csize << 10;//得到扇区数               
        if( SD_Type!=SD_TYPE_V2HC){
            lprintf("SD Type %x -> %x\n", SD_Type, SD_TYPE_V2HC);
            SD_Type=SD_TYPE_V2HC;
        }
    }else//V1.XX的卡
    {   
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        Capacity= (u32)csize << (n - 9);//得到扇区数   
    }
	lprintf("sd capacity %d\n", Capacity);
    return Capacity;
#endif
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//取CSD信息，如果期间出错，返回0
    if(SD_GetCXD(csd, CMD9)!=0) return 0;	    
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)	 //V2.00的卡
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//得到扇区数	 		   
    }else//V1.XX的卡
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//得到扇区数   
    }
    return Capacity;
}

/**
  * @brief  Reads a block of data from the SD.
  * @param  pBuffer: pointer to the buffer that receives the data read from the 
  *                  SD.
  * @param  ReadAddr: SD's internal address to read from.
  * @param  BlockSize: the SD card Data block size.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_ReadBlock(uint8_t* pBuffer, uint32_t ReadBlockNo, uint16_t Size)
{
    lprintf("R addr %X, len %d\n", ReadBlockNo, (u32)Size);
    return SD_ReadDisk(pBuffer, ReadBlockNo, Size>>9);
#if 0
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  //lprintf("SRB:A-%x l-%d\n", ReadAddr, (uint32_t)BlockSize);
  if( SD_Type==SD_TYPE_V2HC){
      ReadAddr>>=9;
  }
  if(logv){
      lprintf("%x", SD_Type);
      lprintf("R%x ", (uint32_t)ReadAddr);
  }

  /*!< SD chip select low */
  SD_CS_LOW();
  
  /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
  SD_SendCmd(SD_CMD_READ_SINGLE_BLOCK, ReadAddr, 0xFF);
  
  /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Read the SD block data : read NumByteToRead data */
      for (i = 0; i < BlockSize; i++)
      {
        /*!< Save the received data */
        *pBuffer = SD_ReadByte();
       
        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /*!< Get CRC bytes (not really needed by us, but required by SD) */
      SD_ReadByte();
      SD_ReadByte();
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
  
  if(rvalue == SD_RESPONSE_NO_ERROR){
      if(logv){
          lprintf("BRO\n");
      }
  }
  else{
      lprintf("BRF\n");
  }
  /*!< Returns the reponse */
  return rvalue;
#endif
}

//向sd卡写入一个数据包的内容 512字节
//buf:数据缓存区
//cmd:指令
//返回值:0,成功;其他,失败;	
u8 SD_SendBlock(u8*buf,u8 cmd)
{	
	u16 t;		  	  
	if(SD_WaitReady())return 1;//等待准备失效
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//不是结束指令
	{
		for(t=0;t<512;t++)SPI1_ReadWriteByte(buf[t]);//提高速度,减少函数传参时间
	    SD_SPI_ReadWriteByte(0xFF);//忽略crc
	    SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//接收响应
		if((t&0x1F)!=0x05)return 2;//响应错误									  					    
	}						 									  					    
    return 0;//写入成功
}
//写SD卡
//buf:数据缓存区
//sector:起始扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
    lprintf("W sec %x, n %d\n", sector, cnt);
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//转换为字节地址
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);
		if(r1==0)//指令发送成功
		{
			r1=SD_SendBlock(buf,0xFE);//写512个字节	   
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//发送指令	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);//接收512个字节 
		}
	}   
	SD_DisSelect();//取消片选
	return r1;//
}	

/**
  * @brief  Reads multiple block of data from the SD.
  * @param  pBuffer: pointer to the buffer that receives the data read from the 
  *                  SD.
  * @param  ReadAddr: SD's internal address to read from.
  * @param  BlockSize: the SD card Data block size.
  * @param  NumberOfBlocks: number of blocks to be read.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_ReadMultiBlocks(uint8_t* pBuffer, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  uint32_t i = 0, Offset = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  
  if( SD_Type==SD_TYPE_V2HC){
      ReadAddr>>=9;
  }
  /*!< SD chip select low */
  SD_CS_LOW();
  /*!< Data transfer */
  while (NumberOfBlocks--)
  {
    /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
    SD_SendCmd (SD_CMD_READ_SINGLE_BLOCK, ReadAddr + Offset, 0xFF);
    /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
    if (SD_GetResponse(SD_RESPONSE_NO_ERROR))
    {
      return  SD_RESPONSE_FAILURE;
    }
    /*!< Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Read the SD block data : read NumByteToRead data */
      for (i = 0; i < BlockSize; i++)
      {
        /*!< Read the pointed data */
        *pBuffer = SD_ReadByte();
        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /*!< Set next read address*/
      Offset += 512;
      /*!< get CRC bytes (not really needed by us, but required by SD) */
      SD_ReadByte();
      SD_ReadByte();
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
    else
    {
      /*!< Set response value to failure */
      rvalue = SD_RESPONSE_FAILURE;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
  /*!< Returns the reponse */
  return rvalue;
}

/**
  * @brief  Writes a block on the SD
  * @param  pBuffer: pointer to the buffer containing the data to be written on 
  *                  the SD.
  * @param  WriteAddr: address to write on.
  * @param  BlockSize: the SD card Data block size.
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_WriteBlock(uint8_t* w_buf, uint32_t WriteBlockNo, uint16_t Size)
{
    lprintf("W addr %X, len %d\n", WriteBlockNo, (u32)Size);
    return SD_WriteDisk(w_buf, WriteBlockNo, Size>>9);
#if 0
  uint32_t i = 0, rtry = 5;
  uint8_t * pBuffer = w_buf, r1;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  if( SD_Type==SD_TYPE_V2HC){
      WriteAddr>>=9;
  }
  if(logv){
      if( SD_Type==SD_TYPE_V2HC){
          lprintf(">");
      }
      lprintf("W%x ", (uint32_t)WriteAddr);
  }

  do{
      pBuffer = w_buf;
      /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write multiple block */
      //SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr, 0xFF);
      r1 = getres_SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr, 0xFF);

      /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
      if (r1 == SD_RESPONSE_NO_ERROR)
      {
          /*!< Send a dummy byte */
          SD_WriteByte(SD_DUMMY_BYTE);

          if(!SD_WaitReady()){
              /*!< Send the data token to signify the start of the data */
              SD_WriteByte(0xFE);

              /*!< Write the block data to SD : write count data by block */
              for (i = 0; i < BlockSize; i++)
              {
                  //lprintf("%d:%b\n", i, (uint8_t)*pBuffer);
                  /*!< Send the pointed byte */
                  SD_WriteByte(*pBuffer);
                  /*!< Point to the next location where the byte read will be saved */
                  pBuffer++;
              }
              /*!< Put CRC bytes (not really needed by us, but required by SD) */
              SD_ReadByte();
              SD_ReadByte();

              /*!< Read data response */
              if (SD_GetDataResponse() == SD_DATA_OK)
              {
                  rvalue = SD_RESPONSE_NO_ERROR;
              }
              else{
                  if(logv){
                      lprintf("err2 %x rty %x\n", SD_GetStatus(), rtry);
                  }
              }
          }
          else{
              lprintf("wait ready timeout %x\n", rtry);
          }
      }
      else{
          if(logv){
              lprintf("err1 %x rty %x\n", SD_GetStatus(), rtry);
          }
      }
  }
  while(rvalue != SD_RESPONSE_NO_ERROR && rtry--);
  if(rvalue == SD_RESPONSE_NO_ERROR){
      if(logv){
          lprintf("BWO\n");
      }
  }
  else{
      lprintf("BWF\n");
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Returns the reponse */
  return rvalue;
#endif
}

/**
  * @brief  Writes many blocks on the SD
  * @param  pBuffer: pointer to the buffer containing the data to be written on 
  *                  the SD.
  * @param  WriteAddr: address to write on.
  * @param  BlockSize: the SD card Data block size.
  * @param  NumberOfBlocks: number of blocks to be written.
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_WriteMultiBlocks(uint8_t* pBuffer, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  uint32_t i = 0, Offset = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;

  if( SD_Type==SD_TYPE_V2HC){
      WriteAddr>>=9;
  }
  /*!< Data transfer */
  while (NumberOfBlocks--)
  {
    /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write blocks */
    SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr + Offset, 0xFF);
    /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
    if (SD_GetResponse(SD_RESPONSE_NO_ERROR))
    {
      return SD_RESPONSE_FAILURE;
    }
    /*!< Send dummy byte */
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Send the data token to signify the start of the data */
    SD_WriteByte(SD_START_DATA_SINGLE_BLOCK_WRITE);
    /*!< Write the block data to SD : write count data by block */
    for (i = 0; i < BlockSize; i++)
    {
      /*!< Send the pointed byte */
      SD_WriteByte(*pBuffer);
      /*!< Point to the next location where the byte read will be saved */
      pBuffer++;
    }
    /*!< Set next write address */
    Offset += 512;
    /*!< Put CRC bytes (not really needed by us, but required by SD) */
    SD_ReadByte();
    SD_ReadByte();
    /*!< Read data response */
    if (SD_GetDataResponse() == SD_DATA_OK)
    {
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
    else
    {
      /*!< Set response value to failure */
      rvalue = SD_RESPONSE_FAILURE;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
  /*!< Returns the reponse */
  return rvalue;
}

/**
  * @brief  Read the CSD card register.
  *         Reading the contents of the CSD register in SPI mode is a simple 
  *         read-block transaction.
  * @param  SD_csd: pointer on an SCD register structure
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetCSDRegister(SD_CSD* SD_csd)
{
  //uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  uint8_t CSD_Tab[16];

  if(SD_GetCXD(CSD_Tab, CMD9)!=0) return rvalue;	    
#if 0
  /*!< SD chip select low */
  SD_CS_LOW();
  /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
  SD_SendCmd(SD_CMD_SEND_CSD, 0, 0xFF);
  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      for (i = 0; i < 16; i++)
      {
        /*!< Store CSD register value on CSD_Tab */
        CSD_Tab[i] = SD_ReadByte();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    SD_WriteByte(SD_DUMMY_BYTE);
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
#endif

  /*!< Byte 0 */
  SD_csd->CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
  SD_csd->SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
  SD_csd->Reserved1 = CSD_Tab[0] & 0x03;

  /*!< Byte 1 */
  SD_csd->TAAC = CSD_Tab[1];

  /*!< Byte 2 */
  SD_csd->NSAC = CSD_Tab[2];

  /*!< Byte 3 */
  SD_csd->MaxBusClkFrec = CSD_Tab[3];

  /*!< Byte 4 */
  SD_csd->CardComdClasses = CSD_Tab[4] << 4;

  /*!< Byte 5 */
  SD_csd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
  SD_csd->RdBlockLen = CSD_Tab[5] & 0x0F;

  /*!< Byte 6 */
  SD_csd->PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
  SD_csd->WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
  SD_csd->RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
  SD_csd->DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
  SD_csd->Reserved2 = 0; /*!< Reserved */

  SD_csd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;

  /*!< Byte 7 */
  SD_csd->DeviceSize |= (CSD_Tab[7]) << 2;

  /*!< Byte 8 */
  SD_csd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;

  SD_csd->MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
  SD_csd->MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);

  /*!< Byte 9 */
  SD_csd->MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
  SD_csd->MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
  SD_csd->DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;
  /*!< Byte 10 */
  SD_csd->DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;
    
  SD_csd->EraseGrSize = (CSD_Tab[10] & 0x40) >> 6;
  SD_csd->EraseGrMul = (CSD_Tab[10] & 0x3F) << 1;

  /*!< Byte 11 */
  SD_csd->EraseGrMul |= (CSD_Tab[11] & 0x80) >> 7;
  SD_csd->WrProtectGrSize = (CSD_Tab[11] & 0x7F);

  /*!< Byte 12 */
  SD_csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
  SD_csd->ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
  SD_csd->WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
  SD_csd->MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;

  /*!< Byte 13 */
  SD_csd->MaxWrBlockLen |= (CSD_Tab[13] & 0xC0) >> 6;
  SD_csd->WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
  SD_csd->Reserved3 = 0;
  SD_csd->ContentProtectAppli = (CSD_Tab[13] & 0x01);

  /*!< Byte 14 */
  SD_csd->FileFormatGrouop = (CSD_Tab[14] & 0x80) >> 7;
  SD_csd->CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
  SD_csd->PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
  SD_csd->TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
  SD_csd->FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
  SD_csd->ECC = (CSD_Tab[14] & 0x03);

  /*!< Byte 15 */
  SD_csd->CSD_CRC = (CSD_Tab[15] & 0xFE) >> 1;
  SD_csd->Reserved4 = 1;

  /*!< Return the reponse */
  return SD_OK;
}

/**
  * @brief  Read the CID card register.
  *         Reading the contents of the CID register in SPI mode is a simple 
  *         read-block transaction.
  * @param  SD_cid: pointer on an CID register structure
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetCIDRegister(SD_CID* SD_cid)
{
  //u8 r1;
  //uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  uint8_t CID_Tab[16];
 
  if(0!=SD_GetCXD(CID_Tab, SD_CMD_SEND_CID)) return rvalue;	    
#if 0
  /*!< SD chip select low */
  SD_CS_LOW();
  
  /*!< Send CMD10 (CID register) */
  SD_SendCmd(SD_CMD_SEND_CID, 0, 0xFF);
  
  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Store CID register value on CID_Tab */
      for (i = 0; i < 16; i++)
      {
        CID_Tab[i] = SD_ReadByte();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    SD_WriteByte(SD_DUMMY_BYTE);
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
#endif

  /*!< Byte 0 */
  SD_cid->ManufacturerID = CID_Tab[0];

  /*!< Byte 1 */
  SD_cid->OEM_AppliID = CID_Tab[1] << 8;

  /*!< Byte 2 */
  SD_cid->OEM_AppliID |= CID_Tab[2];

  /*!< Byte 3 */
  SD_cid->ProdName1 = CID_Tab[3] << 24;

  /*!< Byte 4 */
  SD_cid->ProdName1 |= CID_Tab[4] << 16;

  /*!< Byte 5 */
  SD_cid->ProdName1 |= CID_Tab[5] << 8;

  /*!< Byte 6 */
  SD_cid->ProdName1 |= CID_Tab[6];

  /*!< Byte 7 */
  SD_cid->ProdName2 = CID_Tab[7];

  /*!< Byte 8 */
  SD_cid->ProdRev = CID_Tab[8];

  /*!< Byte 9 */
  SD_cid->ProdSN = CID_Tab[9] << 24;

  /*!< Byte 10 */
  SD_cid->ProdSN |= CID_Tab[10] << 16;

  /*!< Byte 11 */
  SD_cid->ProdSN |= CID_Tab[11] << 8;

  /*!< Byte 12 */
  SD_cid->ProdSN |= CID_Tab[12];

  /*!< Byte 13 */
  SD_cid->Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
  SD_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;

  /*!< Byte 14 */
  SD_cid->ManufactDate |= CID_Tab[14];

  /*!< Byte 15 */
  SD_cid->CID_CRC = (CID_Tab[15] & 0xFE) >> 1;
  SD_cid->Reserved2 = 1;

  /*!< Return the reponse */
  return SD_OK;
}

/**
  * @brief  Send 5 bytes command to the SD card.
  * @param  Cmd: The user expected command to send to SD card.
  * @param  Arg: The command argument.
  * @param  Crc: The CRC.
  * @retval None
  */
#if 0
void SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
  uint32_t i = 0x00;
  
  uint8_t Frame[6];

  Frame[0] = (Cmd | 0x40); /*!< Construct byte 1 */
  
  Frame[1] = (uint8_t)(Arg >> 24); /*!< Construct byte 2 */
  
  Frame[2] = (uint8_t)(Arg >> 16); /*!< Construct byte 3 */
  
  Frame[3] = (uint8_t)(Arg >> 8); /*!< Construct byte 4 */
  
  Frame[4] = (uint8_t)(Arg); /*!< Construct byte 5 */
  
  Frame[5] = (Crc); /*!< Construct CRC: byte 6 */
  
  for (i = 0; i < 6; i++)
  {
    SD_WriteByte(Frame[i]); /*!< Send the Cmd bytes */
  }
  if(Cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
}
#endif

/**
  * @brief  Get SD card data response.
  * @param  None
  * @retval The SD status: Read data response xxx0<status>1
  *         - status 010: Data accecpted
  *         - status 101: Data rejected due to a crc error
  *         - status 110: Data rejected due to a Write error.
  *         - status 111: Data rejected due to other error.
  */
uint8_t SD_GetDataResponse(void)
{
  uint32_t i = 0;
  uint8_t response, rvalue;

  while (i <= 64)
  {
    /*!< Read resonse */
    response = SD_ReadByte();
    /*!< Mask unused bits */
    response &= 0x1F;
    switch (response)
    {
      case SD_DATA_OK:
      {
        rvalue = SD_DATA_OK;
        break;
      }
      case SD_DATA_CRC_ERROR:
        return SD_DATA_CRC_ERROR;
      case SD_DATA_WRITE_ERROR:
        return SD_DATA_WRITE_ERROR;
      default:
      {
        rvalue = SD_DATA_OTHER_ERROR;
        break;
      }
    }
    /*!< Exit loop in case of data ok */
    if (rvalue == SD_DATA_OK)
      break;
    /*!< Increment loop counter */
    i++;
  }

  /*!< Wait null data */
  while (SD_ReadByte() == 0);

  /*!< Return response */
  return response;
}

/**
  * @brief  Returns the SD response.
  * @param  None
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
#if 0
SD_Error SD_GetResponse(uint8_t Response)
{
  uint32_t Count = 0xFFF;
  uint8_t res;
  //lprintf("%s:%d\n", __func__, __LINE__);

  /*!< Check if response is got or a timeout is happen */
  while (((res=SD_ReadByte()) != Response) && Count)
  {
    Count--;
  }
  if (Count == 0)
  {
    /*!< After time out */
    lprintf("%s:fail res %x\n", __func__, res);
    return SD_RESPONSE_FAILURE;
  }
  else
  {
    /*!< Right response got */
    //lprintf("%s:%d Noerr res %x\n", __func__, __LINE__, res);
    return SD_RESPONSE_NO_ERROR;
  }
}
#endif

/**
  * @brief  Returns the SD status.
  * @param  None
  * @retval The SD status.
  */
uint16_t SD_GetStatus(void)
{
  uint16_t Status = 0;

  /*!< Send CMD13 (SD_SEND_STATUS) to get SD status */
  SD_SendCmd(SD_CMD_SEND_STATUS, 0, 0xFF);

  Status = SD_ReadByte();
  Status |= (uint16_t)(SD_ReadByte() << 8);

  /*!< SD chip select high */
  SD_CS_HIGH();

  /*!< Send dummy byte 0xFF */
  SD_WriteByte(SD_DUMMY_BYTE);

  return Status;
}

//SPI硬件层初始化
/**
  * @brief  Put SD in Idle state.
  * @param  None
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GoIdleState(void)
{
	uint8_t r1;
    //SD_Error ret;
	int retry = 100;
  lprintf("%s:%d\n", __func__, __LINE__);
  
  do{
  /*!< Send CMD0 (SD_CMD_GO_IDLE_STATE) with CS low to put SD in SPI mode */
    r1=SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);
  }while((r1!=0X01) && retry--);
  SD_Type=0;//默认无卡
  if(r1==0x01){
    SD_get_type();
  }
  lprintf("sdtype %b\n", SD_Type);
  SD_DisSelect();//取消片选
  if(SD_Type)return 0;
  else if(r1)return r1; 	   
  lprintf("%s:%d\n", __func__, __LINE__);
  return 0xaa;//其他错误
#if 0
	//int retry = 100;
  lprintf("%s:%d\n", __func__, __LINE__);
  /*!< SD chip select low */
  SD_CS_LOW();
  
  /*!< Send CMD0 (SD_CMD_GO_IDLE_STATE) with CS low to put SD in SPI mode */
  SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);
  r1 = SD_GetRes();
  lprintf("get res %b\n", r1);
  if (r1 != (SD_IN_IDLE_STATE))
  {
    /*!< No Idle State Response: return response failue */
    lprintf("%s:%d\n", __func__, __LINE__);
    SD_CS_HIGH();
    return SD_RESPONSE_FAILURE;
  }
  
#if 0
  /*!< Wait for In Idle State Response (R1 Format) equal to 0x01 */
  if (SD_GetResponse(SD_IN_IDLE_STATE))
  {
    /*!< No Idle State Response: return response failue */
    lprintf("%s:%d\n", __func__, __LINE__);
    return SD_RESPONSE_FAILURE;
  }
#endif
  lprintf("%s:%d IDLE OK\n", __func__, __LINE__);
  
    SD_CS_HIGH();
    SD_WriteByte(SD_DUMMY_BYTE);
    //reset complete

    //init
    SD_CS_LOW();
#if 0
    do{
    SD_SendCmd(55, 0, 0xFF);
    r1 = SD_GetRes();
    if(r1 == 0){
	    lprintf("init OK\n");
    }
    else{
	    SD_SendCmd(CMD41,0x40300000,0Xff);//·¢ËÍCMD41
	    r1 = SD_GetRes();
    }
    retry--;
    }while(r1 && retry);
    lprintf("r1 %x retry %d\n", r1, retry);

    SD_CS_HIGH();
    SD_WriteByte(SD_DUMMY_BYTE);
    //reset complete

    //init
    SD_CS_LOW();
    do{
    SD_SendCmd(55, 0, 0xFF);
    r1 = SD_GetRes();
    if(r1 == 0){
	    lprintf("init OK\n");
    }
    else{
	    SD_SendCmd(CMD41,0x00300000,0Xff);//·¢ËÍCMD41
	    r1 = SD_GetRes();
    }
    retry--;
    }while(r1 && retry);
    lprintf("r1 %x retry %d\n", r1, retry);
#endif
#if 0
  /*----------Activates the card initialization process-----------*/
  do
  {
    /*!< SD chip select high */
    SD_CS_HIGH();
    
    /*!< Send Dummy byte 0xFF */
    SD_WriteByte(SD_DUMMY_BYTE);
    
    /*!< SD chip select low */
    SD_CS_LOW();
    
    /*!< Send CMD1 (Activates the card process) until response equal to 0x0 */
    SD_SendCmd(SD_CMD_SEND_OP_COND, 0, 0xFF);
    /*!< Wait for no error Response (R1 Format) equal to 0x00 */
  }
  while (SD_GetResponse(SD_RESPONSE_NO_ERROR));
  
#endif
  ret = SD_get_type();
  /*!< SD chip select high */
  SD_CS_HIGH();
  
  /*!< Send dummy byte 0xFF */
  SD_WriteByte(SD_DUMMY_BYTE);
  
  lprintf("%s:%d\n", __func__, __LINE__);
  return ret;
#endif
}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
