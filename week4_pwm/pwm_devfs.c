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

#define NR_DEV_NODES_PER_PWM    (3)
#define MAX_USER_INPUT_LEN      (256)
#define MINOR_UNKNOWN           (-1)
#define PWM_DATA_ENABLE         (0)
#define PWM_DATA_FREQ           (1)
#define PWM_DATA_DUTY           (2)
#define GET_EIGHT_BIT_DATA(x)   (uint8_t)(x)
#define GET_PWM_NR_FROM_ID(id)  (id + 1)

typedef struct pwm_data_t {
    int id;
    int data_type;
} pwm_data;

/************************************************************
*	Static data
************************************************************/

static int is_device_open = FALSE;
static unsigned int current_minor = MINOR_UNKNOWN;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int get_pwm_data_from_minor_nr (pwm_data* output);

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write
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
    unregister_chrdev (PWM_DEVFS_MAJOR_NUMBER, PWM_DEVFS_DEVICE_NAME);
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
        current_minor = MINOR(inode->i_rdev);
        try_module_get (THIS_MODULE);
    }

    return result;
}

// ------------------------------------------------------------ //

static int device_release(struct inode *inode, struct file *file)
{
    is_device_open = FALSE;
    current_minor = MINOR_UNKNOWN;
    module_put(THIS_MODULE);
    return SUCCESS;
}

// ------------------------------------------------------------ //

static ssize_t device_read(struct file *filp, char *buffer, size_t size, loff_t * offset)
{
    int result = SUCCESS;
    uint8_t data = 0;
    pwm_data pwm = {0};

    if (size <= 0)
        return -ENOMEM;

    result = get_pwm_data_from_minor_nr (&pwm);

    if (result == SUCCESS)
    {
        switch (pwm.data_type)
        {
            case PWM_DATA_ENABLE:
                result = pwm_ctrl_read_enable (pwm.id, &data);
                break;
            case PWM_DATA_FREQ:
                result = pwm_ctrl_read_freq (pwm.id, &data);
                break;
            case PWM_DATA_DUTY:
                result = pwm_ctrl_read_duty (pwm.id, &data);
                break;
        }
    }

    if (result == SUCCESS)
    {
        result = put_user (data, buffer);
    }

    if (result == SUCCESS)
    {
        printk ("[PWM%d] Written to user buffer data=%d\n", GET_PWM_NR_FROM_ID(pwm.id), data);
    }
    
    // We have one byte data, so on success, no need to call device_read() again.
    return (result == SUCCESS) ? 0 : -EFAULT;
}

// ------------------------------------------------------------ //

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    int result = SUCCESS;

    int i = 0;
    uint8_t data = 0;
    pwm_data pwm = {0};
    char user_input[MAX_USER_INPUT_LEN];
    
    result = get_pwm_data_from_minor_nr (&pwm);

    if (result == SUCCESS)
    {
        for (i = 0; (i < length) && (i < MAX_USER_INPUT_LEN) && (result == SUCCESS); i++)
        {
            result = get_user (user_input[i], buffer + i);
        }
        user_input[i] = '\0';
    }

    if (result == SUCCESS)
    {
        int temp_data = 0;
        if (sscanf (user_input, "%d", &temp_data) != 0)
        {
            data = GET_EIGHT_BIT_DATA(temp_data);
        }
        else 
        {
            result = ERROR;
        }
    }

    if (result == SUCCESS)
    {
        switch (pwm.data_type)
        {
            case PWM_DATA_ENABLE:
                result = pwm_ctrl_write_enable (pwm.id, data);
                break;
            case PWM_DATA_FREQ:
                result = pwm_ctrl_write_freq (pwm.id, data);
                break;
            case PWM_DATA_DUTY:
                result = pwm_ctrl_write_duty (pwm.id, data);
                break;
        }
    }

    if (result == SUCCESS)
    {
        printk ("[PWM%d] Written to board data=%d\n", GET_PWM_NR_FROM_ID(pwm.id), data);
    }

    return i;
}

// ------------------------------------------------------------ //

static int get_pwm_data_from_minor_nr (pwm_data* output)
{
    if (output == NULL)
        return ERROR;

    output->id = current_minor / NR_DEV_NODES_PER_PWM;

    output->data_type = current_minor % NR_DEV_NODES_PER_PWM;

    return SUCCESS;
}