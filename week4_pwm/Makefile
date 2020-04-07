# Add module object to kmake
# $(obj-m) specifies object files which are built as loadable kernel modules 
# See: https://www.kernel.org/doc/Documentation/kbuild/makefiles.txt
# obj-m += hello_world.o
obj-m += PWMDriver.o
PWMDriver-objs += pwm_main.o pwm_devfs.o pwm_ctrl.o pwm_clk_ctrl.o lcd_ctrl.o pp_iomem.o bit_manipulation.o
EXTRA_CFLAGS=-I$(PWD)/inc

#
#	Local Compilation Configuration
#

# Kernel version
VERSION=$(shell uname -r)

# Kernel source location
KSRC=/lib/modules/$(VERSION)/build
LPC_KSRC=~/Downloads/linux-2.6.34/

#
#	Cross Compilation Configuration
#

# Architecture
ARCH=arm

# Cross compiler prefix
CROSS_COMPILE=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-

#
#	Targets
#

all: local lpc

local:
	@ echo "=== COMPILE FOR LOCAL ENVIRONMENT ==="
	make -C $(KSRC) M=$(PWD) modules

lpc:
	@ echo "=== CROSS COMPILATION TO LPC3250 ==="
	@ echo "Kernel source: $(LPC_KSRC) "
	make -C $(LPC_KSRC) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

ifneq ($(LPC_KSRC),)
clean:
	make -C $(KSRC) M=$(PWD) clean
	make -C $(LPC_KSRC) M=$(PWD) clean
else
clean:
	make -C $(KSRC) M=$(PWD) clean
endif
