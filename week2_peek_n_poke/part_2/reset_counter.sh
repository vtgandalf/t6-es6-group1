
echo "r 40024000 1" > /sys/kernel/lpc_comm/lpc_cmd

echo "w 40024010 10" > /sys/kernel/lpc_comm/lpc_cmd
echo "w 40024010 00" > /sys/kernel/lpc_comm/lpc_cmd

echo "r 40024000 1" > /sys/kernel/lpc_comm/lpc_cmd
