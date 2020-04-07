#/**
* @file 	lcd_ctrl.c
* @brief 	Interface for reading/writing LCD values
*/

/************************************************************
*	Includes
************************************************************/

#include "lcd_ctrl.h"

#include <linux/kernel.h>				// kernel work
#include <linux/module.h>				// required for module

#include "es6_generic.h"				// for generic defines
#include "pp_iomem.h"                   // reuse from peek and poke
#include "bit_manipulation.h"           // for helper macros

/************************************************************
*	Defines
************************************************************/
#define LCD_CTRL_REG_ADDR           (0x31040018)

#define LCD_ENABLE_BITMASK          (0x00000001)
#define LCD_ENABLE_BIT_OFFSET       (0)

/************************************************************
*	Public functions
************************************************************/

int lcd_ctrl_write_enable (uint8_t new_value)
{
    int result = SUCCESS;

    unsigned long register_data;

    result = pp_iomem_read_4_bytes (LCD_CTRL_REG_ADDR, &register_data);

    if (result == SUCCESS)
    {
        if (new_value == LCD_ENABLE_FLAG)
            register_data = SET_NTH_BIT (register_data, LCD_ENABLE_BIT_OFFSET);
        else if (new_value == LCD_DISABLE_FLAG)
            register_data = CLEAR_NTH_BIT (register_data, LCD_ENABLE_BIT_OFFSET);
        else 
            result = -EINVAL;
    }

    if (result == SUCCESS)
    {
        result = pp_iomem_write_4_bytes (LCD_CTRL_REG_ADDR, register_data);
    }

    return result;
}