#ifndef _PWM_CTRL_H_
#define _PWM_CTRL_H_

#include <linux/types.h> // for uint8_t

int pwm_ctrl_read_enable (int pwm_id, uint8_t* output);
int pwm_ctrl_read_freq (int pwm_id, uint8_t* output);
int pwm_ctrl_read_duty (int pwm_id, uint8_t* output);

#endif