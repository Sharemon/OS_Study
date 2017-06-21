#include <stdio.h>

#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET	0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI	0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)
	
unsigned long * stackPtr;
unsigned long stack[1024];

void trigPendSVC()
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

int main(void)
{
	stackPtr = &stack[1024];		// Why 1024, but not 1023? Because we use STMDB in 'switch.c'. 
															// DB means 'decrease before', so the actual first store address will be &stack[1023]
	
	for (;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		trigPendSVC();
	}
	
	return 0;
}