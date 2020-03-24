# echo "r 4005C000 1" > /sys/kernel/lpc_comm/lpc_cmd
# echo "w 4005C000 80000000" > /sys/kernel/lpc_comm/lpc_cmd
# echo "r 4005C000 1" > /sys/kernel/lpc_comm/lpc_cmd

echo "=== Test setup ==="
echo "EXPECT PWM1: enable ON, freq 42, duty cycle 50"
echo "w 4005C000 8000AA80" > /sys/kernel/lpc_comm/lpc_cmd

echo "EXPECT PWM2: enable ON, freq 199, duty cycle 94"
echo "w 4005C004 8000FE0F" > /sys/kernel/lpc_comm/lpc_cmd

echo "=== Now reading ==="
echo "PWM1 Enable"
cat /dev/pwm1_enable
echo "PWM1 Frequency"
cat /dev/pwm1_freq
echo "PWM1 Duty"
cat /dev/pwm1_duty

echo ""

echo "PWM2 Enable"
cat /dev/pwm2_enable
echo "PWM2 Frequency"
cat /dev/pwm2_freq
echo "PWM2 Duty"
cat /dev/pwm2_duty