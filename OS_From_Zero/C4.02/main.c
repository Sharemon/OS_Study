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
TaskStack_t IdleEnv[MYOS_IDLETASK_STACK_SIZE];

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

void TimeTaskRemove (Task_t * task)
{
	ListRemove(&listTaskDelay, &(task->delayNode));
}


void TaskSystemTickHandler()
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

void TaskSchedRemove(Task_t * task)
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




//---------------- MAIN ------------------

int main(void)
{
	TaskSchedInit();
	
	// Init delay list
	ListInit(&(listTaskDelay));
	
	InitApp();

	TaskInit(&tcbIdle, IdleTask, (void *)0, &IdleEnv[MYOS_IDLETASK_STACK_SIZE], MYOS_PRIO_MAX_COUNT-1);
	
	idleTask = &tcbIdle;
	
	nxtTask = TaskHighestReady();
	
	TaskRun();
	
	return 0;		// will never be called
}

//---------------- MAIN ------------------
