/**
* @file pp_main.c
* @author Thanh Hoang
* @date 05/03/2020
* @brief Assignment Part 2: Read and write to I/O register
*		 Module entry and exit
*/

/************************************************************
*	Includes
************************************************************/
#include "pp_main.h"

#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module

#include "pp_sysfs.h"					// for sysfs ops

MODULE_LICENSE ("GPL");

/************************************************************
*	Static data and declarations
************************************************************/
static int local_init (void); module_init (local_init);
static void local_exit (void); module_exit (local_exit);

/************************************************************
*	Module entry/exit
************************************************************/
static int local_init (void)
{
	int result = SUCCESS;
	
	printk (KERN_INFO "=== Starting module ===\n");
	
	result = pp_sysfs_init ();

	if (result == SUCCESS)
	{
		printk (KERN_INFO "Init OK!\n");
	}
	else 
	{
		printk (KERN_INFO "Init failed with error=%d\n", result);
	}

	return result;	
}

static void local_exit (void)
{
	printk (KERN_INFO "=== Ending module ===\n");
	pp_sysfs_cleanup();
}
