# Externals
XKERNELFOLDER = linux-2.6.34
XKERNELARCH = linux-2.6.34.tar.gz
XKERNEL = https://mirrors.edge.kernel.org/pub/linux/kernel/v2.6/$(XKERNELARCH)
CONFIG = .config
ARCH=arm
CROSS_COMPILE=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-

all: xkernel

xkernel: xkernel.clone xkernel.build
	# Clone and compile

xkernel.build:
	@make -C $(XKERNELFOLDER) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) oldconfig
	@make -C $(XKERNELFOLDER) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) prepare
	@make -C $(XKERNELFOLDER) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) scripts

xkernel.clone:
	@wget $(XKERNEL)
	@tar xvf $(XKERNELARCH)
	@rm $(XKERNELARCH)
	@cp $(CONFIG) $(XKERNELFOLDER)/.config
	# clone the pregenerated .config file so we dont 
	# download the whole image as well

xkernel.clean:
	@rm -rf $(XKERNELFOLDER)
