#include <stdio.h>
#include "myOS.h"
#include "ARMCM3.h"

//--------------------- OS ----------------------

Task_t * curTask;
Task_t * nxtTask;
Task_t * taskTable[2];
Task_t * idleTask;

Task_t tcbtask1;
Task_t tcbtask2;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];


void TaskSched()
{		
	if (curTask == idleTask)
	{
		if (taskTable[0]->delayTicks == 0) nxtTask = taskTable[0];
		else if (taskTable[1]->delayTicks == 0) nxtTask = taskTable[1];
		else return;
	}
	else if (curTask == taskTable[0])
	{
		if (taskTable[1]->delayTicks == 0) nxtTask = taskTable[1];
		else if (curTask->delayTicks != 0) nxtTask = idleTask;
		else return;
	}
	else if (curTask == taskTable[1])
	{
		if (taskTable[0]->delayTicks == 0) nxtTask = taskTable[0];
		else if (curTask->delayTicks != 0) nxtTask = idleTask;
		else return;
	}
	
	
	TaskSwitch();
}


void SysTick_Handler(void)
{
	int i;
	for(i=0; i<2; i++)
	{
		if (taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks --;
		}
	}
	
	TaskSched();
}


void OSdelay(uint32_t delay)
{
	curTask->delayTicks = delay;
	
	TaskSched();
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

//--------------------- OS ----------------------


//---------------- TASK ------------------
Task_t tcbtask1;
Task_t tcbtask2;
Task_t tcbIdle;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];
TaskStack_t IdleEnv[1024];


int taks1Flag;
void Task1(void * param)
{
	for(;;)
	{
		taks1Flag = 0;
		OSdelay(1);
		taks1Flag = 1;
		OSdelay(1);
	}
}

int taks2Flag;
void Task2(void * param)
{
	for(;;)
	{
		taks2Flag = 0;
		OSdelay(1);
		taks2Flag = 1;
		OSdelay(1);
	}
}

void IdleTask(void * param)
{
	for(;;){}
}

//---------------- TASK ------------------

//---------------- MAIN ------------------

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
	TaskInit(&tcbtask1, Task1, (void *)0x11111111, &task1Env[1024]);
	TaskInit(&tcbtask2, Task2, (void *)0x22222222, &task2Env[1024]);
	TaskInit(&tcbIdle, IdleTask, (void *)0, &IdleEnv[1024]);
	
	taskTable[0] = &tcbtask1;
	taskTable[1] = &tcbtask2;
	idleTask = &tcbIdle;
	
	nxtTask = taskTable[0];
	
	TaskRun();
	
	return 0;		// will never be called
}

//---------------- MAIN ------------------
