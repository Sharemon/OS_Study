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
		
		// �����и���Ȥ�ĵط�����ִ����TaskSched����֮�󣬳������ת����������ִ�С�
		// ��ֻ���ź������ͷŻ����¼���ʱ��ʱ��Ż�ص����λ�ü���ִ���������
		// Ҳ����˵������Ϊ�������������ʼ�ȴ��ź���֮���������ͱ����𣬹�һ��ʱ�䣨�п������ź����ͷţ�Ҳ�п�����timout��
		// ������wakeup���ٴ�ִ�е�ʱ���ǴӴ˴���ʼִ�У���ʱwaitEventResutl�Ѿ�����д������ֱ�Ӷ�ȡʹ�á�
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


