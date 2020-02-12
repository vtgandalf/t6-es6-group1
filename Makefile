# Externals
XKERNEL = # add links to the kernel
XKERNELFOLDER = # add the name of the downloaded folder

all: xkernel

xkernel: xkernel.clone xkernel.build
	# Clone and compile

xkernel.build:
	$(XKERNELFOLDER)...

xkernel.clone:
	@wget $(XKERNEL)

xkernel.clean:
	@rm -rf $(XKERNELFOLDER)
