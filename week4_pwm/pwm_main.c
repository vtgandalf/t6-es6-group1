/**
* @file 	pwm_main.c
* @brief 	Module entry/exit
*/

/************************************************************
*	Includes
************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>

#include "es6_generic.h"	// generic defines
#include "pwm_devfs.h"		// for dev files

/************************************************************
*	Module entry/exit
************************************************************/

static int local_init (void); module_init (local_init);
static void local_exit (void); module_exit (local_exit);

static int local_init (void)
{
	int result = SUCCESS;
	
	result = pwm_devfs_initialize ();

	return result;	
}

static void local_exit (void)
{
	pwm_devfs_cleanup();
}
