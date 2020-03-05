/**
* @file pp_iomem.c
* @author Thanh Hoang
* @date 05/03/2020
* @brief Assignment Part 2: Read and write to I/O register
*		 Handle direct read/write to physical memory
*/

/************************************************************
*	Includes
************************************************************/
#include "pp_iomem.h"

#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module
#include <asm/io.h>						// for io_remap and r/w

#include "pp_main.h"					// for generic defines

/************************************************************
*	Static data
************************************************************/
static const size_t FOUR_BYTES_SIZE = 4;

/************************************************************
*	Public functions
************************************************************/

int pp_iomem_read_4_bytes (unsigned long address, unsigned long* output)
{
    if (output == NULL)
		return ERROR;
    
    int result = SUCCESS;
			
	void __iomem* iomem_region = ioremap (address, FOUR_BYTES_SIZE);

	if (iomem_region == NULL)
	{
		printk (KERN_INFO "Failed to map mem region for address 0x%08x\n", address);
	}
	else 
	{
		*output = ioread32 (iomem_region);
		iounmap(iomem_region);
	}

    return result;
}

void pp_iomem_write_4_bytes (unsigned long address, unsigned long value)
{
	void __iomem* iomem_region = ioremap (address, FOUR_BYTES_SIZE);
	
	if (iomem_region == NULL)
	{
		printk (KERN_INFO "Failed to map mem region for address 0x%08x\n", address);
	}
	else 
	{
		iowrite32 (value, iomem_region);
		printk (KERN_INFO "Write OK!\n");
		iounmap (iomem_region);
	}
}
