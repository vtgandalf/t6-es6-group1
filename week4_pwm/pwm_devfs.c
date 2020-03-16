/**
* @file 	pwm_devfs.c
* @author 	Thanh Hoang
* @brief 	Handle devfs read write
*/

/************************************************************
*	Includes
************************************************************/

#include "pwm_devfs.h"

#include <linux/kernel.h>       // kernel work 
#include <linux/module.h>       // required 
#include <linux/fs.h>           // for file operations
#include <linux/kdev_t.h>       // for MINOR macro
#include <asm/uaccess.h>        // for put_user
#include <linux/types.h>

#include "es6_generic.h"        // generic defines
#include "pwm_ctrl.h"           // for pwm control 

/************************************************************
*	Defines
************************************************************/
#define NR_OF_PWMS              (2)
#define NR_DEV_NODES_PER_PWM    (3)
#define NR_OF_MINOR_HANDLERS    (NR_OF_PWMS*NR_DEV_NODES_PER_PWM)

#define MAX_USER_INPUT_LEN      (256)

// Keep data types consistent with data handlers table
#define PWM_DATA_ENABLE         (0)
#define PWM_DATA_FREQ           (1)
#define PWM_DATA_DUTY           (2)

typedef struct pwm_data_t {
    pwm_enum    id;
    int         data_type;
} pwm_data;

typedef struct pwm_data_handler_t {
    int (*read) (pwm_enum, int* output);
    int (*write) (pwm_enum, int);
} pwm_data_handler;

/************************************************************
*	Static data
************************************************************/
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write
};

static int is_device_open = FALSE;

/** @brief Handler lookup table, indexed by data type */
static const pwm_data_handler pwm_handlers_table[NR_DEV_NODES_PER_PWM] = {
    { pwm_ctrl_read_enable,   pwm_ctrl_write_enable },
    { pwm_ctrl_read_freq,     pwm_ctrl_write_freq },
    { pwm_ctrl_read_duty,     pwm_ctrl_write_duty }
};

/** @brief PWM data look up table, indexed by minor number */
static const pwm_data pwm_data_table[NR_OF_MINOR_HANDLERS] = {
    { PWM_1, PWM_DATA_ENABLE },     // minor 0: /dev/pwm1_enable
    { PWM_1, PWM_DATA_FREQ },       // minor 1: /dev/pwm1_freq
    { PWM_1, PWM_DATA_DUTY },       // minor 2: /dev/pwm1_duty
    { PWM_2, PWM_DATA_ENABLE },     // minor 3: /dev/pwm2_enable
    { PWM_2, PWM_DATA_FREQ },       // minor 4: /dev/pwm2_freq
    { PWM_2, PWM_DATA_DUTY }        // minor 5: /dev/pwm2_duty
};

/************************************************************
*	Public functions
************************************************************/

int pwm_devfs_initialize (void)
{
    int result = SUCCESS;

    result = register_chrdev (PWM_DEVFS_MAJOR_NUMBER, PWM_DEVFS_DEVICE_NAME, &fops);

    if (result != SUCCESS)
    {
        printk (KERN_ALERT "Failed to register device with error code [%d]\n", result);
    }

    return result;
}

// ------------------------------------------------------------ //

void pwm_devfs_cleanup (void)
{
    if (is_device_open)
    {
        printk(KERN_ALERT "Cannot unregister device, because it is still open!\n");
        return;
    }

    unregister_chrdev (PWM_DEVFS_MAJOR_NUMBER, PWM_DEVFS_DEVICE_NAME);
	printk (KERN_INFO "=== Module ended successfull ===\n");
}

/************************************************************
*	Static functions
************************************************************/

static int device_open(struct inode *inode, struct file *file)
{
    int result = (is_device_open) ? -EBUSY : SUCCESS;

    if (result == SUCCESS)
    {
        is_device_open = TRUE;
        file->private_data = (void*)(&pwm_data_table[MINOR(inode->i_rdev)]);
        try_module_get (THIS_MODULE);
    }

    return result;
}

// ------------------------------------------------------------ //

static int device_release(struct inode *inode, struct file *file)
{
    is_device_open = FALSE;
    file->private_data = NULL;
    module_put(THIS_MODULE);
    return SUCCESS;
}

// ------------------------------------------------------------ //
static ssize_t device_read (struct file *file, char *buffer, size_t size, loff_t * offset)
{
    static int data = 0;
    static int bytes_processed = 0;

    int i = 0;
    int result = SUCCESS;
    char* data_p = (char*)&data;
    pwm_data* pwm = (pwm_data*)file->private_data;

    if (bytes_processed == 0)
    {
        result = pwm_handlers_table[pwm->data_type].read(pwm->id, &data);

        if (result == SUCCESS)
        {
            *offset = 0;
        }
    }

    for (i = 0; i < *offset; i++)
    {
        data_p++;   
    }

    while (bytes_processed < size 
        && bytes_processed < sizeof(int)
        && result == SUCCESS)
    {
        result = put_user (*data_p, buffer);
        data_p++;
        buffer++;
        bytes_processed++;
    }

    *offset = bytes_processed;

    if (bytes_processed >= sizeof(int))
    {
        bytes_processed = 0;
    }

    if (result == SUCCESS)
    {
        printk ("[PWM%d] data=%d; bytes processed=%d\n", pwm->id, data, bytes_processed);
    }

    return bytes_processed;
}

// ------------------------------------------------------------ //
static ssize_t device_write (struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    int result = SUCCESS;

    int i = 0;
    int data = 0;
    char user_input[MAX_USER_INPUT_LEN];
    pwm_data* pwm = (pwm_data*)(file->private_data);
    
    for (i = 0; 
        (i < length) && (i < MAX_USER_INPUT_LEN) && (result == SUCCESS); 
        i++)
    {
        result = get_user (user_input[i], buffer + i);
    }

    if (result == SUCCESS)
    {
        if (sscanf (user_input, "%d", &data) == 0)
        {
            result = ERROR;
            printk(KERNEL_WARNING "The data is invalid.");
        }
    }

    if (result == SUCCESS)
    {
        result = pwm_handlers_table[pwm->data_type].write(pwm->id, data);
    }

    if (result == SUCCESS)
    {
        printk ("[PWM%d] Set data to [%d]\n", pwm->id, data);
    }

    return i;
}