#include "pwm_devfs.h"

#include <linux/kernel.h>       // kernel work 
#include <linux/module.h>       // required 
#include <linux/fs.h>           // for file operations
#include <linux/kdev_t.h>       // for MINOR macro
#include <asm/uaccess.h>        // for put_user
#include <linux/types.h>

#include "es6_generic.h"        // generic defines
#include "pwm_ctrl.h"           // for pwm control 

#define MAX_INTERNAL_BUF        (256)
#define NR_DEV_NODES_PER_PWM    (3)
#define NR_OF_PWM_SUPPORTED     (2)

#define MINOR_UNKNOWN           (-1)
#define MINOR_ENABLE            (0)
#define MINOR_FREQ              (1)
#define MINOR_DUTY              (2)

#define MINOR_START_PWM1        (0)
#define MINOR_END_PWM1          (3)

#define MINOR_START_PWM2        (4)
#define MINOR_END_PWM2          (6)

#define MIN_USER_BUF_SIZE       (1)

typedef struct pwm_data_t {
    int id;
    int data_type;
} pwm_data;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int get_pwm_data_from_minor_nr (pwm_data* output);

static unsigned int current_minor = MINOR_UNKNOWN;
static int is_device_open = FALSE;

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write
};

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

void pwm_devfs_cleanup (void)
{
    unregister_chrdev (PWM_DEVFS_MAJOR_NUMBER, PWM_DEVFS_DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file)
{
    int result = (is_device_open) ? -EBUSY : SUCCESS;

    printk (KERN_INFO "device_open()\n");

    if (result == SUCCESS)
    {
        is_device_open = TRUE;
        current_minor = MINOR(inode->i_rdev);
        printk (KERN_INFO "me minor number = %d\n", current_minor);
        try_module_get (THIS_MODULE);
    }

    return result;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk (KERN_INFO "device_release()\n");
    is_device_open = FALSE;
    current_minor = MINOR_UNKNOWN;
    module_put(THIS_MODULE);
    return SUCCESS;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t size, loff_t * offset)
{
    printk (KERN_INFO "device_read()\n");

    if (size < MIN_USER_BUF_SIZE)
        return -ENOMEM;

    int result = SUCCESS;
    uint8_t data = 0;
    pwm_data pwm = {0};

    result = get_pwm_data_from_minor_nr (&pwm);

    printk (KERN_INFO "pwm id = %d; type =%d\n", pwm.id, pwm.data_type);

    if (result == SUCCESS)
    {
        switch (pwm.data_type)
        {
            case MINOR_ENABLE:
                result = pwm_ctrl_read_enable (pwm.id, &data);
                break;
            case MINOR_FREQ:
                result = pwm_ctrl_read_freq (pwm.id, &data);
                break;
            case MINOR_DUTY:
                result = pwm_ctrl_read_duty (pwm.id, &data);
                break;
        }
    }

    if (result == SUCCESS)
    {
        printk ("data = %d\n", data);
        result = put_user (data, buffer);
    }
    
    return (result == SUCCESS) ? 0 : -EFAULT;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    printk (KERN_INFO "device_write()\n");
    return SUCCESS;
}

static int get_pwm_data_from_minor_nr (pwm_data* output)
{
    if (output == NULL)
        return ERROR;

    output->id = current_minor / NR_DEV_NODES_PER_PWM;
    output->data_type = current_minor % NR_DEV_NODES_PER_PWM;

    return SUCCESS;
}