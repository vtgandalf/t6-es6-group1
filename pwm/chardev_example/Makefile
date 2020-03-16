# Flags
UCFLAGS=-Wall

# Kernel modules
obj-m += chardev.o

# Kernel version
VERSION=$(shell uname -r)

# Kernel source location
KSRC=/lib/modules/$(VERSION)/build

# Cross Compilation Configuration
ARCH=arm
LPC_KSRC=../linux-2.6.34/

# Cross compiler prefix
CROSS_COMPILE=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-

# Targets
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
	# make user_space.clean
	make -C $(KSRC) M=$(PWD) clean
	make -C $(LPC_KSRC) M=$(PWD) clean
else
clean:
	# make user_space.clean
	make -C $(KSRC) M=$(PWD) clean
endif
