#include "myOS.h"


void EventInit(event_t * event, eventType_t type)
{
	event->type = type;
	ListInit(&event->waitList);
}

void EventWait(event_t * event, task_t * task, void * msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = TaskEnterCritical();
	
	task->state |= state;
	task->eventMsg = msg;
	task->waitEvent = event;
	task->waitEventResult = ErrorNoError;
	
	TaskSchedUnRdy(task);
	
	ListAddLast(&event->waitList, &task->linkNode);
	
	if (timeout)
	{
		TimeTaskWait(task, timeout);
	}
	
	TaskExitCritical(status);
}

task_t * EventWakeUp(event_t * event, void * msg, uint32_t result)
{
	node_t * node;
	task_t * task = (task_t *)0;
	
	uint32_t status = TaskEnterCritical();
	
	if ((node = ListRemoveFirst(&event->waitList)) != (node_t *)0)		// that's why the ListRemoveFirst function will return (node_t *)0 when empty
	{
		task = nodeParent(node, task_t, linkNode);
		task->waitEventResult = result;
		task->waitEvent = (event_t *)0;
		task->state &= ~MYOS_TASK_STATE_WAIT_MASK;
		task->eventMsg = msg;
		
		if (task->delayTicks > 0)
		{
			TimeTaskWakeUp(task);
		}
	}
	
	TaskSchedRdy(task);
	
	TaskExitCritical(status);
	
	return task;
}

// this function is called in systick handler when task wait event timeout,
// so there is no need to judge the task->delayticks and wake up task
void EventRemoveTask(task_t * task, void * msg, uint32_t result)
{
	uint32_t status = TaskEnterCritical();
	
	ListRemove(&task->waitEvent->waitList, &task->linkNode);
	task->waitEventResult = result;
	task->waitEvent = (event_t *)0;
	task->state &= ~MYOS_TASK_STATE_WAIT_MASK;
	task->eventMsg = msg;
	
	TaskExitCritical(status);
}

uint32_t EventRemoveAll(event_t * event, void * msg, uint32_t result)
{
	uint32_t count = ListCount(&event->waitList);
	
	node_t * node;
	
	uint32_t status = TaskEnterCritical();
	
	while((node = ListRemoveFirst(&event->waitList)) != (node_t *)0)
	{
		task_t * task = nodeParent(node, task_t, linkNode);
		task->waitEventResult = result;
		task->waitEvent = (event_t *)0;
		task->state &= ~MYOS_TASK_STATE_WAIT_MASK;
		task->eventMsg = msg;
		
		if (task->delayTicks > 0)
		{
			TimeTaskWakeUp(task);
		}
		
		TaskSchedRdy(task);
	}
	
	TaskExitCritical(status);
	
	return count;
}
	
uint32_t EventCount(event_t * event)
{
	uint32_t count = 0;
	uint32_t status = TaskEnterCritical();
	
	count = ListCount(&event->waitList);
	
	TaskExitCritical(status);
	return count;
}



