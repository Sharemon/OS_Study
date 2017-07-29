#include "myOS.h"

void MemBlockInit(memBlock_t * memblock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt)
{
	uint8_t * memBlockStart = memStart;
	uint8_t * memBlockEnd = memStart + blockSize * blockCnt;
	
	if (blockSize < sizeof(node_t))
	{
		return;
	}
	
	EventInit(&memblock->event, EventTypeMemBlock);
	memblock->memStart = memBlockStart;
	memblock->blockSize = blockSize;
	memblock->maxCount = blockCnt;
	
	ListInit(&memblock->blockList);
	while(memBlockStart < memBlockEnd)
	{
		NodeInit((node_t *)memBlockStart);
		ListAddLast(&memblock->blockList, (node_t *)memBlockStart);
		
		memBlockStart += blockSize;
	}
}

uint32_t memBlock_tWait (memBlock_t * memBlock, uint8_t **mem, uint32_t waitTicks)
{
	uint32_t status = TaskEnterCritical();
	
	if (ListCount(&memBlock->blockList) > 0)
	{
		*mem = (uint8_t *)ListRemoveFirst(&memBlock->blockList);
		TaskExitCritical(status);
		return ErrorNoError;
	}
	else
	{
		EventWait(&memBlock->event, curTask, (void *)0, EventTypeMemBlock, waitTicks);
		TaskExitCritical(status);
		
		TaskSched();
		
		*mem = curTask->eventMsg;
		return curTask->waitEventResult;
	}
}

uint32_t memBlock_tNoWaitGet (memBlock_t * memBlock, void ** mem)
{
	uint32_t status = TaskEnterCritical();
	
	if (ListCount(&memBlock->blockList) > 0)
	{
		*mem = (uint8_t *)ListRemoveFirst(&memBlock->blockList);
		TaskExitCritical(status);
		return ErrorNoError;
	}
	else
	{
		TaskExitCritical(status);
		return ErrorResourceUnavaliable;
	}
}

void memBlock_tNotify (memBlock_t * memBlock, uint8_t * mem)
{
	uint32_t status = TaskEnterCritical();
	
	if (EventWaitCount(&memBlock->event) > 0)
	{
		task_t * task = EventWakeUp(&memBlock->event, (void *)mem, ErrorNoError);
		if (task->prio < curTask->prio)
		{
			TaskSched();
		}
	}
	else 
	{
		ListAddLast(&memBlock->blockList, (node_t *)mem);
	}
	TaskExitCritical(status);
}

void memBlock_tGetInfo (memBlock_t * memBlock, memBlockInfo_t * info)
{
	uint32_t status = TaskEnterCritical();
	
	info->count = ListCount(&memBlock->blockList);
	info->maxCount = memBlock->maxCount;
	info->blockSize = memBlock->blockSize;
	info->taskCount = EventWaitCount(&memBlock->event);
	
	TaskExitCritical(status);
}

uint32_t memBlock_tDestroy (memBlock_t * memBlock)
{
	uint32_t status = TaskEnterCritical();
	uint32_t count = EventRemoveAll(&memBlock->event, (void *)0, ErrorDel);
	TaskExitCritical(status);
	
	if (count > 0)
	{
		TaskSched();
	}
	return count;
}

