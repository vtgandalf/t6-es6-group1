#include<stdio.h>

int main()
{
    char* addr = 0x40024000;
    std::cout << "Data from upcounter with address 0x40024000: %i\n", *addr);
    return 0;
}