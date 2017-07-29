#ifndef _MEM_H
#define _MEM_H

typedef struct _memBlock_t
{
	event_t event;
	void * memStart;
	uint32_t blockSize;
	uint32_t maxCount;
	list_t blockList;
	
}memBlock_t;

typedef struct _memBlockInfo_t
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t blockSize;
	uint32_t taskCount;
}memBlockInfo_t;

void MemBlockInit(memBlock_t * memblock, uint8_t * memStart, uint32_t blockSize, uint32_t maxCount);
uint32_t tMemBlockWait (memBlock_t * memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet (memBlock_t * memBlock, void ** mem);
void tMemBlockNotify (memBlock_t * memBlock, uint8_t * mem);
void tMemBlockGetInfo (memBlock_t * memBlock, memBlockInfo_t * info);
uint32_t tMemBlockDestroy (memBlock_t * memBlock);

#endif
