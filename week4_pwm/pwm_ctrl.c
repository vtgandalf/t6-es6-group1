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

/************************************************************
*	Includes
************************************************************/

#define PWM_CLK                         (13000000)

#define PWM1_ID                         (0)
#define PWM2_ID                         (1)

#define PWM1_CTRL_REG_ADDR              (0x4005C000)
#define PWM2_CTRL_REG_ADDR              (0x4005C004)

#define PWM_ENABLE_BITMASK              (0x80000000)
#define PWM_ENABLE_START_BIT_NR         (31)

#define PWM_FREQ_BITMASK                (0x0000FF00)
#define PWM_FREQ_START_BIT_NR           (8)
#define PWM_FREQ_MAX_RELOAD_VAL         (256)

#define PWM_DUTY_BITMASK                (0x000000FF)
#define PWM_DUTY_START_BIT_NR           (0)
#define PWM_DUTY_MAX_DUTY_VAL           (256)

#define PWM_FREQ_FROM_RELOAD(x)         ((PWM_CLK/x)/PWM_FREQ_MAX_RELOAD_VAL)
#define PWM_FREQ_TO_RELOAD(f)           (PWM_CLK/(PWM_FREQ_MAX_RELOAD_VAL*f))

#define PWM_DUTY_FROM_DUTY_VALUE(x)     (((PWM_DUTY_MAX_DUTY_VAL-x)*100)/PWM_DUTY_MAX_DUTY_VAL)
#define PWM_DUTY_TO_DUTY_VALUE(x)       (PWM_DUTY_MAX_DUTY_VAL - ((PWM_DUTY_MAX_DUTY_VAL*x)/100))

#define NR_BITS_IN_A_BYTE               (8)
#define GET_NTH_BIT(v,n)                ((v & (1 << n)) >> n)
#define SET_NTH_BIT(v,n)                (v | (1 << n))
#define CLEAR_NTH_BIT(v,n)              (v & ~(1<<n))

/************************************************************
*	Static data
************************************************************/

static int get_ctrl_reg_address (int pwm_id, unsigned long* output);
static int read_data (int pwm_id, unsigned long bitmask, int start_bit_nr, uint8_t* output);
static int write_data (int pwm_id, int start_bit_nr, uint8_t new_byte_value);

/************************************************************
*	Public functions
************************************************************/

int pwm_ctrl_read_enable (int pwm_id, uint8_t* output)
{
    if (output == NULL)
        return ERROR;

    int result = SUCCESS;

    result = read_data (pwm_id, PWM_ENABLE_BITMASK, PWM_ENABLE_START_BIT_NR, output);

    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_write_enable (int pwm_id, int new_value)
{
    int result = SUCCESS;
    
    result = write_data (pwm_id, PWM_ENABLE_START_BIT_NR, (uint8_t)(new_value));

    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_read_freq (int pwm_id, int* output)
{
    if (output == NULL)
        return ERROR;
    
    int result = SUCCESS;

    uint8_t reload_value = 0;

    result = read_data (pwm_id, PWM_FREQ_BITMASK, PWM_FREQ_START_BIT_NR, &reload_value);

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

int pwm_ctrl_write_freq (int pwm_id, int new_value)
{
    int result = SUCCESS;

    uint8_t reload_value = PWM_FREQ_TO_RELOAD(new_value);

    result = write_data (pwm_id, PWM_FREQ_START_BIT_NR, reload_value);

    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_read_duty (int pwm_id, uint8_t* output)
{
    if (output == NULL)
        return ERROR;

    int result = SUCCESS;

    uint8_t duty_value = 0;

    result = read_data (pwm_id, PWM_DUTY_BITMASK, PWM_DUTY_START_BIT_NR, &duty_value);

    if (result == SUCCESS)
    {
        *output = PWM_DUTY_FROM_DUTY_VALUE(duty_value);
    }
    return result;
}

// ------------------------------------------------------------ //

int pwm_ctrl_write_duty (int pwm_id, int new_value)
{
    int result = SUCCESS;

    uint8_t duty_value = PWM_DUTY_TO_DUTY_VALUE(new_value);

    result = write_data (pwm_id, PWM_DUTY_START_BIT_NR, duty_value);
    
    return result;
}

/************************************************************
*	Static functions
************************************************************/

static int get_ctrl_reg_address (int pwm_id, unsigned long* output)
{
    if (output == NULL)
        return ERROR;

    int result = SUCCESS;
    
    switch (pwm_id)
    {
        case PWM1_ID:
            *output = PWM1_CTRL_REG_ADDR;
            break;
        case PWM2_ID:
            *output = PWM2_CTRL_REG_ADDR;
            break;
        default:
            result = ERROR;
            break;
    }

    return result;
}

// ------------------------------------------------------------ //

static int read_data (int pwm_id, unsigned long bitmask, int start_bit_nr, uint8_t* output)
{
    int result = SUCCESS;

    unsigned long ctrl_reg_addr;

    unsigned long ctrl_reg_data;

    result = get_ctrl_reg_address (pwm_id, &ctrl_reg_addr);
    
    if (result == SUCCESS)
    {
        result = pp_iomem_read_4_bytes (ctrl_reg_addr, &ctrl_reg_data);
    }

    if (result == SUCCESS)
    {
        *output = (ctrl_reg_data & bitmask) >> start_bit_nr;
    }

    return result;
}

// ------------------------------------------------------------ //

static int write_data (int pwm_id, int start_bit_nr, uint8_t new_byte_value)
{
    int result = SUCCESS;

    unsigned long ctrl_reg_addr;
    
    unsigned long ctrl_reg_data;

    result = get_ctrl_reg_address (pwm_id, &ctrl_reg_addr);

    if (result == SUCCESS)
    {
        result = pp_iomem_read_4_bytes (ctrl_reg_addr, &ctrl_reg_data);
    }

    if (result == SUCCESS)
    {
        int bit_pos = 0;

        for (bit_pos = 0; bit_pos < NR_BITS_IN_A_BYTE; bit_pos++)
        {
            int bit_pos_with_offset = bit_pos + start_bit_nr;
            uint8_t bit_value = GET_NTH_BIT(new_byte_value, bit_pos);
            ctrl_reg_data = (bit_value == 0) ?
                            CLEAR_NTH_BIT(ctrl_reg_data, bit_pos_with_offset) :
                            SET_NTH_BIT(ctrl_reg_data, bit_pos_with_offset);
        }

        result = pp_iomem_write_4_bytes (ctrl_reg_addr, ctrl_reg_data);
    }

    return result;
}