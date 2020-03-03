/**
* @file hello_world.c
* @author Thanh Hoang
* @date 11/02/2020
* @brief Assignment 0: Hello World Kernel Module
*/

/************************************************************
*	Includes
************************************************************/
#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module
#include <linux/slab.h>					// for kmalloc(), kfree()
#include <linux/kobject.h>				// for sysfs
#include <linux/device.h>				// for sysfs
#include <asm/io.h>						// for ioremap and io r/w

#include "peek_and_poke.h"				// for data types and defines

MODULE_LICENSE ("GPL");

/************************************************************
*	Static data and declarations
************************************************************/
static int local_init (void); module_init (local_init);
static void local_exit (void); module_exit (local_exit);

static int init_sysfs (void);
static int parse_string (char* src, char* delim, input_param* output);
static int execute_read (input_param input);
static int execute_write (input_param input);

static ssize_t lpc_cmd_store (struct device *dev, struct device_attribute *attr, const char *buffer, size_t count);

static DEVICE_ATTR(lpc_cmd, S_IRWXU, NULL, lpc_cmd_store);

static struct attribute* attrs[] = {
	&dev_attr_lpc_cmd.attr,
	NULL
};
static struct attribute_group attr_group = {
    .attrs = attrs,
};
static struct kobject* sysfs_obj = NULL;

static input_param input = {0};
static int is_input_valid = FALSE;

/************************************************************
*	Init/terminate
************************************************************/
static int local_init (void)
{
	int result = SUCCESS;
	
	printk (KERN_INFO "=== Starting module ===\n");
	
	result = init_sysfs ();

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
	kobject_put (sysfs_obj);
}

/************************************************************
*	Static functions
************************************************************/
/**
 * @brief 		Initialize sysfs object 
 * 
 * @return int 	Error code
 */
static int init_sysfs (void)
{
	int result = SUCCESS;
	
	// create /sys/kernel/[SYSFS_DIR]
    sysfs_obj = kobject_create_and_add(SYSFS_DIR, kernel_kobj);

    if (sysfs_obj == NULL)
    {
        printk (KERN_INFO "module failed to load: kobject_create_and_add failed\n");
        return -ENOMEM;
    }

    result = sysfs_create_group(sysfs_obj, &attr_group);
    if (result != SUCCESS)
    {
        // failed to create files. remove parent directory.
        printk (KERN_INFO "module failed to load: sysfs_create_group failed with result %d\n", result);
        
		// remove reference to sysfs object
		kobject_put(sysfs_obj); 
        return -ENOMEM;
    }

    printk(KERN_INFO "/sys/kernel/%s/ created\n", SYSFS_DIR);

    return result;
}

static ssize_t lpc_cmd_store (struct device *dev, struct device_attribute *attr, const char *buffer, size_t count)
{
	char internal_buffer[SYSFS_MAX_BUF_LEN + 1];
	int internal_buffer_size = 0;

	// notify us if user gives a really long input
	if (count > SYSFS_MAX_BUF_LEN)
		printk (KERN_ALERT "[lpc_cmd_store] User buffer larger than internal buffer\n");
	
	// continue anyway
	internal_buffer_size = count > SYSFS_MAX_BUF_LEN ? SYSFS_MAX_BUF_LEN : count;
	memcpy (internal_buffer, buffer, internal_buffer_size);
	internal_buffer[internal_buffer_size] = '\0';
	
	// store input, if possible
	if (parse_string(internal_buffer, PROTOCOL_DELIM, &input) == SUCCESS)
	{
		is_input_valid = TRUE;
		if (input.op == PROTOCOL_READ_OP)
			execute_read (input);
		else if (input.op == PROTOCOL_WRITE_OP)
			execute_write (input);
	}
	else 
	{
		is_input_valid = FALSE;
	}
	
	return internal_buffer_size;
}

static int execute_read (input_param input)
{
	int result = is_input_valid ? SUCCESS : ERROR;

	if (result == SUCCESS)
	{
		int nr_of_registers = 0;
		sscanf (input.data, "%d", &nr_of_registers);
		if (nr_of_registers == 0)
		{
			printk (KERN_INFO "Cannot parse number of registers from args [%s]", input.data);
			return ERROR;
		}
		else 
			printk (KERN_INFO "Expect to read [%d] registers starting from [0x%08x]\n", nr_of_registers, input.addr);

		int reg = 0;
		for (reg = 0; reg < nr_of_registers; reg++)
		{
			unsigned long current_addr = input.addr + (reg * REGISTER_SIZE);
			void __iomem* iomem_region = ioremap (current_addr, REGISTER_SIZE);

			if (iomem_region == NULL)
			{
				printk (KERN_INFO "Failed to map mem region for address 0x%08x\n", input.addr);
			}
			else 
			{
				unsigned long temp_data = 0;
				temp_data = ioread32 (iomem_region);
				printk (KERN_INFO "Addr [0x%08x] = %lu\n", current_addr, temp_data);
				iounmap(iomem_region);
			}
		}
	}
	return result;
}

static int execute_write (input_param input)
{
	int result = is_input_valid ? SUCCESS : ERROR;

	if (result == SUCCESS)
	{
		unsigned long new_value = -1;
		sscanf (input.data, "%08x", &new_value); 
		
		if (new_value == -1)
		{
			printk (KERN_INFO "Cannot parse new value from args [%s]", input.data);
			return ERROR;
		}
		else 
			printk (KERN_INFO "Expect to write [0x%08x] to address [0x%08x]\n", new_value, input.addr);

		void __iomem* iomem_region = ioremap (input.addr, REGISTER_SIZE);

		if (iomem_region == NULL)
		{
			printk (KERN_INFO "Failed to map mem region for address 0x%08x\n", input.addr);
		}
		else 
		{
			iowrite32 (new_value, iomem_region);
			printk (KERN_INFO "Write OK!\n");
			iounmap(iomem_region);
		}
	}
	return result;
}

/**
 * @brief			Parse a string according to protocol
 * 
 * @param src 		Source string 
 * @param delim 	Delimiter
 * @param output 	Output param struct 
 * @return int 		Error code
 */
static int parse_string (char* src, char* delim, input_param* output)
{
	// null check parameters
	if (src == NULL || delim == NULL || output == NULL)
		return -ENOMEM;
		
	// duplicated source string (allocated on behalf of kernel)
	char* dup = kmalloc (strlen(src) + 1, GFP_KERNEL);
	
	// failed allocation 
	if (dup == NULL)
		return -ENOMEM;
	else 
		strcpy (dup, src);
	
	// parse
	int i = 0; 
	char* param_str;

	for (i = 0; i < PROTOCOL_MAX_INDEX; i++)
	{
		param_str = strsep(&dup, delim);
		switch (i)
		{
			case PROTOCOL_INDEX_OP:
				output->op = param_str[0]; // take the first character
				break;
			case PROTOCOL_INDEX_ADDR:
                output->addr = simple_strtoul (param_str, NULL, HEXADECIMAL_BASE);
				break;
            case PROTOCOL_INDEX_DATA:
                strcpy (output->data, param_str);
				int used_size = strlen(param_str);
				output->data[used_size] = '\0';
                break;
		}
	}
	
	kfree (dup);
	return SUCCESS;
}
