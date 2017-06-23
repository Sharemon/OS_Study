#include <stdio.h>
#include "myOS.h"
#include "ARMCM3.h"

	
Task_t * curTask;
Task_t * nxtTask;
Task_t * taskTable[2];

Task_t tcbtask1;
Task_t tcbtask2;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];

void delay(int n)
{
	while(--n);
}

void trigPendSVC()
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void TaskSched()
{
	if (curTask == taskTable[0])
		nxtTask = taskTable[1];
	else
		nxtTask = taskTable[0];
	
	TaskSwitch();
}


void TaskInit(Task_t * tTaskPtr, void (*entry)(void *), 
		void * param, TaskStack_t * stack)
{
	// will be saved by hardware
	// will never store local variables. After return, the local variables will be released
	*(--stack) = (unsigned long) (1<<24);
	*(--stack) = (unsigned long) entry;
	*(--stack) = (unsigned long) 0x14;
	*(--stack) = (unsigned long) 0x12;
	*(--stack) = (unsigned long) 0x03;
	*(--stack) = (unsigned long) 0x02;
	*(--stack) = (unsigned long) 0x01;
	*(--stack) = (unsigned long) param;
	
	// need to be saved by user
	*(--stack) = (unsigned long) 0x11;
	*(--stack) = (unsigned long) 0x10;
	*(--stack) = (unsigned long) 0x09;
	*(--stack) = (unsigned long) 0x08;
	*(--stack) = (unsigned long) 0x07;
	*(--stack) = (unsigned long) 0x06;
	*(--stack) = (unsigned long) 0x05;
	*(--stack) = (unsigned long) 0x04;
	
	tTaskPtr->stackPtr = stack;
}

int taks1Flag;
void task1(void * param)
{
	for(;;)
	{
		taks1Flag = 0;
		delay(100);
		taks1Flag = 1;
		delay(100);
	}
}

int taks2Flag;
void task2(void * param)
{
	for(;;)
	{
		taks2Flag = 0;
		delay(100);
		taks2Flag = 1;
		delay(100);
	}
}

void SysTick_Handler(void)
{
	TaskSched();
}


int main(void)
{
	// Init SysTick
	SysTick->LOAD = SystemCoreClock/100 -1;
	NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS)-1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
	
	
	// Initial task
	TaskInit(&tcbtask1, task1, (void *)0x11111111, &task1Env[1024]);
	TaskInit(&tcbtask2, task2, (void *)0x22222222, &task2Env[1024]);
	
	taskTable[0] = &tcbtask1;
	taskTable[1] = &tcbtask2;
	
	nxtTask = taskTable[0];
	
	TaskRun();
	
	return 0;		// will never be called
}
