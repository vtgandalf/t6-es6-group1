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

PWM_CLK_DEV_ENABLE=/dev/pwm${PWM_ID}_clk_enable
PWM_CLK_DEV_SRC=/dev/pwm${PWM_ID}_clk_src
PWM_CLK_DEV_FREQ_DIV=/dev/pwm${PWM_ID}_clk_freq_div

echo "======== TEST R/W ENABLE PWM$PWM_ID CLOCK ======="
echo ""

cat $PWM_CLK_DEV_ENABLE
echo 1 > $PWM_CLK_DEV_ENABLE
cat $PWM_CLK_DEV_ENABLE
echo ""

echo 0 > $PWM_CLK_DEV_ENABLE
cat $PWM_CLK_DEV_ENABLE
echo ""

echo "Try to write 3. "
echo 3 > $PWM_CLK_DEV_ENABLE
cat $PWM_CLK_DEV_ENABLE
echo ""

echo "Try to write -1. "
echo -1 > $PWM_CLK_DEV_ENABLE
cat $PWM_CLK_DEV_ENABLE
echo ""

echo "======== TEST R/W SRC PWM$PWM_ID CLOCK ======="
echo ""

cat $PWM_CLK_DEV_SRC
echo 1 > $PWM_CLK_DEV_SRC
cat $PWM_CLK_DEV_SRC
echo ""

echo 0 > $PWM_CLK_DEV_SRC
cat $PWM_CLK_DEV_SRC
echo ""

echo "Try to write 3. "
echo 3 > $PWM_CLK_DEV_SRC
cat $PWM_CLK_DEV_SRC
echo ""

echo "Try to write -1. "
echo -1 > $PWM_CLK_DEV_SRC
cat $PWM_CLK_DEV_SRC
echo ""

echo "======== TEST R/W FREQ DIV PWM$PWM_ID CLOCK ======="
echo ""

cat $PWM_CLK_DEV_FREQ_DIV
echo 15 > $PWM_CLK_DEV_FREQ_DIV
cat $PWM_CLK_DEV_FREQ_DIV
echo ""

echo 5 > $PWM_CLK_DEV_FREQ_DIV
cat $PWM_CLK_DEV_FREQ_DIV
echo ""

echo "Try to write 150. "
echo 150 > $PWM_CLK_DEV_FREQ_DIV
cat $PWM_CLK_DEV_FREQ_DIV
echo ""

echo "Try to write -10. "
echo -10 > $PWM_CLK_DEV_FREQ_DIV
cat $PWM_CLK_DEV_FREQ_DIV
echo ""