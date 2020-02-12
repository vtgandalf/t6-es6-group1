# Externals
XKERNELFOLDER = linux-2.6.34
XKERNELARCH = linux-2.6.34.tar.gz
XKERNEL = https://mirrors.edge.kernel.org/pub/linux/kernel/v2.6/$(XKERNELARCH)

all: xkernel

xkernel: xkernel.clone xkernel.build
	# Clone and compile

xkernel.build:
	$(XKERNELFOLDER)...

xkernel.clone:
	@wget $(XKERNEL)
	@tar xvf $(XKERNELARCH)
	@rm linux-2.6.34.tar.gz
	# clone the pregenerated .config file so we dont 
	# download the whole image as well

xkernel.clean:
	@rm -rf $(XKERNELFOLDER)
