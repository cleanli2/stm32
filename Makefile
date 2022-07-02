TARGET = stm32lcd
GIT_SHA1="$(shell git log --format='_%h' -1)"
DIRTY="$(shell git diff --quiet || echo 'dirty')"
CLEAN="$(shell git diff --quiet && echo 'clean')"

export CC             = arm-none-eabi-gcc           
export AS             = arm-none-eabi-as
export LD             = arm-none-eabi-ld
export OBJCOPY        = arm-none-eabi-objcopy

TOP=$(shell pwd)

INC_FLAGS= \
           -I $(TOP)/src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc             \
		   -I $(TOP)/src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/      \
		   -I $(TOP)/src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/ \
		   -I $(TOP)/src \
		   -I $(TOP)/src/sd \
		   -I $(TOP)/src/usb/USB/STM32_USB-FS-Device_Driver/inc \
		   -I $(TOP)/src/usb/USB/CONFIG \
		   -I $(TOP)/src/usb \
		   -I $(TOP)/src/lcd800x480 \


CFLAGS =  -W -Wall -g -mcpu=cortex-m3 -mthumb -DUSE_STDPERIPH_DRIVER $(INC_FLAGS) -O0 -std=gnu11 -ffunction-sections -fdata-sections
CFLAGS+=-DGIT_SHA1=\"$(GIT_SHA1)$(DIRTY)$(CLEAN)\"
LDFLAGS =  -mthumb -mcpu=cortex-m3 -Wl,--start-group -lc -lm -Wl,--end-group -specs=nano.specs -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=Project.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80
C_SRC= \
src/adc.c                                                                                             \
src/cmd.c                                                                                             \
src/CoIDE_startup.c                                                                                   \
src/common.c                                                                                          \
src/date.c                                                                                            \
src/env.c                                                                                             \
src/fs.c                                                                                              \
src/lcd800x480/cch_str.c                                                                              \
src/lcd800x480/gui.c                                                                                  \
src/lcd800x480/lcd_sueb.c                                                                             \
src/lcd800x480/test.c                                                                                 \
src/lcd800x480/touch.c                                                                                \
src/lprintf.c                                                                                         \
src/main.c                                                                                            \
src/music.c                                                                                           \
src/rtc.c                                                                                             \
src/os_task.c                                                                                             \
src/sd/sd_lowlevel.c                                                                                  \
src/sd/stm32_eval_spi_sd.c                                                                            \
src/sd/w25flash.c                                                                                     \
src/soc_rtc.c                                                                                         \
src/stm32f10x_it.c                                                                                    \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c  \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/misc.c                    \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_can.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_cec.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dac.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dbgmcu.c        \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c          \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c         \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_fsmc.c          \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c          \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_iwdg.c          \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_sdio.c          \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c           \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c         \
src/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_wwdg.c          \
src/task.c                                                                                            \
src/ui.c                                                                                              \
src/usb/USB/CONFIG/hw_config.c                                                                        \
src/usb/USB/CONFIG/mass_mal.c                                                                         \
src/usb/USB/CONFIG/memory.c                                                                           \
src/usb/USB/CONFIG/scsi_data.c                                                                        \
src/usb/USB/CONFIG/usb_bot.c                                                                          \
src/usb/USB/CONFIG/usb_desc.c                                                                         \
src/usb/USB/CONFIG/usb_endp.c                                                                         \
src/usb/USB/CONFIG/usb_istr.c                                                                         \
src/usb/USB/CONFIG/usb_prop.c                                                                         \
src/usb/USB/CONFIG/usb_pwr.c                                                                          \
src/usb/USB/CONFIG/usb_scsi.c                                                                         \
src/usb/USB/STM32_USB-FS-Device_Driver/src/usb_core.c                                                 \
src/usb/USB/STM32_USB-FS-Device_Driver/src/usb_init.c                                                 \
src/usb/USB/STM32_USB-FS-Device_Driver/src/usb_int.c                                                  \
src/usb/USB/STM32_USB-FS-Device_Driver/src/usb_mem.c                                                  \
src/usb/USB/STM32_USB-FS-Device_Driver/src/usb_regs.c                                                 \
src/usb/USB/STM32_USB-FS-Device_Driver/src/usb_sil.c                                                  \
src/usb/usb_main.c

C_OBJ=$(C_SRC:%.c=%.o)
ifeq ($(type),write_w25f)
CFLAGS+=-DWRITE_W25F
C_OBJ+=unused/ziku16.o
endif

ifeq ($(board),)
board=alientek_mini
endif
ifeq ($(board),alientek_mini)
$(warning choose board alientek_mini)
CFLAGS+=-DALIENTEK_MINI
CFLAGS+=-DSTM32F10X_HD
LDFILE=stm32_f103_128k_gcc
else
$(warning choose board hammer)
board=hammer
LDFILE=stm32_f103_512k_gcc
CFLAGS+=-DSTM32F10X_MD
endif

.PHONY: all clean

all:$(C_OBJ) src/asm.o
	touch src/version.h
	$(CC) $(C_OBJ) src/asm.o -T $(LDFILE).ld -o $(TARGET).elf $(LDFLAGS)
	$(OBJCOPY) $(TARGET).elf  $(TARGET).bin -Obinary 
	$(OBJCOPY) $(TARGET).elf  $(TARGET).hex -Oihex
	cp $(TARGET).hex $(TARGET)$(GIT_SHA1)_$(DIRTY)$(CLEAN).hex
	rm $(TARGET)_*.hex
	cp $(TARGET).hex $(TARGET)_$(board)_$(type)$(GIT_SHA1)_$(DIRTY)$(CLEAN).hex

$(C_OBJ):%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<
clean:
	rm -f $(shell find ./ -name '*.o')
	rm -f $(shell find ./ -name '*.d')
	rm -f $(shell find ./ -name '*.map')
	rm -f $(shell find ./ -name '*.elf')
	rm -f $(shell find ./ -name '*.bin')
	rm -f $(shell find ./ -name '*.hex')

src/asm.o:src/asm.s
	$(AS) -mcpu=cortex-m3 -mthumb src/asm.s -o src/asm.o
