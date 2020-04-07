/*
* @file     kernel_read_cp15.c
* @brief    Kernel module to read control register
*/

#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE ("GPL");

static int local_init (void);
static void local_exit (void);
module_init (local_init);
module_exit (local_exit);

static int local_init (void)
{
	printk (KERN_INFO "=== Starting module ===\n");

	// read control register 
	unsigned int ctrl_reg = 0;
	asm volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(ctrl_reg) );
	printk (KERN_INFO "cp15 c1=0x%x\n", ctrl_reg);	

	return 0;	
}

static void local_exit (void)
{
	printk (KERN_INFO "=== Ending module ===\n");
}
