/**
* @file 	pwm_main.c
* @author 	Thanh Hoang
* @brief 	Module entry/exit
*/

/************************************************************
*	Includes
************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>

#include "es6_generic.h"
#include "pwm_devfs.h"

/************************************************************
*	Module entry/exit
************************************************************/

static int local_init (void); module_init (local_init);
static void local_exit (void); module_exit (local_exit);

static int local_init (void)
{
	int result = SUCCESS;
	
	printk (KERN_INFO "=== Starting module ===\n");
	
	result = pwm_devfs_initialize ();

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
	pwm_devfs_cleanup();
}
