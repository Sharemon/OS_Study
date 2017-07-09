#include "myOS.h"


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
	
	TaskSchedRdy(tTaskPtr);
}


void TaskSuspend (Task_t * task)
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

void TaskWakeUp (Task_t * task)
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



