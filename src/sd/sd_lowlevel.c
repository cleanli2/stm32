#include "stm32_eval_spi_sd.h"
#include "sd_lowlevel.h"
#include "common.h"

#define STM32_SPI_INITED 0x55
#define GPIO_SPI_INITED 0xaa
#define NO_SPI_INITED 0
static uint32_t stm32_spi_choose = 1;
static uint32_t spi_inited = NO_SPI_INITED;
/**
  * @brief  DeInitializes the SD/SD communication.
  * @param  None
  * @retval None
  */
void stm32_spi_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */
  SPI_I2S_DeInit(SD_SPI);   /*!< DeInitializes the SD_SPI */
  
  /*!< SD_SPI Periph clock disable */
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, DISABLE); 

  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
  spi_inited = NO_SPI_INITED;
}

/**
  * @brief  Initializes the SD_SPI and CS pins.
  * @param  None
  * @retval None
  */
void stm32_spi_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

  lprintf("%s spi_inited %x\n", __func__, spi_inited);
  if(spi_inited == STM32_SPI_INITED){
      return;
  }
  /*!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO 
       and SD_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(SD_CS_GPIO_CLK | SD_SPI_MOSI_GPIO_CLK | SD_SPI_MISO_GPIO_CLK |
                         SD_SPI_SCK_GPIO_CLK | SD_DETECT_GPIO_CLK, ENABLE);

  /*!< SD_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE); 

  
  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SF_SPI_CS_PIN pin: SPI FLASH CS pin */
  GPIO_InitStructure.GPIO_Pin = SF_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SF_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

  /*!< SD_SPI Config */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SD_SPI, &SPI_InitStructure);
  
  SPI_Cmd(SD_SPI, ENABLE); /*!< SD_SPI enable */
  spi_inited = STM32_SPI_INITED;
}

/**
  * @brief  Write a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
uint8_t stm32_spi_WriteByte(uint8_t Data)
{
  /*!< Wait until the transmit buffer is empty */
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
  }
  
  /*!< Send the byte */
  SPI_I2S_SendData(SD_SPI, Data);
  
  /*!< Wait to receive a byte*/
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  
  /*!< Return the byte read from the SPI bus */ 
  return SPI_I2S_ReceiveData(SD_SPI);
}

/**
  * @brief  Read a byte from the SD.
  * @param  None
  * @retval The received byte.
  */
uint8_t stm32_spi_ReadByte(void)
{
  uint8_t Data = 0;
  
  /*!< Wait until the transmit buffer is empty */
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
  }
  /*!< Send the byte */
  SPI_I2S_SendData(SD_SPI, SD_DUMMY_BYTE);

  /*!< Wait until a data is received */
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  /*!< Get the received data */
  Data = SPI_I2S_ReceiveData(SD_SPI);

  /*!< Return the shifted data */
  return Data;
}

/*******************gpio spi**************************************************/
void gpio_spi_LowLevel_DeInit(void)
{
    spi_inited = NO_SPI_INITED;
}

void gpio_spi_LowLevel_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO

    lprintf("%s spi_inited %x\n", __func__, spi_inited);
    if(spi_inited == GPIO_SPI_INITED){
        return;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_5);

    GPIO_InitStructure.GPIO_Pin = DOUT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //上拉输入
    GPIO_Init(DOUT_GG, &GPIO_InitStructure);
    GPIO_SetBits(DOUT_GG, DOUT_PIN);	
    spi_inited = GPIO_SPI_INITED;
}

uint8_t gpio_spi_WriteByte(uint8_t num)
{
    u8 count=0, ret=0;   
    //lprintf("TPw:%x\n", num);
    for(count=0;count<8;count++)  
    { 	  
        ret<<=1; 	 
        if(num&0x80){
            TDIN(1);  
        }
        else{
            TDIN(0);   
        }
        num<<=1;    
        TCLK(0); 	 
        delay_us(10);
        TCLK(1);		//上升沿有效	        
        delay_us(10);
        if(DOUT)ret++; 		 
    }		 			    
    return ret;
}

uint8_t gpio_spi_ReadByte(void)
{
    return gpio_spi_WriteByte(SD_DUMMY_BYTE);
}
/*********************************************************************/
void SD_LowLevel_DeInit(void)
{
    if(stm32_spi_choose){
        stm32_spi_LowLevel_DeInit();
    }
    else{
        gpio_spi_LowLevel_DeInit();
    }
}
void SD_LowLevel_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
    if(stm32_spi_choose){
        stm32_spi_LowLevel_Init();
    }
    else{
        gpio_spi_LowLevel_Init();
    }
    //cs pins
    RCC_APB2PeriphClockCmd(SD_CS_GPIO_CLK | SF_CS_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(SD_CS_GPIO_PORT, SD_CS_PIN);

    GPIO_InitStructure.GPIO_Pin = SF_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SF_CS_GPIO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(SF_CS_GPIO_PORT, SF_CS_PIN);

}
uint8_t SD_WriteByte(uint8_t Data)
{
    if(stm32_spi_choose){
        return stm32_spi_WriteByte(Data);
    }
    else{
        return gpio_spi_WriteByte(Data);
    }
}
uint8_t SD_ReadByte(void)
{
    if(stm32_spi_choose){
        return stm32_spi_ReadByte();
    }
    else{
        return gpio_spi_ReadByte();
    }
}
void spi_choose_stm32(uint8_t choose)
{
    stm32_spi_choose = choose;
}
uint16_t spi_speed(uint32_t speed)
{
  return SPI_set_speed(SD_SPI, speed);
}
