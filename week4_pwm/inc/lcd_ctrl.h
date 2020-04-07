#ifndef _LCD_CTRL_H_
#define _LCD_CTRL_H_

#include <linux/types.h> // for uint8_t

#define LCD_DISABLE_FLAG            (0)
#define LCD_ENABLE_FLAG             (1)

int lcd_ctrl_write_enable (uint8_t new_value);

#endif