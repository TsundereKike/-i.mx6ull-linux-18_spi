CROSS_COPILE ?= arm-linux-gnueabihf-
TARGET 		 ?= spi

CC			 := $(CROSS_COPILE)gcc
LD			 := $(CROSS_COPILE)ld
OBJCOPY		 := $(CROSS_COPILE)objcopy
OBJDUMP 	 := $(CROSS_COPILE)objdump

LIBPATH		 := -lgcc -L /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/4.9.4

INCUDIRS	:= imx6u \
			   bsp/clk \
			   bsp/delay \
			   bsp/led \
			   bsp/beep \
			   bsp/key \
			   bsp/gpio \
			   bsp/int \
			   bsp/exti \
			   bsp/epit \
			   bsp/uart \
			   bsp/lcd \
			   bsp/rtc \
			   bsp/i2c \
			   bsp/ap3216c \
			   bsp/spi \
			   bsp/icm20608 \
			   bsp/fpu \
			   stdio/include

SRCDIRS		:= project \
			   bsp/clk \
			   bsp/delay \
			   bsp/led	\
			   bsp/beep \
			   bsp/key \
			   bsp/gpio \
			   bsp/int \
			   bsp/exti \
			   bsp/epit \
			   bsp/uart \
			   bsp/lcd \
			   bsp/rtc \
			   bsp/i2c \
			   bsp/ap3216c \
			   bsp/spi \
			   bsp/icm20608 \
			   bsp/fpu \
			   stdio/lib

INCLUDE		:= $(patsubst %, -I %, $(INCUDIRS))

SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

SFILENDIR	:= $(notdir $(SFILES))
CFILENDIR	:= $(notdir $(CFILES))

SOBJS		:= $(patsubst %, obj/%, $(SFILENDIR:.S=.o))
COBJS		:= $(patsubst %, obj/%, $(CFILENDIR:.c=.o))

OBJS		:= $(SOBJS)$(COBJS)

VPATH		:= $(SRCDIRS)

.PHONY:clean

$(TARGET).bin : $(OBJS)
	$(LD) -Timx6u.lds -o $(TARGET).elf $^ $(LIBPATH)
	$(OBJCOPY) -O binary -S $(TARGET).elf $@
	$(OBJDUMP) -D -m arm $(TARGET).elf > $(TARGET).dis

$(SOBJS) : obj/%.o : %.S
	$(CC) -Wall -nostdlib -fno-builtin -c -O2 $(INCLUDE) -o $@ $<

$(COBJS) : obj/%.o : %.c
	$(CC) -Wall -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -Wa,-mimplicit-it=thumb -nostdlib -fno-builtin -c -O2 $(INCLUDE) -o $@ $<

clean:
	rm -rf *.bin *.elf *.dis $(OBJS)

print:
	@echo INCLUDE = $(INCLUDE)
	@echo SFILES = $(SFILES)
	@echo CFILES = $(CFILES)
	@echo SFILENDIR = $(SFILENDIR)
	@echo CFILENDIR = $(CFILENDIR)
	@echo SOBJS = $(SOBJS)
	@echo COBJS	= $(COBJS)
	@echo OBJS = $(OBJS)
