
# Userspace programs
USERSPACE += assignment1

# Flags
UCFLAGS=-Wall

# Kernel modules
# obj-m += peek_poke_module.o
obj-m += assignment2.o

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

all: local user_space lpc

local:
	@ echo "=== COMPILE FOR LOCAL ENVIRONMENT ==="
	make -C $(KSRC) M=$(PWD) modules

user_space:
	gcc $(UCFLAGS) $(USERSPACE).c -o $(USERSPACE)

user_space.clean:
	rm $(USERSPACE)

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
