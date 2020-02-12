/**
* @file hello_world.c
* @author Thanh Hoang
* @date 11/02/2020
* @brief Assignment 0: Hello World Kernel Module
*/

/************************************************************
*	Includes
************************************************************/
#include <linux/module.h> // needed by ALL modules
#include <linux/kernel.h> // needed for KERN_INFO

/************************************************************
*	Module info
************************************************************/
MODULE_AUTHOR ("Thanh, Hoang");
MODULE_DESCRIPTION ("Hello World kernel module (w/ outro)");

/************************************************************
*	Defines
************************************************************/
#define SUCCESS		0
#define ERROR		1

/************************************************************
*	Functions
************************************************************/

/**
* @brief Initialize function for insmod
*/
int init_module (void)
{
	printk (KERN_INFO "Hello, World!\n");
	return SUCCESS;
}

/**
* @brief Cleanup function for rmmod
*/
void cleanup_module (void)
{
	printk (KERN_INFO "Goodbye, World!\n");
}
