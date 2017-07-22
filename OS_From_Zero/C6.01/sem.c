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
