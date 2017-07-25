#ifndef _SEM_H
#define _SEM_H

typedef struct _sem_t
{
	event_t event;
	uint32_t count;
	uint32_t maxCount;
}sem_t;

typedef struct _semInfo_t
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;
}semInfo_t;


void SemInit(sem_t * sem, uint32_t startCount, uint32_t maxCount);
uint32_t SemWait(sem_t * sem, uint32_t waitTicks);
uint32_t SemNoWaitGet(sem_t * sem);
void SemNotify(sem_t * sem);
uint32_t SemDestroy(sem_t * sem);
void SemGetInfo(sem_t * sem, semInfo_t * semInfo);


#endif
