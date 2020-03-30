/**
* @file 	pwm_devfs.c
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
#include "pwm_clk_ctrl.h"       // for r/w to pwm clock
#include "pwm_ctrl.h"           // for r/w to pwm control 

/************************************************************
*	Defines
************************************************************/
#define MAX_USER_INPUT_LEN      (256)

#define NR_OF_PWMS              (2)
#define NR_DEV_NODES_PER_PWM    (6)
#define NR_OF_MINOR_HANDLERS    (NR_OF_PWMS*NR_DEV_NODES_PER_PWM)

// Keep data types consistent with indices of data handlers table
#define PWM_DATA_ENABLE         (0)
#define PWM_DATA_FREQ           (1)
#define PWM_DATA_DUTY           (2)
#define PWM_DATA_CLK_ENABLE     (3)
#define PWM_DATA_CLK_SOURCE     (4)
#define PWM_DATA_CLK_FREQ_DIV   (5)

typedef struct pwm_data_t {
    pwm_enum    id;
    int         data_type;
} pwm_data;

typedef struct pwm_data_handler_t {
    // int holds enough space for uint8_t
    int (*read) (pwm_enum, int*);
    int (*write) (pwm_enum, int);
} pwm_data_handler;

/************************************************************
*	Static data
************************************************************/
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int is_device_open = FALSE;

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write
};

/** @brief Handler lookup table, indexed by data type (PWM_DATA_[X]) */
static const pwm_data_handler pwm_handlers_table[NR_DEV_NODES_PER_PWM] = {
    { pwm_ctrl_read_enable,         pwm_ctrl_write_enable },
    { pwm_ctrl_read_freq,           pwm_ctrl_write_freq },
    { pwm_ctrl_read_duty,           pwm_ctrl_write_duty },
    { pwm_clk_ctrl_read_enable,     pwm_clk_ctrl_write_enable },
    { pwm_clk_ctrl_read_src,        pwm_clk_ctrl_write_src},
    { pwm_clk_ctrl_read_freq_div,   pwm_clk_ctrl_write_freq_div}  
};

/** @brief PWM data look up table, indexed by minor number */
static const pwm_data pwm_data_table[NR_OF_MINOR_HANDLERS] = {
    { PWM_1, PWM_DATA_ENABLE },         // minor 0: /dev/pwm1_enable
    { PWM_1, PWM_DATA_FREQ },           // minor 1: /dev/pwm1_freq
    { PWM_1, PWM_DATA_DUTY },           // minor 2: /dev/pwm1_duty
    { PWM_1, PWM_DATA_CLK_ENABLE },     // minor 3: /dev/pwm1_clk_enable
    { PWM_1, PWM_DATA_CLK_SOURCE },     // minor 4: /dev/pwm1_clk_src
    { PWM_1, PWM_DATA_CLK_FREQ_DIV },   // minor 5: /dev/pwm1_clk_freq_div
    { PWM_2, PWM_DATA_ENABLE },         // minor 6: /dev/pwm2_enable
    { PWM_2, PWM_DATA_FREQ },           // minor 7: /dev/pwm2_freq
    { PWM_2, PWM_DATA_DUTY },           // minor 8: /dev/pwm2_duty
    { PWM_2, PWM_DATA_CLK_ENABLE },     // minor 9: /dev/pwm2_clk_enable
    { PWM_2, PWM_DATA_CLK_SOURCE },     // minor 10: /dev/pwm2_clk_src
    { PWM_2, PWM_DATA_CLK_FREQ_DIV }    // minor 11: /dev/pwm2_clk_freq_div
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
        return -ENOMEM;
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
        printk ("[device_read  PWM%d] data=%d, bytes_processed=%d\n", pwm->id + 1, data, bytes_processed);
        data = 0;
        bytes_processed = 0;
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
            result = -EINVAL;
        }
    }

    if (result == SUCCESS)
    {
        printk (KERN_INFO "[device_write  PWM%d] data=%d\n", pwm->id + 1, data);
        result = pwm_handlers_table[pwm->data_type].write(pwm->id, data);
    }
    
    return i;
}