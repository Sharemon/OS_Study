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
list_t listTaskTable[MYOS_PRIO_MAX_COUNT];

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

void TaskDelayedInit (void)
{
	ListInit(&listTaskDelay);
}

void TimeTaskWait (Task_t * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	ListAddLast(&listTaskDelay, &(task->delayNode));
	task->state |= MYOS_TASK_STATE_DELAYED;
}

void TimeTaskWakeUp (Task_t * task)
{
	ListRemove(&listTaskDelay, &(task->delayNode));
	task->state &= ~MYOS_TASK_STATE_DELAYED;
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
			ListAddFirst(&(listTaskTable[task->prio]), &(task->linkNode));
			
			// set task prio ready bitmap
			BitmapSet(&(taskPrioBitmap), task->prio);
		}
	}
		
	if (--(curTask->slice) == 0)
	{
		// if there is more task in the ready list, put the current task to the end of list
		if (listTaskTable[curTask->prio].nodeCount > 1)
		{
			ListRemoveFirst(&(listTaskTable[curTask->prio]));
			ListAddLast(&(listTaskTable[curTask->prio]), &(curTask->linkNode));
		}
		
		// reset current task slice
		curTask->slice = MYOS_SLICE_MAX;
	}
	
	TaskExitCritical(status);
	
	TaskSched();
}


void OSdelay(uint32_t delay)
{
	uint32_t status  = TaskEnterCritical();
	
	curTask->delayTicks = delay;
	
	// add task to delay list
	ListAddFirst(&(listTaskDelay), &(curTask->delayNode));
	
	// set task state to delay state
	curTask->state |= MYOS_TASK_STATE_DELAYED;
	
	// remove task from ready list
	ListRemove(&(listTaskTable[curTask->prio]), &(curTask->linkNode));
	
	// if there is no task in ready list, clear prio bitmap
	if (listTaskTable[curTask->prio].nodeCount == 0)
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
	
	tTaskPtr->slice = MYOS_SLICE_MAX;
	tTaskPtr->stackPtr = stack;
	tTaskPtr->delayTicks = 0;
	tTaskPtr->prio = prio;
	tTaskPtr->state = MYOS_TASK_STATE_RDY;
	
	NodeInit(&(tTaskPtr->delayNode));
	NodeInit(&(tTaskPtr->linkNode));
	
	ListAddFirst(&(listTaskTable[prio]), &(tTaskPtr->linkNode));
	BitmapSet(&taskPrioBitmap, prio);
}

Task_t * TaskHighestReady(void)
{
	uint32_t highestPrio = BitmapGetFirstSet(&(taskPrioBitmap));
	node_t * node = ListFirst(&(listTaskTable[highestPrio]));
	return (nodeParent(node, Task_t, linkNode));
}

void TaskSchedInit(void)
{
	int i;
	
	schedLockCount = 0;
	BitmapInit(&taskPrioBitmap);
	for (i=0; i<MYOS_PRIO_MAX_COUNT; i++)
	{
		ListInit(&(listTaskTable[i]));
	}
}

void TaskSchedDisable (void)
{
	uint32_t status = TaskEnterCritical();
	
	if (schedLockCount < 255)
		schedLockCount++;
	
	TaskExitCritical(status);
}

void TaskSchedEnable (void)
{
	uint32_t status = TaskEnterCritical();
	
	if (schedLockCount > 0)
	{
		if (--schedLockCount == 0)
			TaskSched();
	}
	
	TaskExitCritical(status);
}

void TaskSchedRdy (Task_t * task)
{
	ListAddFirst(&(listTaskTable[task->prio]), &(task->linkNode));
	BitmapSet(&taskPrioBitmap, task->prio);
}

void TaskSchedUnRdy (Task_t * task)
{
	ListRemove(&listTaskTable[task->prio], &(task->linkNode));
	if (ListCount(&listTaskTable[task->prio]) == 0)
	{
		BitmapClear(&taskPrioBitmap, task->prio);
	}
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
Task_t tcbtask3;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];
TaskStack_t task3Env[1024];


int taks1Flag;
void Task1(void * param)
{
	SysTickInit(1);

	for(;;)
	{		
		taks1Flag = 0;
		OSdelay(1);
		taks1Flag = 1;
		OSdelay(1);
		
	}
}

void delay(uint32_t n)
{
	while(--n);
}

int taks2Flag;
void Task2(void * param)
{
	for(;;)
	{

		taks2Flag = 0;
		delay(500);//OSdelay(1);
		taks2Flag = 1;
		delay(500);//OSdelay(1);
	}
}

int taks3Flag;
void Task3(void * param)
{
	for(;;)
	{

		taks3Flag = 0;
		delay(500);//OSdelay(1);
		taks3Flag = 1;
		delay(500);//OSdelay(1);
	}
}

//---------------- TASK ------------------

//---------------- MAIN ------------------

int main(void)
{
	TaskSchedInit();
	
	// Init delay list
	ListInit(&(listTaskDelay));
	
	// Initial task
	TaskInit(&tcbtask1, Task1, (void *)0x11111111, &task1Env[1024], 0);
	TaskInit(&tcbtask2, Task2, (void *)0x22222222, &task2Env[1024], 1);
	TaskInit(&tcbtask3, Task3, (void *)0x33333333, &task3Env[1024], 1);
	TaskInit(&tcbIdle, IdleTask, (void *)0, &IdleEnv[1024], MYOS_PRIO_MAX_COUNT-1);
	
	idleTask = &tcbIdle;
	
	nxtTask = TaskHighestReady();
	
	TaskRun();
	
	return 0;		// will never be called
}

//---------------- MAIN ------------------
