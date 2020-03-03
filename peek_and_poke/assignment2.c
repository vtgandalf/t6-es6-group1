/*
* @file     assignment2.c
* @author   Valeri Todorov
* @date     25/02/2020
*
* @brief    This is part two of the assignment
*           for week2.
*/

#include <linux/device.h>
#include <linux/kernel.h>   	// Doing kernel stuff
#include <linux/kobject.h>  	// For sysfs
#include <linux/module.h>   	// This is a module
#include <asm/io.h>		// For io mapping

// Define the folder and the name of the file
// that will be accessed
#define sysfs_dir "es6_week2_assignment"
#define sysfs_file "assignment_part_2"
#define sysfs_max_data_size 1024

// This method could be overwritten in order to set
// the file r/w permissions for all users.
// Generally this is not reccomended and is consedered
// to be a security flaw. This is why I have only showed
// how its done, but I dont actually do it
// #undef VERIFY_OCTAL_PERMISSIONS
// #define VERIFY_OCTAL_PERMISSIONS(perms) (perms)

MODULE_LICENSE("Dual BSD/GPL");
// MODUlE_AUTHOR("Valeri Todorov"); // This line gives an error for some reason?
MODULE_DESCRIPTION("Part 2 of ES6 assignment, week2.");

static char sysfs_buffer[sysfs_max_data_size + 1] =
    "Testing Data.";
static ssize_t used_buffer_size = 0;

// Standart impl taken from the given examples
static ssize_t sysfs_show(struct device* dev,
                          struct device_attribute* attr,
                          char* buffer)
{
    printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n",
           sysfs_dir, sysfs_file);
    return sprintf(buffer, "%s\n", sysfs_buffer);
}

// Method to handle writing to the file
// This method handles both writing and reading to
// a specific memory address using the suggested
// protocol in the assignment description
static ssize_t sysfs_store(struct device* dev,
                           struct device_attribute* attr,
                           const char* buffer,
                           size_t count)
{
    // Create the object holding the parameters
    char params[3][sysfs_max_data_size + 1] = {{" "}, {" "}, {" "}};

    // Create an object to hold the value of the buffer
    // This is necessary because the buffer is const
    // and some actions with it are not permitted
    char test[sysfs_max_data_size + 1];

    // Make a ptr to the object to use for strsep
    char* testPtr = test;
    strcpy(test, buffer);

    // Fetch the parameters
    // Using while since for doesn't want to compile
    int i = 0;
    while (i < 3)
    {
        char* param = strsep(&testPtr, " ");

        if (param == NULL)
            break;

        strcpy(params[i], param);
        i++;
    }

    // Create the object that holds the data
    u_int data;

    // Copy the last parameter to it
    // This can be overwritten later on if
    // in r mode
    simple_strtoul(params[2], 0, &data);

    // Check if one of the parameters is invalid
    if ((strcmp(params[0], "r") != 0 &&
         strcmp(params[0], "w") != 0) ||
        strcmp(params[1], " ") == 0 ||
        (strcmp(params[0], "w") == 0 &&
         strcmp(params[2], " ") == 0))
    {
        // Print debug msg with instructions
        printk(
            KERN_INFO
            "The entered data is of incorrect type.\n"
            "Format: (mode address data_to_write)\n"
            "Example: w 0x40024000 0\n"
            "Example: r 0x40024000\n"
        );
        return count;
    }

    // Convert address to the appropriate type
    // make it unsigned because it is an address
    u_long addr = 0;
    simple_strtoul(params[1], 0, &addr);
    void* addrPtr = ioremap(addr, 32);

    // If we are in read mode
    if (strcmp(params[0], "r") == 0)
    {
        // Print debug information
        printk(
            KERN_INFO
            "address: %lx\n",
            addrPtr
        );

        // Read data from the addr
        data = ioread32(addrPtr);
        if (data == NULL)
        {
        printk(
            KERN_INFO "Retreived data from the address is null. This should not happen."
            );
        return count;
        }

        // Print debug info
        printk(
            KERN_INFO
            "retrieved data: %lu\n",
            data
        );
    }
    // Handle writing mode
    else
    {
        // Print debug info
        printk(
            KERN_INFO
            "data: %lu,address: %lu\n",
            data,
            addrPtr
        );

        // Write the data to the given address
        // Note: In the datasheet it was mentioned
        //       that writing to specific addresses
        //       may need to be done by setting a specific
        //       registry first, but since this program is
        //       for general addresses handling this is not
        //       accounted for here
	    iowrite32(data, addrPtr);
    }

    // Save the read data in the file
    used_buffer_size = sizeof(data) > sysfs_max_data_size ?
    sysfs_max_data_size : sizeof(data);
    snprintf(sysfs_buffer, used_buffer_size, "%lu", data);
    sysfs_buffer[used_buffer_size] = '\0';

    // Print debug info
    printk(KERN_INFO
            "sysfile_write (/sys/kernel/%s/%s called, buffer: "
            "%s, buffer size: %u, count: %u\n", sysfs_dir, sysfs_file, buffer, used_buffer_size, count);

    // return the buffer size
    return used_buffer_size;
}

// Set the permissions like that since the standart way is in conflict
// with the kernel permissions, it shall not compile otherwise
// Note: Writing and reading to the kern file is allowed only for a superuser
static DEVICE_ATTR(assignment_part_2, 0660, sysfs_show, sysfs_store);

static struct attribute *attrs[] = {
    &dev_attr_assignment_part_2.attr,
    NULL // to terminate the list of attrs
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *sysfs_obj = NULL;

// Standart init method that was taken from the examples
int __init sysfs_init(void)
{
    int result = 0;

    // Create the kernel object (this is the dir)
    sysfs_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
    if (sysfs_obj == NULL)
    {
        printk(KERN_WARNING
              "%s module failed to load, kobject_create_and_add failed\n",
              sysfs_file);
        return -ENOMEM;
    }

    // Create the file in the dir
    result = sysfs_create_group(sysfs_obj, &attr_group);
    if (result != 0)
    {
        printk(KERN_WARNING
               "%s module failed to load: sysfs_create_group "
               "failed with result %d\n", sysfs_file, result);
        kobject_put(sysfs_obj);
        return -ENOMEM;
    }

    printk(KERN_INFO
           "/sys/kernel/%s/%s created\n",
           sysfs_dir,
           sysfs_file);
    return result;
}

void __exit sysfs_exit(void)
{
    kobject_put(sysfs_obj);
    printk(KERN_INFO
           "/sys/kernel/%s/%s removed\n",
           sysfs_dir,
           sysfs_file);
}

module_init(sysfs_init);
module_exit(sysfs_exit);
