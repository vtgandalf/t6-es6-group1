/*
* @file     kernel_read_mmu_fault.c
* @brief    Kernel module to test if a read to up counter address 
*			w/o mapping results in a fault
*/

#include <linux/kernel.h>
#include <linux/module.h>

#define COUNTER_ADDR	(0x40024000)

MODULE_LICENSE ("GPL");

static int local_init (void);
static void local_exit (void);

module_init (local_init);
module_exit (local_exit);

static int local_init (void)
{
	printk (KERN_INFO "=== Starting module ===\n");

    // perform a read without mapping
	int* raw_addr = COUNTER_ADDR;
    printk (KERN_INFO "Now attempt to read from up counter address");
	printk (KERN_INFO "Value at 0x%x = 0x%x\n", raw_addr, *raw_addr);

    // read back registers
	unsigned int data_fsr = 0;
	asm volatile("MRC p15, 0, %0, c5, c0, 0" : "=r"(data_fsr) );
	printk (KERN_INFO "cp15 data fault status register=0x%x\n", data_fsr);

	unsigned int instruction_fsr = 0;
	asm volatile("MRC p15, 0, %0, c5, c0, 1" : "=r"(instruction_fsr) );
	printk (KERN_INFO "cp15 instruction fault status register=0x%x\n", instruction_fsr);

	unsigned int fault_addr_reg = 0;
	asm volatile("MRC p15, 0, %0, c6, c0, 0 " : "=r"(fault_addr_reg) );
	printk (KERN_INFO "cp15 fault address register=0x%x\n", fault_addr_reg);

	return 0;	
}

static void local_exit (void)
{
	printk (KERN_INFO "=== Ending module ===\n");
}
