#include <stdio.h>

int main(void)
{
    (unsigned long)* rtc_up_counter = ((unsigned long)*)0x40024000;
    printf("%ul",(unsigned long)*rtc_up_counter);
    return 0;
}
