/**
* @file 	pwm_ctrl.c
* @author 	Thanh Hoang
* @brief 	Interface for reading/writing PWM values
*/

/************************************************************
*	Includes
************************************************************/

#include "pwm_ctrl.h"

#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module

#include "es6_generic.h"				// for generic defines
#include "pp_iomem.h"                   // reuse from peek and poke
#include "bit_manipulation.h"           // for helper macros

/************************************************************
*	Defines
************************************************************/

#define PWM_CLK                         (13000000)

#define PWM1_CTRL_REG_ADDR              (0x4005C000)
#define PWM2_CTRL_REG_ADDR              (0x4005C004)

#define PWM_ENABLE_BITMASK              (0x80000000)
#define PWM_ENABLE_BIT_OFFSET           (31)
#define PWM_ENABLE_LEN_IN_BITS          (1)
#define PWM_ENABLE_FLAG                 (1)
#define PWM_DISABLE_FLAG                (0)

#define PWM_FREQ_BITMASK                (0x0000FF00)
#define PWM_FREQ_BIT_OFFSET             (8)
#define PWM_FREQ_LEN_IN_BITS            (8)
#define PWM_FREQ_MAX_RELOAD_VAL         (256)

#define PWM_DUTY_BITMASK                (0x000000FF)
#define PWM_DUTY_BIT_OFFSET             (0)
#define PWM_DUTY_LEN_IN_BITS            (8)
#define PWM_DUTY_MAX_DUTY_VAL           (256)

#define PWM_FREQ_FROM_RELOAD(x)         ((PWM_CLK/x)/PWM_FREQ_MAX_RELOAD_VAL)
#define PWM_FREQ_TO_RELOAD(f)           (PWM_CLK/(PWM_FREQ_MAX_RELOAD_VAL*f))
#define PWM_FREQ_MAX_VALUE              (50000)
#define PWM_FREQ_MIN_VALUE              (198)

#define PWM_MAX_DUTY_PERCENTAGE         (100)
#define PWM_DUTY_FROM_DUTY_VALUE(x)     (((PWM_DUTY_MAX_DUTY_VAL-x)*100)/PWM_DUTY_MAX_DUTY_VAL)
#define PWM_DUTY_TO_DUTY_VALUE(x)       (PWM_DUTY_MAX_DUTY_VAL - ((PWM_DUTY_MAX_DUTY_VAL*x)/100))

/************************************************************
*	Static data
************************************************************/

static int get_ctrl_reg_address (pwm_enum pwm, unsigned long* output);
static int read_data (pwm_enum pwm, unsigned long bitmask, int bit_offset, uint8_t* output);
static int write_data (pwm_enum pwm, int bit_offset, uint8_t new_byte_value, int nr_of_bits);

/************************************************************
*	Public functions
************************************************************/

int pwm_ctrl_read_enable (pwm_enum pwm, uint8_t* output)
{
    int result = SUCCESS;

    result = read_data (pwm, PWM_ENABLE_BITMASK, PWM_ENABLE_BIT_OFFSET, output);

    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_write_enable (pwm_enum pwm, uint8_t new_value)
{
    int result = SUCCESS;

    if (new_value != PWM_ENABLE_FLAG && new_value != PWM_DISABLE_FLAG)
    {
        printk ("[PWM control write] Invalid flag: %d\n", new_value);
        return -EINVAL;
    }
    
    result = write_data (pwm, PWM_ENABLE_BIT_OFFSET, new_value, PWM_ENABLE_LEN_IN_BITS);

    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_read_freq (pwm_enum pwm, int* output)
{
    int result = SUCCESS;

    uint8_t reload_value = 0;

    result = read_data (pwm, PWM_FREQ_BITMASK, PWM_FREQ_BIT_OFFSET, &reload_value);

    if (result == SUCCESS)
    {
        // Reload value 0 is treated as max value (256). See p.603 datasheet.
        *output =   (reload_value == 0) ? 
                    PWM_FREQ_FROM_RELOAD(PWM_FREQ_MAX_RELOAD_VAL) : 
                    PWM_FREQ_FROM_RELOAD(reload_value);
    }

    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_write_freq (pwm_enum pwm, int new_value)
{
    int result = SUCCESS;

    if (new_value < PWM_FREQ_MIN_VALUE || new_value > PWM_FREQ_MAX_VALUE)
    {
        printk (KERN_INFO "[PWM control write] Frequency out of range: %d\n", new_value);
        return -EINVAL;
    }

    uint8_t reload_value = PWM_FREQ_TO_RELOAD(new_value);

    result = write_data (pwm, PWM_FREQ_BIT_OFFSET, reload_value, PWM_FREQ_LEN_IN_BITS);

    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_read_duty (pwm_enum pwm, uint8_t* output)
{
    int result = SUCCESS;

    uint8_t duty_value = 0;

    result = read_data (pwm, PWM_DUTY_BITMASK, PWM_DUTY_BIT_OFFSET, &duty_value);

    if (result == SUCCESS)
    {
        *output = PWM_DUTY_FROM_DUTY_VALUE(duty_value);
    }
    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_write_duty (pwm_enum pwm, uint8_t new_value)
{
    int result = SUCCESS;

    if (new_value < 0 || new_value > PWM_MAX_DUTY_PERCENTAGE)
    {
        printk (KERN_INFO "[PWM control write] Duty out of range: %d\n", new_value);
        return -EINVAL;
    }

    uint8_t duty_value = PWM_DUTY_TO_DUTY_VALUE(new_value);

    result = write_data (pwm, PWM_DUTY_BIT_OFFSET, duty_value, PWM_DUTY_LEN_IN_BITS);
    
    return result;
}

/************************************************************
*	Static functions
************************************************************/

static int get_ctrl_reg_address (pwm_enum pwm, unsigned long* output)
{
    if (output == NULL)
        return -EINVAL;

    int result = SUCCESS;
    
    switch (pwm)
    {
        case PWM_1:
            *output = PWM1_CTRL_REG_ADDR;
            break;
        case PWM_2:
            *output = PWM2_CTRL_REG_ADDR;
            break;
        default:
            result = -EINVAL;
            break;
    }

    return result;
}

// ------------------------------------------------------------ //

static int read_data (pwm_enum pwm, unsigned long bitmask, int bit_offset, uint8_t* output)
{
    int result = SUCCESS;

    unsigned long ctrl_reg_addr;

    unsigned long ctrl_reg_data;

    result = get_ctrl_reg_address (pwm, &ctrl_reg_addr);
    
    if (result == SUCCESS)
    {
        result = pp_iomem_read_4_bytes (ctrl_reg_addr, &ctrl_reg_data);
    }

    if (result == SUCCESS)
    {
        *output = (ctrl_reg_data & bitmask) >> bit_offset;
    }

    return result;
}

// ------------------------------------------------------------ //

static int write_data (pwm_enum pwm, int bit_offset, uint8_t new_byte_value, int nr_of_bits)
{
    if (nr_of_bits > NR_BITS_IN_A_BYTE)
        return -EINVAL;

    int result = SUCCESS;

    unsigned long ctrl_reg_addr;
    
    unsigned long ctrl_reg_data;

    result = get_ctrl_reg_address (pwm, &ctrl_reg_addr);

    if (result == SUCCESS)
    {
        result = pp_iomem_read_4_bytes (ctrl_reg_addr, &ctrl_reg_data);
    }

    if (result == SUCCESS)
    {
        result = write_n_bits_from_src_to_dest (new_byte_value, &ctrl_reg_data, nr_of_bits, bit_offset);
    }

    if (result == SUCCESS)
    {
        result = pp_iomem_write_4_bytes (ctrl_reg_addr, ctrl_reg_data);
    }

    return result;
}