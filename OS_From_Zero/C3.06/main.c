#include <stdio.h>
#include "myOS.h"
#include "tLib.h"
#include "tConfig.h"
#include "ARMCM3.h"



int tickCounter;
void TaskSched(void);

//--------------------- OS ----------------------

Task_t * curTask;
Task_t * nxtTask;

bitmap_t taskPrioBitmap;
Task_t * taskTable[MYOS_PRO_MAX_COUNT];

Task_t * idleTask;
Task_t tcbIdle;
TaskStack_t IdleEnv[1024];

list_t listTaskDelay;

int schedLockCount;

uint32_t TaskEnterCritical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	
	return primask;
}

void TaskExitCritical(uint32_t primask)
{
	__set_PRIMASK(primask);
}


void SysTick_Handler(void)
{
	node_t * node;
	
	uint32_t status  = TaskEnterCritical();
	for(node=listTaskDelay.headNode.postNode; node!=&(listTaskDelay.headNode); node=node->postNode)
	{
		Task_t * task = nodeParent(node, Task_t, delayNode);
		if (--(task->delayTicks) == 0)
		{
			// remove delay node from delay list
			ListRemove(&(listTaskDelay), node);
			
			// clear delay state
			task->state &= ~MYOS_TASK_STATE_DELAYED;
			
			// add task to task rdy table
			taskTable[task->prio] = task;
			
			// set task prio ready bitmap
			BitmapSet(&(taskPrioBitmap), task->prio);
		}
	}
		
	
	
	TaskExitCritical(status);
	
	TaskSched();
}


void OSdelay(uint32_t delay)
{
	uint32_t status  = TaskEnterCritical();
	
	curTask->delayTicks = delay;
	ListAddFirst(&(listTaskDelay), &(curTask->delayNode));
	curTask->state |= MYOS_TASK_STATE_DELAYED;
	taskTable[curTask->prio] = (Task_t *)0;
	BitmapClear(&taskPrioBitmap, curTask->prio);
	
	TaskExitCritical(status);
	
	TaskSched();
}


void TaskInit(Task_t * tTaskPtr, void (*entry)(void *), 
		void * param, TaskStack_t * stack, uint32_t prio)
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
	tTaskPtr->delayTicks = 0;
	tTaskPtr->prio = prio;
	tTaskPtr->state = MYOS_TASK_STATE_RDY;
	
	NodeInit(&(tTaskPtr->delayNode));
	
	taskTable[prio] = tTaskPtr;
	BitmapSet(&taskPrioBitmap, prio);
}

Task_t * TaskHighestReady(void)
{
	return taskTable[BitmapGetFirstSet(&taskPrioBitmap)];
}

void TaskSchedInit(void)
{
	schedLockCount = 0;
	BitmapInit(&taskPrioBitmap);
}

void TaskSched()
{		
	Task_t * tempTask;
	uint32_t status = TaskEnterCritical();
	
	if (schedLockCount >0)
	{
		TaskExitCritical(status);
		return;
	}
	
	tempTask = TaskHighestReady();
	if (tempTask != curTask)
	{
		nxtTask = tempTask;
		TaskSwitch();
	}
	
	TaskExitCritical(status);
}


void IdleTask(void * param)
{
	for(;;){}
}
//--------------------- OS ----------------------


void SysTickInit(uint32_t n)
{
	// Init SysTick
	SysTick->LOAD = n * SystemCoreClock/1000 -1;
	NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS)-1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}

//---------------- TASK ------------------
Task_t tcbtask1;
Task_t tcbtask2;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];


int taks1Flag;
void Task1(void * param)
{
	SysTickInit(10);

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

//---------------- TASK ------------------

//---------------- MAIN ------------------

int main(void)
{
	// Init delay list
	ListInit(&(listTaskDelay));
	
	// Initial task
	TaskInit(&tcbtask1, Task1, (void *)0x11111111, &task1Env[1024], 0);
	TaskInit(&tcbtask2, Task2, (void *)0x22222222, &task2Env[1024], 1);
	TaskInit(&tcbIdle, IdleTask, (void *)0, &IdleEnv[1024], MYOS_PRO_MAX_COUNT-1);
	
	idleTask = &tcbIdle;
	
	nxtTask = TaskHighestReady();
	
	TaskRun();
	
	return 0;		// will never be called
}

//---------------- MAIN ------------------
