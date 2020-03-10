#ifndef _PWM_DEVFS_H_
#define _PWM_DEVFS_H_

#define PWM_DEVFS_MAJOR_NUMBER      (250)
#define PWM_DEVFS_DEVICE_NAME       "es6_pwm"

int pwm_devfs_initialize (void);
void pwm_devfs_cleanup (void);

#endif