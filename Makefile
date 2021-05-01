TARGET = gpio_toggle

export CC             = arm-none-eabi-gcc           
export AS             = arm-none-eabi-as
export LD             = arm-none-eabi-ld
export OBJCOPY        = arm-none-eabi-objcopy

TOP=$(shell pwd)

INC_FLAGS= \
           -I $(TOP)/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc             \
		   -I $(TOP)/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/      \
		   -I $(TOP)/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/ \
		   -I $(TOP)


CFLAGS =  -W -Wall -g -mcpu=cortex-m3 -mthumb -D STM32F10X_MD -D USE_STDPERIPH_DRIVER $(INC_FLAGS) -O0 -std=gnu11
C_SRC=$(shell find ./ -name '*.c')  
C_OBJ=$(C_SRC:%.c=%.o)          

.PHONY: all clean

all:$(C_OBJ)
	$(CC) $(C_OBJ) -T stm32_f103ze_gcc.ld -o $(TARGET).elf   -mthumb -mcpu=cortex-m3 -Wl,--start-group -lc -lm -Wl,--end-group -specs=nano.specs -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=Project.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80 
	$(OBJCOPY) $(TARGET).elf  $(TARGET).bin -Obinary 
	$(OBJCOPY) $(TARGET).elf  $(TARGET).hex -Oihex

$(C_OBJ):%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<
clean:
	rm -f $(shell find ./ -name '*.o')
	rm -f $(shell find ./ -name '*.d')
	rm -f $(shell find ./ -name '*.map')
	rm -f $(shell find ./ -name '*.elf')
	rm -f $(shell find ./ -name '*.bin')
	rm -f $(shell find ./ -name '*.hex')
