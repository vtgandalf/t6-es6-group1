/**
* @file 	pwm_clk_ctrl.c
* @brief 	Interface for controlling PWM clock source
*/

/************************************************************
*	Includes
************************************************************/

#include "pwm_clk_ctrl.h"

#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module

#include "es6_generic.h"				// for generic defines
#include "pp_iomem.h"                   // reuse from peek and poke
#include "bit_manipulation.h"           // for helper macros

/************************************************************
*	Defines
************************************************************/

#define PWM_CLK_CTRL_REG_ADDR           (0x400040B8)

#define PWM1_CLK_EN_BITMASK             (0x00000001)
#define PWM1_CLK_EN_BIT_OFFSET          (0)

#define PWM1_CLK_SRC_BITMASK            (0x00000002)
#define PWM1_CLK_SRC_BIT_OFFSET         (1)

#define PWM1_CLK_FREQ_DIV_BITMASK       (0x000000F0)
#define PWM1_CLK_FREQ_DIV_BIT_OFFSET    (4)

#define PWM2_CLK_EN_BITMASK             (0x00000004)
#define PWM2_CLK_EN_BIT_OFFSET          (2)

#define PWM2_CLK_SRC_BITMASK            (0x00000008)
#define PWM2_CLK_SRC_BIT_OFFSET         (3)

#define PWM2_CLK_FREQ_DIV_BITMASK       (0x00000F00)
#define PWM2_CLK_FREQ_DIV_BIT_OFFSET    (8)

#define PWM_CLK_EN_LEN_IN_BITS          (1)
#define PWM_CLK_SRC_LEN_IN_BITS         (1)
#define PWM_CLK_FREQ_DIV_LEN_IN_BITS    (4)

// Clock data
#define PWM_CLK_CTRL_NR_OF_SRC          (2)
#define PWM_CLK_CTRL_RTC_CLK            (0)
#define PWM_CLK_CTRL_PERIPH_CLK         (1)
#define PWM_CLK_CTRL_RTC_FREQ           (32000)
#define PWM_CLK_CTRL_PERIPH_FREQ        (13000000)

// For input checks
#define PWM_CLK_ENABLE_FLAG             (1)
#define PWM_CLK_DISABLE_FLAG            (0)
#define PWM_CLK_MAX_FREQ_DIV            (15)

typedef enum pwm_clk_data_enum_t {
    PWM_CLK_DATA_EN = 0,
    PWM_CLK_DATA_SRC,
    PWM_CLK_DATA_FREQ_DIV
} pwm_clk_data_enum;

/************************************************************
*	Static data
************************************************************/

static int read_data (pwm_enum pwm, pwm_clk_data_enum data_type, uint8_t* output);
static int write_data (pwm_enum pwm, pwm_clk_data_enum data_type, uint8_t new_value);
static int get_bitmask_and_offset (pwm_enum pwm, pwm_clk_data_enum data_type, unsigned long* bitmask, int* offset);

// Clock frequencies array, indexed by clock source flag
static const int clock_frequencies[PWM_CLK_CTRL_NR_OF_SRC] = { 
    PWM_CLK_CTRL_RTC_FREQ, 
    PWM_CLK_CTRL_PERIPH_FREQ 
};

/************************************************************
*	Public functions
************************************************************/

int pwm_clk_ctrl_get_frequency (pwm_enum pwm, int* frequency)
{
    if (frequency == NULL)
        return -EINVAL;

    int result = SUCCESS;
    
    uint8_t clock_source;

    uint8_t divider; 

    result = pwm_clk_ctrl_read_src (pwm, &clock_source);

    if (result == SUCCESS)
    {
        result = pwm_clk_ctrl_read_freq_div (pwm, &divider);
    }

    if (divider == 0)
    {
        result = -ENODATA;
    }

    if (result == SUCCESS)
    {
        *frequency = clock_frequencies[clock_source] / divider;
    }
    
    return result;
}

// ------------------------------------------------------------ //

int pwm_clk_ctrl_read_enable (pwm_enum pwm, uint8_t* output)
{
    int result = SUCCESS;

    result = read_data (pwm, PWM_CLK_DATA_EN, output);

    return result;
}

// ------------------------------------------------------------ //

int pwm_clk_ctrl_write_enable (pwm_enum pwm, uint8_t new_value)
{
    if (new_value != PWM_CLK_ENABLE_FLAG
    &&  new_value != PWM_CLK_DISABLE_FLAG)
    {
        return -EINVAL;
    }

    int result = SUCCESS;

    result = write_data (pwm, PWM_CLK_DATA_EN, new_value);
        
    return result;
}

// ------------------------------------------------------------ //

int pwm_clk_ctrl_read_src (pwm_enum pwm, uint8_t* output)
{
    int result = SUCCESS;

    result = read_data (pwm, PWM_CLK_DATA_SRC, output);
    
    return result; 
}

// ------------------------------------------------------------ //

int pwm_clk_ctrl_write_src (pwm_enum pwm, uint8_t new_value)
{   
    if (new_value != PWM_CLK_CTRL_RTC_CLK
    &&  new_value != PWM_CLK_CTRL_PERIPH_CLK)
    {
        return -EINVAL;
    }

    int result = SUCCESS;

    result = write_data (pwm, PWM_CLK_DATA_SRC, new_value);

    return result;
}

// ------------------------------------------------------------ //

int pwm_clk_ctrl_read_freq_div (pwm_enum pwm, uint8_t* output)
{    
    int result = SUCCESS;

    result = read_data (pwm, PWM_CLK_DATA_FREQ_DIV, output);

    return result;
}

// ------------------------------------------------------------ //

int pwm_clk_ctrl_write_freq_div (pwm_enum pwm, uint8_t new_value)
{
    if (new_value < 0 || new_value > PWM_CLK_MAX_FREQ_DIV)
    {
        return -EINVAL;
    }
    
    int result = SUCCESS;

    result = write_data (pwm, PWM_CLK_DATA_FREQ_DIV, new_value);

    return result;
}

/************************************************************
*	Static functions
************************************************************/

static int read_data (pwm_enum pwm, pwm_clk_data_enum data_type, uint8_t* output)
{
    if (output == NULL)
        return -EINVAL;

    int result = SUCCESS;

    int offset;

    unsigned long bitmask;

    unsigned long register_data;    

    result = get_bitmask_and_offset (pwm, data_type, &bitmask, &offset);

    if (result == SUCCESS)
    {
        result = pp_iomem_read_4_bytes (PWM_CLK_CTRL_REG_ADDR, &register_data);
    }

    if (result == SUCCESS)
    {
        *output = (register_data & bitmask) >> offset;
    }

    return result;
}

// ------------------------------------------------------------ //

static int write_data (pwm_enum pwm, pwm_clk_data_enum data_type, uint8_t new_value)
{
    int result = SUCCESS;
    
    int data_len = 0;

    switch (data_type)
    {
        case PWM_CLK_DATA_EN:
            data_len = PWM_CLK_EN_LEN_IN_BITS;
            break;
        case PWM_CLK_DATA_SRC:
            data_len = PWM_CLK_SRC_LEN_IN_BITS;
            break;
        case PWM_CLK_DATA_FREQ_DIV:
            data_len = PWM_CLK_FREQ_DIV_LEN_IN_BITS;
            break;
        default:
            result = -EINVAL;
            break;
    }
    
    int offset;
    
    unsigned long bitmask;

    unsigned long register_data;
    
    if (result == SUCCESS)
    {
        result = get_bitmask_and_offset (pwm, data_type, &bitmask, &offset);
    }

    if (result == SUCCESS)
    {
        result = pp_iomem_read_4_bytes (PWM_CLK_CTRL_REG_ADDR, &register_data);
    }

    if (result == SUCCESS)
    {
        result = write_n_bits_from_src_to_dest (new_value, &register_data, data_len, offset);
    }

    if (result == SUCCESS)
    {
        result = pp_iomem_write_4_bytes (PWM_CLK_CTRL_REG_ADDR, register_data);
    }

    return result;
}

// ------------------------------------------------------------ //

static int get_bitmask_and_offset (pwm_enum pwm, pwm_clk_data_enum data_type, unsigned long* bitmask, int* offset)
{
    if (bitmask == NULL || offset == NULL)
        return -EINVAL;

    int result = SUCCESS;

    switch (data_type)
    {
        case PWM_CLK_DATA_EN:
            *bitmask = (pwm == PWM_1 ? PWM1_CLK_EN_BITMASK : PWM2_CLK_EN_BITMASK);
            *offset = (pwm == PWM_1 ? PWM1_CLK_EN_BIT_OFFSET : PWM2_CLK_EN_BIT_OFFSET);
            break;
        case PWM_CLK_DATA_SRC:
            *bitmask = (pwm == PWM_1 ? PWM1_CLK_SRC_BITMASK : PWM2_CLK_SRC_BITMASK);
            *offset = (pwm == PWM_1 ? PWM1_CLK_SRC_BIT_OFFSET : PWM2_CLK_SRC_BIT_OFFSET);
            break;
        case PWM_CLK_DATA_FREQ_DIV:
            *bitmask = (pwm == PWM_1 ? PWM1_CLK_FREQ_DIV_BITMASK : PWM2_CLK_FREQ_DIV_BITMASK);
            *offset = (pwm == PWM_1 ? PWM1_CLK_FREQ_DIV_BIT_OFFSET : PWM2_CLK_FREQ_DIV_BIT_OFFSET);
            break;
        default:
            result = -EINVAL;
            break; 
    }

    return result;
}