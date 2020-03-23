#ifndef _PWM_CTRL_H_
#define _PWM_CTRL_H_

#include <linux/types.h> // for uint8_t

typedef enum pwm_enum_t {
    PWM_UNKNOWN = 0,
    PWM_1, 
    PWM_2 
} pwm_enum;

int pwm_ctrl_read_enable (pwm_enum pwm, uint8_t* output);
int pwm_ctrl_write_enable (pwm_enum pwm, uint8_t new_value);

int pwm_ctrl_read_freq (pwm_enum pwm, int* output);
int pwm_ctrl_write_freq (pwm_enum pwm, int new_value);

int pwm_ctrl_read_duty (pwm_enum pwm, uint8_t* output);
int pwm_ctrl_write_duty (pwm_enum pwm, uint8_t new_value);

#endif