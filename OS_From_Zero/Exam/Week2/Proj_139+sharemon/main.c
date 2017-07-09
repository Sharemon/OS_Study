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
	//=================== Change for delay implementation =====================
	if (listTaskDelay.nodeCount == 0)
	{
		task->delayTicks = ticks;
		ListAddFirst(&listTaskDelay, &(task->delayNode));
	}
	else
	{
		node_t * node;
		uint32_t tickRemain = ticks;
		
		// in single linked list mode, we cannot touch the preNode
		// so need to judge the tick in advance
		for (node = &(listTaskDelay.headNode); ; node=node->postNode)
		{
			// if node is the last node, insert node at the end of listTaskDelay
			if (node->postNode == &(listTaskDelay.headNode))
			{
				task->delayTicks = tickRemain;
				ListInsertAfter(&listTaskDelay, node, &(task->delayNode));
				
				break;
			}
			else
			{
				task_t * tempTask = nodeParent(node->postNode, task_t, delayNode);
				
				if (tickRemain < tempTask->delayTicks)
				{
					task->delayTicks = tickRemain;
					ListInsertAfter(&listTaskDelay, node, &(task->delayNode));
					
					tempTask->delayTicks -= tickRemain;
					break;
				}
				
				tickRemain = tickRemain - tempTask->delayTicks;
			}
		}
	}
	//=================== Change for delay implementation =====================
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
	
	task_t * task = nodeParent(listTaskDelay.headNode.postNode, task_t, delayNode);
	
	uint32_t status  = TaskEnterCritical();
	
	//=================== Change for delay implementation =====================
	if (--(task->delayTicks) == 0)
	{
			TimeTaskWakeUp(task);
			
			TaskSchedRdy(task);
	}
	
	// Loop to find if there is any other ready task
	// As the delay tick could be 0 if the task have the same delay the first node
	// This loop may not be execute if there is not equal delay tick
	for (node=listTaskDelay.headNode.postNode;node!=&(listTaskDelay.headNode);node=node->postNode)
	{
		task = nodeParent(node, task_t, delayNode);
		if (task->delayTicks == 0)
		{
			TimeTaskWakeUp(task);
			
			TaskSchedRdy(task);
		}
		else
		{
			break;
		}
	}
	//=================== Change for delay implementation =====================
		
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
