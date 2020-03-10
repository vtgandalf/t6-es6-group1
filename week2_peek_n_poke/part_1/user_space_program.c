/*
* @file     user_space_program.c
* @brief    Read up counter address from user space
*/

#include <stdio.h>		// for printf()
#include <sys/types.h> 	// for getpid()
#include <unistd.h>		// for getpid()

#define COUNTER_ADDR	(0x40024000)

int main (void)
{
	int* addr=COUNTER_ADDR;
	printf ("addr=0x%x\n", addr);
	printf ("pid=%d\n", getpid());
	printf ("Press any key to read from address...");
	getchar();
	printf ("Now deferencing pointer. Expect segfault.\n");
	printf ("value=0x%x\n", *addr); // segfault 
	return 0;
}