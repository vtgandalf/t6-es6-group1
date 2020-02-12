/*
 * hello_world_4.c - Demonstrates module documentation.
 */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */

#define DRIVER_AUTHOR "Peter Jay Salzman 60p@dirac.org62"
#define DRIVER_DESC "A sample driver"
/*
 * Get rid of taint message by declaring code as GPL.
 */
MODULE_LICENSE("GPL");

/*
 * You can use strings, like this:
 */
/*MODULE_AUTHOR("Peter Jay Salzman 60p@dirac.org62"); */
/*MODULE_DESCRIPTION("A sample driver"); */

/*
 * Or with defines, like this:
 */
MODULE_AUTHOR(DRIVER_AUTHOR);    /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */

/*
 * This module uses /dev/testdevice. The MODULE_SUPPORTED_DEVICE macro might
 * be used in the future to help automatic configuration of modules, but is
 * currently unused other than for documentation purposes.
 */
MODULE_SUPPORTED_DEVICE("testdevice");

static int __init init_hello_4(void)
{
    printk(KERN_INFO "Hello, world 4\n");
    return 0;
}

static void __exit cleanup_hello_4(void)
{
    printk(KERN_INFO "Goodbye, world 4\n");
}

module_init(init_hello_4);
module_exit(cleanup_hello_4);
