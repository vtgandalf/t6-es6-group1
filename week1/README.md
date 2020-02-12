# Week 1 - Assignment 0: Hello World Kernel Module
This section documents how the hello world kernel module was cross-compiled for the LPC3250 board. Because the kernel version on the board is different than on the VM, the kernel source needs to first be cross-compiled and then the kernel module.

### 0. Cross-compilation flags
The flags relevant for cross-compilation are:
```
ARCH=$(TARGET_ARCHITECTURE)
CROSS_COMPILE=$(CROSS_COMPILER_WITHOUT_GCC_SUFFIX)
```
The architecture for LPC3250 is ARM and the cross compiler is provided on Canvas.

### 1. Download correct kernel source
Connect to LPC3250 via serial (from datasheet: baud rate 115200, 8 data bits, 1 stop bits, no parity, no flow control). 

Run ```uname -r``` to find kernel version. 

Download source for said kernel version from https://mirrors.edge.kernel.org/pub/linux/kernel/.

### 2. Extract kernel image configuration
The ```scripts/``` directory of the kernel source has a script to extract the config of the image. This is needed to compile the kernel source properly for the LPC3250. In ```scripts/``` directory run:  

```
  ./extract-ikconfig /path/to/kernel/image > /path/to/saved/output
```

The output of this is saved in file ```.config``` in the main repository.

### 3. Cross-compile kernel source
The kernel source is compiled with:
```
  make -C /path/to/kernel/source ARCH=arm CROSS_COMPILE=/path/to/cross/compiler/without/gcc/suffix oldconfig
  make -C /path/to/kernel/source ARCH=arm CROSS_COMPILE=/path/to/cross/compiler/without/gcc/suffix prepare
  make -C /path/to/kernel/source ARCH=arm CROSS_COMPILE=/path/to/cross/compiler/without/gcc/suffix scripts
```
This builds the headers needed for the kernel module. 

### 4. Cross-compile kernel module
The module can be compiled with:
```
  make -C /path/to/compiled/kernel/source [other relevant flags]
```

### 5. Copy output to board
The output ```.ko``` files can be copied to the user files partition of the boot USB stick. This partition is automatically mounted on at /home/ for LPC3250.
