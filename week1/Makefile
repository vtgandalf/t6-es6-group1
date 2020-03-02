# Add module object to kmake
# $(obj-m) specifies object files which are built as loadable kernel modules 
# See: https://www.kernel.org/doc/Documentation/kbuild/makefiles.txt
# obj-m += hello_world.o
# obj-m += hello_world_1.o
# obj-m += hello_world_2.o
# obj-m += hello_world_3.o
# obj-m += hello_world_4.o
# obj-m += hello_world_5.o
# obj-m += startstop.o
# startstop-objs := start.o stop.o
obj-m += peek_and_poke.o

#
#	Local Compilation Configuration
#

# Kernel version
VERSION=$(shell uname -r)

# Kernel source location
KSRC=/lib/modules/$(VERSION)/build

#
#	Cross Compilation Configuration
#
LPC_KSRC=../linux-2.6.34/

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
	@ echo "Kernel source: $(LPC_KSRC)"
	make -C $(LPC_KSRC) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

ifneq ($(LPC_KSRC),)
clean:
	make -C $(KSRC) M=$(PWD) clean
	make -C $(LPC_KSRC) M=$(PWD) clean
else
clean:
	make -C $(KSRC) M=$(PWD) clean
endif
