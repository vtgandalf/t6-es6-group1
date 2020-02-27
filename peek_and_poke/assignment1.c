#include<stdio.h>

int main()
{
    char* addr =  0x40024000;
    printf("Trying to read data from addr 0x%x ...\n", addr);
    printf("Data from upcounter with addr 0x40024000: %i\n", *addr);
    return 0;
}
