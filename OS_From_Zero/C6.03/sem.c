#include "myOS.h"

void SemInit(sem_t * sem, uint32_t startCount, uint32_t maxCount)
{
	EventInit(&sem->event, EventTypeSem);
	
	sem->maxCount = maxCount;
	
	if ((startCount <= maxCount) || (maxCount == 0))
	{
		sem->count = startCount;
	}
	else 
	{
		sem->count = maxCount;
	}
}

uint32_t SemWait(sem_t * sem, uint32_t waitTicks)
{
	uint32_t status = TaskEnterCritical();
	
	if (sem->count > 0)
	{
		--sem->count;
		TaskExitCritical(status);
		
		return ErrorNoError;
	}
	else
	{
		EventWait(&sem->event, curTask, (void *)0, EventTypeSem, waitTicks);
		
		TaskExitCritical(status);
		
		TaskSched();
		
		// 这里有个有趣的地方，当执行完TaskSched（）之后，程序会跳转到其他任务执行。
		// 而只有信号量被释放或者事件超时的时候才会回到这个位置继续执行这个任务，
		// 也就是说，从行为来看，这个任务开始等待信号量之后，这个任务就被挂起，过一段时间（有可能是信号量释放，也有可能是timout）
		// 该任务被wakeup，再次执行的时候，是从此处开始执行，此时waitEventResutl已经被改写，可以直接读取使用。
		return curTask->waitEventResult;
		
	}
	
}


uint32_t SemNoWaitGet(sem_t * sem)
{
	uint32_t status = TaskEnterCritical();
	
	if (sem->count > 0)
	{
		--sem->count;
		TaskExitCritical(status);
		
		return ErrorNoError;
	}
	else
	{
		TaskExitCritical(status);
		
		return ErrorResourceUnavaliable;
	}
	
	
	
}


void SemNotify(sem_t * sem)
{
	uint32_t status = TaskEnterCritical();
	
	if (EventWaitCount(&sem->event)>0)
	{
		task_t * task = EventWakeUp(&sem->event, (void *)0, ErrorNoError);
		if (task->prio > curTask->prio)
		{
			TaskSched();
		}
	}
	else
	{
		++sem->count;
		if ((sem->maxCount != 0) && (sem->count > sem->maxCount))
		{
			sem->count = sem->maxCount;
		}
	}
	
	TaskExitCritical(status);
}


uint32_t SemDestroy(sem_t * sem)
{
	int count = 0;

	uint32_t status = TaskEnterCritical();
	
	count = EventRemoveAll(&sem->event, (void *)0, ErrorDel);
	sem->count = 0;

	TaskExitCritical(status);

	if (count > 0)
	{
		TaskSched();
	}

	return count;
}


void SemGetInfo(sem_t * sem, semInfo_t * semInfo)
{
	uint32_t status = TaskEnterCritical();
	
	semInfo->count = sem->count;
	semInfo->maxCount = sem->maxCount;
	semInfo->taskCount = EventWaitCount(&sem->event);

	TaskExitCritical(status);
}


