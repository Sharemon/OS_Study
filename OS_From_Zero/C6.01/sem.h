#ifndef _SEM_H
#define _SEM_H

typedef struct _sem_t
{
	event_t event;
	uint32_t count;
	uint32_t maxCount;
}sem_t;


void SemInit(sem_t * sem, uint32_t startCount, uint32_t maxCount);

#endif
