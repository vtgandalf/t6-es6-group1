#ifndef _PWM_CLK_CTRL_H_
#define _PWM_CLK_CTRL_H_

#include <linux/types.h> // for uint8_t

#include "pwm_enum.h"

int pwm_clk_ctrl_get_frequency (pwm_enum pwm, int* frequency);

int pwm_clk_ctrl_read_enable (pwm_enum pwm, uint8_t* output);
int pwm_clk_ctrl_write_enable (pwm_enum pwm, uint8_t new_value);

int pwm_clk_ctrl_read_src (pwm_enum pwm, uint8_t* output);
int pwm_clk_ctrl_write_src (pwm_enum pwm, uint8_t new_value);

int pwm_clk_ctrl_read_freq_div (pwm_enum pwm, uint8_t* output);
int pwm_clk_ctrl_write_freq_div (pwm_enum pwm, uint8_t new_value);

#endif