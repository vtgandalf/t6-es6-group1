/*
* @file     assignment2.c
* @author   Valeri Todorov
* @date     25/02/2020
*
* @brief    This is part two of the assignment
*           for week2.
*/

#include <linux/device.h>
#include <linux/kernel.h>   // Doing kernel stuff
#include <linux/kobject.h>  // For sysfs
#include <linux/module.h>   // This is a module

// Define the folder and the name of the file
// that will be accessed
#define sysfs_dir "es6_week2_assignment"
#define sysfs_file "assignment_part_2"
#define sysfs_max_data_size 1024

// #undef VERIFY_OCTAL_PERMISSIONS
// #define VERIFY_OCTAL_PERMISSIONS(perms) (perms)

MODULE_LICENSE("Dual BSD/GPL");
// MODUlE_AUTHOR("Valeri Todorov");
MODULE_DESCRIPTION("Part 2 of ES6 assignment, week2.");

static char sysfs_buffer[sysfs_max_data_size + 1] =
    "Testing Data.";
static ssize_t used_buffer_size = 0;

static ssize_t sysfs_show(struct device* dev,
                          struct device_attribute* attr,
                          char* buffer)
{
    printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n",
           sysfs_dir, sysfs_file);
    return sprintf(buffer, "%s\n", sysfs_buffer);
}

static ssize_t sysfs_store(struct device* dev,
                           struct device_attribute* attr,
                           const char* buffer,
                           size_t count)
{
    char params[3][sysfs_max_data_size + 1] = {{" "}, {" "}, {" "}};
    char test[sysfs_max_data_size + 1];
    char* testPtr = test;
    strcpy(test, buffer);

    // Fetch the parameters
    int i = 0;
    while (i < 3)
    {
        char* param = strsep(&testPtr, " ");

        if (param == NULL)
            break;

        strcpy(params[i], param);
        i++;
    }
    char data[sysfs_max_data_size + 1];
    strcpy(data, params[2]);

    // Check if one of the parameters is invalid
    if ((strcmp(params[0], "r") != 0 &&
         strcmp(params[0], "wr") != 0) ||
        strcmp(params[1], " ") == 0 ||
        strcmp(params[2], " ") == 0)
    {
        printk(
            KERN_INFO
            "The entered data is of incorrect type."
            "Example: (mode address n_registers/data) r/rw 0x0000000 0\n"
        );
        return count;
    }

    // Convert parameters
    u_long addr;
    kstrtol(params[1], 0, &addr);
    char* addrPtr = addr;
    long n_reg_or_data;
    kstrtol(params[2], 0, &n_reg_or_data);

    // If we are in read mode
    if (strcmp(params[0], "r") == 0)
    {
        printk(
            KERN_INFO
            "n_registers: %i ,address: %lx\n",
            n_reg_or_data,
            addrPtr
        );

        // Fetch the data from the address
        int i = 0;
        while (i < n_reg_or_data)
        {
            data[i] = *(addrPtr + i);
            i++;
        }
        data[n_reg_or_data] = "\0";

        printk(
            KERN_INFO
            "retrieved data: %s\n",
            data
        );
    }
    // Handle writing mode
    else
    {
        printk(
            KERN_INFO
            "data: %s ,address: %lx\n",
            params[2],
            addrPtr
        );
        // TODO(valtod): Actually write the data to the memory address
        // *(addrPtr) = data;
    }

    // Save the read data in the file
    used_buffer_size = sizeof(data) > sysfs_max_data_size ?
    sysfs_max_data_size : sizeof(data);

    printk(KERN_INFO
            "sysfile_write (/sys/kernel/%s/%s called, buffer: "
            "%s, bugger size: %u, count: %u\n", sysfs_dir, sysfs_file, buffer, used_buffer_size, count);

    memcpy(sysfs_buffer, data, used_buffer_size);
    sysfs_buffer[used_buffer_size] = '\0';

    return used_buffer_size;
}

// TODO(valtod): Add a method that writes information to it.
static DEVICE_ATTR(assignment_part_2, 0660, sysfs_show, sysfs_store);

static struct attribute *attrs[] = {
    &dev_attr_assignment_part_2.attr,
    NULL // to terminate the list of attrs
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *sysfs_obj = NULL;

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