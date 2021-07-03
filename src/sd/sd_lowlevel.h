#ifndef _SD_LOWLEVEL_H
#define _SD_LOWLEVEL_H

#define SD_SPI                           SPI1
#define SD_SPI_CLK                       RCC_APB2Periph_SPI1
#define SD_SPI_SCK_PIN                   GPIO_Pin_5                  /* PA.05 */
#define SD_SPI_SCK_GPIO_PORT             GPIOA                       /* GPIOA */
#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOA
#define SD_SPI_MISO_PIN                  GPIO_Pin_6                  /* PA.06 */
#define SD_SPI_MISO_GPIO_PORT            GPIOA                       /* GPIOA */
#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SD_SPI_MOSI_PIN                  GPIO_Pin_7                  /* PA.07 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOA                       /* GPIOA */
#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SD_CS_PIN                        GPIO_Pin_1                  /* PC.12 */
#define SD_CS_GPIO_PORT                  GPIOB                       /* GPIOC */
#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOB
#if 1
#define SD_DETECT_PIN                    GPIO_Pin_7                  /* PE.07 */
#define SD_DETECT_GPIO_PORT              GPIOE                       /* GPIOE */
#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOE
#endif

//spi flash
#define SF_CS_PIN                 GPIO_Pin_4                  /* PC.12 */
#define SF_CS_GPIO_PORT           GPIOB                       /* GPIOC */
#define SF_CS_GPIO_CLK            RCC_APB2Periph_GPIOB

/*******************gpio spi********/
#define DOUT_GG GPIOA
#define TDIN_GG GPIOA
#define TCLK_GG GPIOA
#define DOUT_PIN GPIO_Pin_6
#define TDIN_PIN GPIO_Pin_7
#define TCLK_PIN GPIO_Pin_5
#define DOUT GPIO_ReadInputDataBit(DOUT_GG, DOUT_PIN)
#define TDIN(x) GPIO_WriteBit(TDIN_GG, TDIN_PIN, x)
#define TCLK(x) GPIO_WriteBit(TCLK_GG, TCLK_PIN, x)

/*******************export api********/
void SD_LowLevel_Init(void);
void SD_LowLevel_DeInit(void);
#endif
