echo "r 4005C000 1" > /sys/kernel/lpc_comm/lpc_cmd
echo "w 4005C000 80000000" > /sys/kernel/lpc_comm/lpc_cmd
echo "r 4005C000 1" > /sys/kernel/lpc_comm/lpc_cmd

echo "PWM1: enable ON, freq 42, duty cycle 50"
echo "w 4005C000 8000AA80" > /sys/kernel/lpc_comm/lpc_cmd

echo "PWM2: enable ON, freq 199, duty cycle 94"
echo "w 4005C004 8000FE0F" > /sys/kernel/lpc_comm/lpc_cmd
