if [ -z "$PWM_ID" ]; 
then echo "Set variable PWM_ID before running"
echo "Command: export PWM_ID=[id]"
exit 1
fi;

if [ "$PWM_ID" != 1 -a "$PWM_ID" != 2 ];
then echo "Invalid PWM ID: ${PWM_ID}"
echo "PWM ID's allowed: 1 and 2"
exit 1
fi;

PWM_DEV_ENABLE=/dev/pwm${PWM_ID}_enable
PWM_DEV_FREQ=/dev/pwm${PWM_ID}_freq
PWM_DEV_DUTY=/dev/pwm${PWM_ID}_duty

PWM_CLK_DEV_ENABLE=/dev/pwm${PWM_ID}_clk_enable
PWM_CLK_DEV_SRC=/dev/pwm${PWM_ID}_clk_src
PWM_CLK_DEV_FREQ_DIV=/dev/pwm${PWM_ID}_clk_freq_div

echo "======== TEST R/W ENABLE PWM$PWM_ID ======="
echo ""

cat $PWM_DEV_ENABLE
echo 1 > $PWM_DEV_ENABLE
cat $PWM_DEV_ENABLE
echo ""

echo 0 > $PWM_DEV_ENABLE
cat $PWM_DEV_ENABLE
echo ""

echo "Try to write 3. "
echo 3 > $PWM_DEV_ENABLE
cat $PWM_DEV_ENABLE
echo ""

echo "Try to write -1. "
echo -1 > $PWM_DEV_ENABLE
cat $PWM_DEV_ENABLE
echo ""

echo "======== TEST R/W FREQ PWM$PWM_ID ======="
echo ""

cat $PWM_DEV_FREQ
echo 1000 > $PWM_DEV_FREQ
cat $PWM_DEV_FREQ
echo ""

echo 198 > $PWM_DEV_FREQ
cat $PWM_DEV_FREQ
echo ""

echo "Try to write 60000. "
echo 60000 > $PWM_DEV_FREQ
cat $PWM_DEV_FREQ
echo ""

echo "Try to write -500. "
echo -500 > $PWM_DEV_FREQ
cat $PWM_DEV_FREQ
echo ""

echo "======== TEST R/W DUTY PWM$PWM_ID ======="
echo ""

cat $PWM_DEV_DUTY
echo 50 > $PWM_DEV_DUTY
cat $PWM_DEV_DUTY
echo ""

echo 25 > $PWM_DEV_DUTY
cat $PWM_DEV_DUTY
echo ""

echo "Try to write 150. "
echo 150 > $PWM_DEV_DUTY
cat $PWM_DEV_DUTY
echo ""

echo "Try to write -10. "
echo -10 > $PWM_DEV_DUTY
cat $PWM_DEV_DUTY
echo ""