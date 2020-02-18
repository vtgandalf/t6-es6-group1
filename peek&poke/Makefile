# Userspace programs
USERSPACE += assignment1

# Flags
CFLAGS=-Wall

# Kernel modules
obj-m += peek_poke_module.o

# Kernel version
VERSION=$(shell uname -r)

# Kernel source location
KSRC=/lib/modules/$(VERSION)/build

# Cross Compilation Configuration
LPC_KSRC=../linux-2.6.34/

# Cross compiler prefix 
CROSS_COMPILE=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-

# Targets

local:
	@ echo "=== COMPILE FOR LOCAL ENVIRONMENT ==="
	make -C $(KSRC) M=$(PWD) modules

user_space:
	gcc $(CFLAGS) $(USERSPACE).c -o $(USERSPACE)

lpc:
	@ echo "=== CROSS COMPILATION TO LPC3250 ==="
	@ echo "Kernel source: $(LPC_KSRC)"
	make -C $(LPC_KSRC) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

ifneq ($(LPC_KSRC),)
clean:
	make -C $(KSRC) M=$(PWD) clean
	make -C $(LPC_KSRC) M=$(PWD) clean
	rm $(USERSPACE)
else
clean:
	make -C $(KSRC) M=$(PWD) clean
	$(USERSPACE)
endif	