#include <stdio.h>

int main(void)
{
    char* rtc_up_counter = (char*)0x40024000;
    printf("%c",(char)*rtc_up_counter);
    return 0;
}
