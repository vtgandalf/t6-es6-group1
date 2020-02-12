# Add module object to kmake
# $(obj-m) specifies object files which are built as loadable kernel modules 
# See: https://www.kernel.org/doc/Documentation/kbuild/makefiles.txt
obj-m += hello_world.o
obj-m += hello_world_1.o
obj-m += hello_world_2.o
obj-m += hello_world_3.o
obj-m += hello_world_4.o
obj-m += hello_world_5.o
obj-m += startstop.o
startstop-objs := start.o stop.o

# Location of build folder
# (uname -r is the release version of the kernel)
BUILD_FOLDER=/lib/modules/$(shell uname -r)/build

all:
	@echo "=== COMPILE FOR LOCAL ENVIRONMENT ==="
	make -C $(BUILD_FOLDER) M=$(PWD) modules

# ARCH and CROSS_COMPILE needs to be exported for other processes which 'make' invokes 
# 'make' variables are not epxorted by default
lpc:
	@echo "=== CROSS COMPILATION TO LPC3250 ==="
	export ARCH=arm
	export CROSS_COMPILE=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-gcc
	make -C $(BUILD_FOLDER) M=$(PWD) modules

clean:
	make -C $(BUILD_FOLDER) M=$(PWD) clean
