# Add module object to kmake
# $(obj-m) specifies object files which are built as loadable kernel modules 
# See: https://www.kernel.org/doc/Documentation/kbuild/makefiles.txt
obj-m += kspaced_mem_access.o

# Location of build folder
# (uname -r is the release version of the kernel)
BUILD_FOLDER=/lib/modules/$(shell uname -r)/build

all:
	make -C $(BUILD_FOLDER) M=$(PWD) modules

clean:
	make -C $(BUILD_FOLDER) M=$(PWD) clean
