#include "myOS.h"


void TaskInit(task_t * tTaskPtr, void (*entry)(void *), 
		void * param, taskStack_t * stack, uint32_t prio)
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
	tTaskPtr->suspendCount = 0;
	
	tTaskPtr->clean = (void (*)(void *))0;
	tTaskPtr->cleanParam = (void *)0;
	tTaskPtr->requestDeleteFlag = 0;
	
	NodeInit(&(tTaskPtr->delayNode));
	NodeInit(&(tTaskPtr->linkNode));
	
	TaskSchedRdy(tTaskPtr);
}


void TaskSuspend (task_t * task)
{
	uint32_t status = TaskEnterCritical();
	
	if (!(task->state & MYOS_TASK_STATE_DELAYED))
	{
		if (++(task->suspendCount) <= 1)
		{
			TaskSchedUnRdy(task);
			task->state |= MYOS_TASK_STATE_SUSPENDED;
			
			if (curTask == task)
			{
				TaskSched();
			}
		}
	}
	
	TaskExitCritical(status);
}

void TaskWakeUp (task_t * task)
{
	uint32_t status = TaskEnterCritical();
	
	if (task->state & MYOS_TASK_STATE_SUSPENDED)
	{
		if (--(task->suspendCount) == 0)
		{
			TaskSchedRdy(task);
			task->state &= ~MYOS_TASK_STATE_SUSPENDED;

			TaskSched();
		}
	}
	
	TaskExitCritical(status);
}

void TaskSetCleanCallFunc(task_t * task, void (*clean)(void * param), void * param)
{
	task->clean = clean;
	task->cleanParam = param;
}

void TaskForceDelete(task_t * task)
{
	uint32_t status = TaskEnterCritical();
	
	if (task->state & MYOS_TASK_STATE_DELAYED)
	{
		TimeTaskRemove(task);
	}
	else if (! (task->state & MYOS_TASK_STATE_SUSPENDED))
	{
		TaskSchedRemove(task);
	}
	
	if (task->clean)
	{
		task->clean(task->cleanParam);
	}
	
	if (curTask == task)
	{
		TaskSched();
	}
	
	
	TaskExitCritical(status);
}

void TaskRequestDelete(task_t * task)
{
	uint32_t status = TaskEnterCritical();
	
	task->requestDeleteFlag = 1;
	
	TaskExitCritical(status);
}

// this function can only be calle by current task
// it's not meaningful to check other task's delete request flag
uint8_t TaskIsRequestDeleted(void)
{
	uint8_t deleted;
	
	uint32_t status = TaskEnterCritical();
	
	deleted = curTask->requestDeleteFlag;
	
	TaskExitCritical(status);
	
	return deleted;
}


void TaskDeleteSelf(void)
{
	uint32_t status = TaskEnterCritical();
	
	TaskSchedRemove(curTask);
	
	if (curTask->clean)
	{
		curTask->clean(curTask->cleanParam);
	}
	
	TaskSched();
	
	TaskExitCritical(status);	
}

void TaskGetInfo(task_t * task, taskInfo_t * taskInfo)
{
	uint32_t status = TaskEnterCritical();
	
	taskInfo->delayTicks = task->delayTicks;
	taskInfo->prio = task->prio;
	taskInfo->state = task->state;
	taskInfo->slice = task->slice;
	taskInfo->suspendCount = task->suspendCount;
	
	TaskExitCritical(status);	
}


