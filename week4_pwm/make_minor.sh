echo "Making minor files for PWM 1"
mknod /dev/pwm1_enable c 250 0
mknod /dev/pwm1_freq c 250 1
mknod /dev/pwm1_duty c 250 2
mknod /dev/pwm1_clk_enable c 250 3
mknod /dev/pwm1_clk_src c 250 4
mknod /dev/pwm1_clk_freq_div c 250 5
echo "Making minor files for PWM 2"
mknod /dev/pwm2_enable c 250 6
mknod /dev/pwm2_freq c 250 7
mknod /dev/pwm2_duty c 250 8
mknod /dev/pwm2_clk_enable c 250 9
mknod /dev/pwm2_clk_src c 250 10
mknod /dev/pwm2_clk_freq_div c 250 11
echo "Done. Now listing available minor files:"
ls -al /dev/pwm*