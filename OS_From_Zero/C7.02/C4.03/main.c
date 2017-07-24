#include <stdio.h>
#include "myOS.h"
#include "tLib.h"
#include "tConfig.h"
#include "ARMCM3.h"



int tickCounter;
void TaskSched(void);

//--------------------- OS ----------------------

task_t * curTask;
task_t * nxtTask;

bitmap_t taskPrioBitmap;
list_t listTaskTable[MYOS_PRIO_MAX_COUNT];

task_t * idleTask;
task_t tcbIdle;
taskStack_t IdleEnv[MYOS_IDLETASK_STACK_SIZE];

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

void TimeTaskWait (task_t * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	ListAddLast(&listTaskDelay, &(task->delayNode));
	task->state |= MYOS_TASK_STATE_DELAYED;
}

void TimeTaskWakeUp (task_t * task)
{
	ListRemove(&listTaskDelay, &(task->delayNode));
	task->state &= ~MYOS_TASK_STATE_DELAYED;
}

void TimeTaskRemove (task_t * task)
{
	ListRemove(&listTaskDelay, &(task->delayNode));
}


void TaskSystemTickHandler()
{
	node_t * node;
	
	uint32_t status  = TaskEnterCritical();
	for(node=listTaskDelay.headNode.postNode; node!=&(listTaskDelay.headNode); node=node->postNode)
	{
		task_t * task = nodeParent(node, task_t, delayNode);
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



task_t * TaskHighestReady(void)
{
	uint32_t highestPrio = BitmapGetFirstSet(&(taskPrioBitmap));
	node_t * node = ListFirst(&(listTaskTable[highestPrio]));
	return (nodeParent(node, task_t, linkNode));
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

void TaskSchedRdy (task_t * task)
{
	ListAddFirst(&(listTaskTable[task->prio]), &(task->linkNode));
	BitmapSet(&taskPrioBitmap, task->prio);
}

void TaskSchedUnRdy (task_t * task)
{
	ListRemove(&listTaskTable[task->prio], &(task->linkNode));
	if (ListCount(&listTaskTable[task->prio]) == 0)
	{
		BitmapClear(&taskPrioBitmap, task->prio);
	}
}

void TaskSchedRemove(task_t * task)
{
	ListRemove(&listTaskTable[task->prio], &(task->linkNode));
	if (ListCount(&listTaskTable[task->prio]) == 0)
	{
		BitmapClear(&taskPrioBitmap, task->prio);
	}
}




void TaskSched()
{		
	task_t * tempTask;
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
