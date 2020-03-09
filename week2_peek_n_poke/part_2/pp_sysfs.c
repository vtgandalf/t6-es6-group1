/**
* @file pp_sysfs.c
* @author Thanh Hoang
* @date 05/03/2020
* @brief Assignment Part 2: Read and write to I/O register
*		 Handle sysfs input and output
*/

/************************************************************
*	Includes
************************************************************/
#include "pp_sysfs.h"

#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module
#include <linux/slab.h>					// for kmalloc(), kfree()
#include <linux/kobject.h>				// for sysfs
#include <linux/device.h>				// for sysfs

#include "pp_generic.h"					// for generic defines
#include "pp_iomem.h"					// for io access

/************************************************************
*	Defines
************************************************************/

#define SYSFS_DIR               "lpc_comm"
#define SYSFS_MAX_BUF_LEN       1024
#define SYSFS_MAX_INPUT_LEN     100         // enough for a 32 bit address string

#define PROTOCOL_INDEX_OP       0
#define PROTOCOL_INDEX_ADDR     1
#define PROTOCOL_INDEX_DATA     2
#define PROTOCOL_DELIM	        " "
#define PROTOCOL_MAX_INDEX      3
#define PROTOCOL_READ_OP        'r'
#define PROTOCOL_WRITE_OP       'w'

#define REGISTER_SIZE           4       	// (in bytes)
#define HEXADECIMAL_BASE	    16

typedef struct input_param_t {
	char 			op;
	unsigned long 	addr;
	char           data[SYSFS_MAX_INPUT_LEN];
} input_param;

/************************************************************
*	Sysfs configuration
************************************************************/

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

/************************************************************
*	Static data
************************************************************/
static void do_read (input_param input);
static void do_write (input_param input);
static int parse_string (char* src, char* delim, input_param* output);

/************************************************************
*	Public functions
************************************************************/

int pp_sysfs_init (void)
{
	int result = SUCCESS;

    sysfs_obj = kobject_create_and_add(SYSFS_DIR, kernel_kobj);

    if (sysfs_obj == NULL)
    {
        printk (KERN_INFO "module failed to load: kobject_create_and_add failed\n");
        return -ENOMEM;
    }

    result = sysfs_create_group(sysfs_obj, &attr_group);

    if (result != SUCCESS)
    {
        printk (KERN_INFO "module failed to load: sysfs_create_group failed with result %d\n", result);
		kobject_put(sysfs_obj);
        return -ENOMEM;
    }

    printk(KERN_INFO "/sys/kernel/%s/ created\n", SYSFS_DIR);

    return result;
}

void pp_sysfs_cleanup (void)
{
	kobject_put (sysfs_obj);
}

/************************************************************
*	Static functions
************************************************************/

static ssize_t lpc_cmd_store (struct device *dev, struct device_attribute *attr, const char *buffer, size_t count)
{
	char internal_buffer[SYSFS_MAX_BUF_LEN + 1];
	int internal_buffer_size = 0;
	input_param input = {0};

	if (count > SYSFS_MAX_BUF_LEN)
		printk (KERN_ALERT "[pp_sysfs] User buffer larger than internal buffer\n");

	internal_buffer_size = count > SYSFS_MAX_BUF_LEN ? SYSFS_MAX_BUF_LEN : count;
	memcpy (internal_buffer, buffer, internal_buffer_size);
	internal_buffer[internal_buffer_size] = '\0';

	if (parse_string(internal_buffer, PROTOCOL_DELIM, &input) == SUCCESS)
	{
		if (input.op == PROTOCOL_READ_OP)
		{
			do_read (input);
		}
		else if (input.op == PROTOCOL_WRITE_OP)
		{
			do_write (input);
		}
		else
		{
			printk (KERN_ALERT "[pp_sysfs] Unknown operation given %c\n", input.op);
		}
	}

	return internal_buffer_size;
}

static void do_read (input_param input)
{
	int nr_of_registers = 0;

	if (sscanf (input.data, "%d", &nr_of_registers) != 0)
	{
		int reg = 0;
		for (reg = 0; reg < nr_of_registers; reg++)
		{
			unsigned long current_addr = input.addr + (reg * REGISTER_SIZE);
			unsigned long temp_data = 0;
			if (pp_iomem_read_4_bytes (current_addr, &temp_data) == SUCCESS)
				printk (KERN_INFO "Addr [0x%08x] = %lu\n", current_addr, temp_data);
		}
	}
}

static void do_write (input_param input)
{
	unsigned long new_value = 0;
	if (sscanf (input.data, "%x", &new_value) != 0 &&
		pp_iomem_write_4_bytes (input.addr, new_value) == SUCCESS)
		printk (KERN_INFO "Write OK!\n");
}

static int parse_string (char* src, char* delim, input_param* output)
{
	if (src == NULL || delim == NULL || output == NULL)
		return -ENOMEM;

	char* dup = kmalloc (strlen(src) + 1, GFP_KERNEL);

	if (dup == NULL)
		return -ENOMEM;
	else
		strcpy (dup, src);

	int i = 0;
	char* param_str;

	for (i = 0; i < PROTOCOL_MAX_INDEX; i++)
	{
		param_str = strsep(&dup, delim);
		if (dup == NULL || strcmp(param_str, "") == 0)
			break;

		switch (i)
		{
			case PROTOCOL_INDEX_OP:
				output->op = param_str[0];
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

	printk(KERN_INFO "i is %i\n", i);
	if (i < PROTOCOL_MAX_INDEX)
	{
		printk(KERN_INFO "Not all parameters have been entered.\n");
		return -ENOMEM;
	}

	return SUCCESS;
}
