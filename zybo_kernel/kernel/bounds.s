	.arch armv7-a
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 4
	.eabi_attribute 18, 4
	.file	"bounds.c"
@ GNU C (crosstool-NG 1.15.2) version 4.4.3 (arm-unknown-linux-uclibcgnueabi)
@	compiled by GNU C version 4.6.3, GMP version 4.3.2, MPFR version 2.4.2-p2.
@ GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
@ options passed:  -nostdinc -I./arch/arm/include
@ -I./arch/arm/include/generated/uapi -I./arch/arm/include/generated
@ -I./include -I./arch/arm/include/uapi -I./include/uapi
@ -I./include/generated/uapi -D__KERNEL__ -D__LINUX_ARM_ARCH__=7 -Uarm
@ -DKBUILD_BASENAME="bounds" -DKBUILD_MODNAME="bounds" -isystem
@ /usr/local/xtools/arm-unknown-linux-uclibcgnueabi/lib/gcc/arm-unknown-linux-uclibcgnueabi/4.4.3/include
@ -include ./include/linux/kconfig.h -MD kernel/.bounds.s.d kernel/bounds.c
@ -D_FORTIFY_SOURCE=2 -mlittle-endian -mabi=aapcs-linux
@ -mno-thumb-interwork -mfpu=vfp -marm -march=armv7-a -msoft-float
@ -mtune=xscale -auxbase-strip kernel/bounds.s -Os -Wall -Wundef
@ -Wstrict-prototypes -Wno-trigraphs -Werror-implicit-function-declaration
@ -Wno-format-security -Wframe-larger-than=1024
@ -Wdeclaration-after-statement -Wno-pointer-sign -Werror=implicit-int
@ -Werror=strict-prototypes -std=gnu89 -fno-strict-aliasing -fno-common
@ -fno-PIE -fno-dwarf2-cfi-asm -funwind-tables
@ -fno-delete-null-pointer-checks -fno-stack-protector -fomit-frame-pointer
@ -fno-strict-overflow -fconserve-stack -fverbose-asm
@ options enabled:  -falign-loops -fargument-alias -fauto-inc-dec
@ -fbranch-count-reg -fcaller-saves -fcprop-registers -fcrossjumping
@ -fcse-follow-jumps -fdefer-pop -fearly-inlining
@ -feliminate-unused-debug-types -fexpensive-optimizations
@ -fforward-propagate -ffunction-cse -fgcse -fgcse-lm
@ -fguess-branch-probability -fident -fif-conversion -fif-conversion2
@ -findirect-inlining -finline -finline-functions
@ -finline-functions-called-once -finline-small-functions -fipa-cp
@ -fipa-pure-const -fipa-reference -fira-share-save-slots
@ -fira-share-spill-slots -fivopts -fkeep-static-consts
@ -fleading-underscore -fmath-errno -fmerge-constants -fmerge-debug-strings
@ -fmove-loop-invariants -fomit-frame-pointer -foptimize-register-move
@ -foptimize-sibling-calls -fpeephole -fpeephole2 -freg-struct-return
@ -fregmove -freorder-blocks -freorder-functions -frerun-cse-after-loop
@ -fsched-interblock -fsched-spec -fsched-stalled-insns-dep
@ -fschedule-insns -fschedule-insns2 -fsection-anchors -fsigned-zeros
@ -fsplit-ivs-in-unroller -fsplit-wide-types -fthread-jumps
@ -ftoplevel-reorder -ftrapping-math -ftree-builtin-call-dce -ftree-ccp
@ -ftree-ch -ftree-copy-prop -ftree-copyrename -ftree-cselim -ftree-dce
@ -ftree-dominator-opts -ftree-dse -ftree-fre -ftree-loop-im
@ -ftree-loop-ivcanon -ftree-loop-optimize -ftree-parallelize-loops=
@ -ftree-pre -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-sra
@ -ftree-switch-conversion -ftree-ter -ftree-vect-loop-version -ftree-vrp
@ -funit-at-a-time -funwind-tables -fverbose-asm -fzero-initialized-in-bss
@ -mlittle-endian -msched-prolog -muclibc

@ Compiler executable checksum: b090a2698577a71981abe5e6ee4eeafc

	.text
	.align	2
	.global	foo
	.type	foo, %function
foo:
	.fnstart
.LFB149:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
#APP
@ 18 "kernel/bounds.c" 1
	
->NR_PAGEFLAGS #21 __NR_PAGEFLAGS	@
@ 0 "" 2
@ 19 "kernel/bounds.c" 1
	
->MAX_NR_ZONES #3 __MAX_NR_ZONES	@
@ 0 "" 2
@ 21 "kernel/bounds.c" 1
	
->NR_CPUS_BITS #2 ilog2(CONFIG_NR_CPUS)	@
@ 0 "" 2
@ 23 "kernel/bounds.c" 1
	
->SPINLOCK_SIZE #4 sizeof(spinlock_t)	@
@ 0 "" 2
	bx	lr	@
.LFE149:
	.fnend
	.size	foo, .-foo
	.ident	"GCC: (crosstool-NG 1.15.2) 4.4.3"
	.section	.note.GNU-stack,"",%progbits
