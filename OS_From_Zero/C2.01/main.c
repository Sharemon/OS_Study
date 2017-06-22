#include <stdio.h>
#include "myOS.h"

#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET	0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI	0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)
	
int flag;

void delay(int n)
{
	while(--n);
}


unsigned long * stackPtr;
unsigned long stack[1024];

void trigPendSVC()
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

Task_t ttask1;
Task_t ttask2;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];

void TaskInit(Task_t * tTaskPtr, void (*entry)(void *), 
		void * param, TaskStack_t * stack)
{
	tTaskPtr->stackPtr = stack;
}

void task1(void * param)
{
	for(;;)
	{
		;
	}
}

void task2(void * param)
{
	for(;;)
	{
		;
	}
}

extern void PendSV(void);

int main(void)
{
	stackPtr = &stack[1024];		// Why 1024, but not 1023? Because we use STMDB in 'switch.c'. 
															// DB means 'decrease before', so the actual first store address will be &stack[1023]
	
	// Initial task
	TaskInit(&ttask1, task1, (void *)0x11111111, &task1Env[1024]);
	TaskInit(&ttask2, task2, (void *)0x22222222, &task2Env[1024]);
	
	for (;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		PendSV();
	}
	
	return 0;
}